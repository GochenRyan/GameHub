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

#pragma once
#include "System.h"
#include "Synchronize.h"
#ifdef WINDOWS_PLATFORM
#include <Windows.h>
#endif // WINDOWS_PLATFORM
#include <new.h>
#define NOELLE_NEW new
#define NOELLE_DELETE delete
#define USE_STL_TYPE_TRAIT
#ifdef USE_STL_TYPE_TRAIT
#include <type_traits>
#endif // USE_STL_TYPE_TRAIT

namespace Noelle
{
	class MemManager
	{
	public:
		MemManager();
		virtual ~MemManager();

		// Allocate memory
		virtual void* Allocate(unsigned int uiSize, unsigned int uiAlignment, bool bIsArray) = 0;
		// Deallocate memory
		virtual void Deallocate(char* pcAddr, unsigned int uiAlignment, bool bIsArray) = 0;
	};

#if WINDOWS_PLATFORM && _WIN64 && _DEBUG
	class DebugMem : public MemManager
	{
	public:
		DebugMem();
		~DebugMem();

		virtual void* Allocate(unsigned int uiSize, unsigned int uiAlignment, bool bIsArray);
		virtual void Deallocate(char* pcAddr, unsigned int uiAlignment, bool bIsArray);

	private:
		enum
		{
			BEGIN_MASK = 0xDEADC0DE,
			END_MASK = 0xDEADC0DE,
			RECORD_NUM = 32,
			CALLSTACK_NUM = 32
		};

		class Block
		{
		public:
			Block()
			{
				for (unsigned int i = 0; i < CALLSTACK_NUM; i++)
				{
					pAddr[i] = nullptr;
				}
				m_pPrev = nullptr;
				m_pNext = nullptr;
			}
			void* pAddr[CALLSTACK_NUM];  // call stack information when allocating memory
			unsigned int m_uiStackInfoNum;  // call stack layers
			unsigned int m_uiSize;  // size of allocated memory
			bool m_bIsArray;
			bool m_bAlignment;
			Block* m_pPrev;
			Block* m_pNext;
		};

		Block* m_pHead;
		Block* m_pTail;

		unsigned int m_uiNumNewCalls; // the number of times "New" was called
		unsigned int m_uiNumDeleteCalls;  // the number of times "Delete" was called
		unsigned int m_uiNumBlocks;  // the number of current blocks 
		unsigned int m_uiNumBytes;  // the number of current bytes
		unsigned int m_uiMaxNumBytes;  // Maximum number of bytes requested
		unsigned int m_uiMaxNumBlocks;  // Maximum number of blocks requested

		unsigned int m_uiSizeRecord[RECORD_NUM];  // the number of memory requests recorded in the n power of 2 range
		void InsertBlock(Block* pBlock);
		void RemoveBlock(Block* pBlock);
		bool InitDbgHelpLib();
	};
#endif
}