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


#include "cEvent.h"


cEvent::cEvent(bool manual):m_bCreated(TRUE)
{
 m_event = CreateEvent(NULL,manual,FALSE,NULL);
 if( !m_event )
  {
   m_bCreated = FALSE;
  };
}

cEvent::~cEvent(void)
{
 CloseHandle(m_event);
}


// Set an event to signaled
void cEvent::Signal(void)
{
 SetEvent(m_event);
}

// Wait for an event -- wait for an event object
// to be set to signaled
bool cEvent::Wait(void)
{
 if( WaitForSingleObject(m_event,INFINITE) != WAIT_OBJECT_0 )
  {
   return FALSE;
  }
 return TRUE;
}

bool cEvent::CheckSignal(DWORD ms)
{
 if( WaitForSingleObject(m_event,ms) != WAIT_OBJECT_0 )
  {
   return FALSE;
  }
 return TRUE;
}

bool cEvent::Check(void)
{
 if( WaitForSingleObject(m_event,0) != WAIT_OBJECT_0 )
  {
   return FALSE;
  }
 return TRUE;
}

// Reset an event flag to unsignaled
void cEvent::Reset(void)
{
 ResetEvent(m_event);
}
