/* --COPYRIGHT--,BSD
 * Copyright (c) 2011, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#include <Windows.h>
#include "cMutex.h"

/*!
 \brief Constructor

 \fn cMutex::cMutex
 \param void
*/
cMutex::cMutex(void)
{
 hMutex = CreateMutex(NULL,false,NULL);
 if(!hMutex) 
	 bCreated = false;
 bCreated = true;
 dwOwner = 0;
}

/*!
 \brief Destructor

 \fn cMutex::~cMutex
 \param void
*/
cMutex::~cMutex(void)
{
 WaitForSingleObject(hMutex,INFINITE);
 CloseHandle(hMutex);
}

/*!
 \brief Lock

 \fn cMutex::Lock
*/
void cMutex::Lock()
{
 DWORD id = GetCurrentThreadId();
 if(id == dwOwner)
        return;
 WaitForSingleObject(hMutex,INFINITE);
 dwOwner = id;
}

/*!
 \brief Unlock

 \fn cMutex::Unlock
*/
void cMutex::Unlock()
{
 DWORD id = GetCurrentThreadId();
 if(!(id == dwOwner))
  return;

 dwOwner = 0;
 ReleaseMutex(hMutex);
}

