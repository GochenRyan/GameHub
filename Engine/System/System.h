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
#include "Platform.h"
#if WINDOWS_PLATFORM
#include <tchar.h>
#include <memory.h>
#endif

namespace Noelle
{
	inline const TCHAR* Csrchr(const TCHAR* pString, int c)
	{
#ifdef WINDOWS_PLATFORM
		return _tcsrchr(pString, c);
#else
		static_assert(0, "No Implement!");;
		return NULL;
#endif
	}

	inline void Strcat(TCHAR* pDest, unsigned int uiCount, const TCHAR* pSource)
	{
#ifdef WINDOWS_PLATFORM
		_tcscat_s(pDest, uiCount, pSource);
#else
		static_assert(0, "No Implement!");;
		return;
#endif
	}
}