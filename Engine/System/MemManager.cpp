// Copyright 2022 Gaocheng Ran <rangaocheng@outlook.com>

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "MemManager.h"
#include "System.h"
using namespace Noelle;

#if _DEBUG
#if WINDOWS_PLATFORM
#include <DbgHelp.h>
typedef BOOL
(WINAPI 
	*tFSymInitializeW)(
	_In_ HANDLE hProcess,
	_In_opt_ PCWSTR UserSearchPath,
	_In_ BOOL fInvadeProcess
	);

static HMODULE s_DbgHelpLib = nullptr;

bool DebugMem::InitDbgHelpLib()
{
	TCHAR szDbgName[MAX_PATH];
	GetModuleFileName(NULL, szDbgName, MAX_PATH);
	TCHAR* p = (TCHAR*)Csrchr(szDbgName, _T('\\'));
	if (p)
		*p = 0;
	Strcat(szDbgName, MAX_PATH, _T("\\dbghelp.dll"));

	// Find DLL of current directory
	s_DbgHelpLib = LoadLibrary(szDbgName);

	fnSymInitializeW = (tFSymInitializeW)GetProcAddress(s_DbgHelpLib, "SymInitializeW");
}

static tFSymInitializeW fnSymInitializeW = NULL;

#endif
#endif


void* DebugMem::Allocate(unsigned int uiSize, unsigned int uiAlignment, bool bIsArray)
{
	m_uiNumNewCalls++;

	// 
	unsigned int uiExtendedSize = sizeof(Block) + sizeof(unsigned int) + uiSize + sizeof(unsigned int);
	char* pcAddr = (char*)malloc(uiExtendedSize);
	if (!pcAddr)
		return nullptr;

	// 
	Block* pBlock = (Block*)pcAddr;
	pBlock->m_uiSize = uiSize;
	pBlock->m_bIsArray = bIsArray;

	bool bAlignment = (uiAlignment > 0) ? true : false;
	pBlock->m_bAlignment = bAlignment;

	//
	InsertBlock(pBlock);

	pcAddr += sizeof(Block);
	// 
	unsigned int* pBeginMask = (unsigned int*)(pcAddr);
	*pBeginMask = BEGIN_MASK;
	pcAddr += sizeof(unsigned int);
	// 
	unsigned int* pEndMask = (unsigned int*)(pcAddr + uiSize);
	*pEndMask = END_MASK;

	m_uiNumBlocks++;
	m_uiNumBytes += uiSize;

	if (m_uiNumBlocks > m_uiMaxNumBlocks)
	{
		m_uiMaxNumBlocks = m_uiNumBlocks;
	}

	if (m_uiNumBytes > m_uiMaxNumBytes)
	{
		m_uiMaxNumBytes = m_uiNumBytes;
	}

	unsigned int uiTwoPowerI = 1;
	int i;
	for (i = 0; i <= RECORD_NUM - 2; i++, uiTwoPowerI <<= 1)
	{
		if (uiSize <= uiTwoPowerI)
		{
			m_uiSizeRecord[i]++;
			break;
		}
	}
	if (i == RECORD_NUM - 1)
	{
		m_uiSizeRecord[i]++;
	}

	return (void*)pcAddr;
}

void DebugMem::Deallocate(char* pcAddr, unsigned int uiAlignment, bool bIsArray)
{
	m_uiNumDeleteCalls++;
	pcAddr -= sizeof(unsigned int);

	unsigned int* pBeginMask = (unsigned int*)(pcAddr);
	//NOELLE_ASSERT(*pBeginMask == BEGIN_MASK);

	pcAddr -= sizeof(Block);

	Block* pBlock = (Block*)pcAddr;
	RemoveBlock(pBlock);

	//NOELLE_ASSERT(pBlock->m_bIsArray == bIsArray);
	//NOELLE_ASSERT(m_uiNumBlocks > 0 && m_uiNumBytes >= pBlock->m_uiSize);
	bool bAlignment = (uiAlignment > 0) ? true : false;
	//NOELLE_ASSERT(pBlock->m_bAlignment == bAlignment);
	unsigned int* pEndMask = (unsigned int*)(pcAddr + sizeof(Block) + sizeof(unsigned int) + pBlock->m_uiSize);
	//NOELLE_ASSERT(*pEndMask == END_MASK);

	m_uiNumBlocks--;
	m_uiNumBytes -= pBlock->m_uiSize;

	free(pcAddr);
}

void DebugMem::InsertBlock(Block* pBlock)
{
	if (m_pHead == nullptr)
	{
		pBlock->m_pPrev = nullptr;
		pBlock->m_pNext = nullptr;
		m_pHead = pBlock;
		m_pTail = pBlock;
	}
	else
	{
		pBlock->m_pNext = nullptr;
		pBlock->m_pPrev = m_pTail;
		m_pTail->m_pNext = pBlock;
		m_pTail = pBlock;
	}
}

void DebugMem::RemoveBlock(Block* pBlock)
{
	if (pBlock->m_pPrev)
	{
		pBlock->m_pPrev->m_pNext = pBlock->m_pNext;
	}
	else
	{
		m_pHead = pBlock->m_pNext;
	}

	if (pBlock->m_pNext)
	{
		pBlock->m_pNext->m_pPrev = pBlock->m_pPrev;
	}
	else
	{
		m_pTail = pBlock->m_pPrev;
	}
}