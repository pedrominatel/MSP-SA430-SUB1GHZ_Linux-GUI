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
#include "cevent.h"


/*!
 \brief Constructor

 \fn cEvent::cEvent
 \param manual
*/
cEvent::cEvent(bool manual):m_bCreated(TRUE)
{
 m_event = CreateEvent(NULL,manual,FALSE,NULL);
 if( !m_event )
  {
   m_bCreated = FALSE;
  };
}

/*!
 \brief Destructor

 \fn cEvent::~cEvent
 \param void
*/
cEvent::~cEvent(void)
{
 CloseHandle(m_event);
}

/*!
 \brief Set an event to signaled

 \fn cEvent::Signal
 \param void
*/
void cEvent::Signal(void)
{
 SetEvent(m_event);
}

/*!
 \brief Wait for an event -- wait for an event object to be set to signaled

 \fn cEvent::Wait
 \param void
 \return bool
*/
bool cEvent::Wait(void)
{
 if( WaitForSingleObject(m_event,INFINITE) != WAIT_OBJECT_0 )
  {
   return FALSE;
  }
 return TRUE;
}

/*!
 \brief CheckSignal

 \fn cEvent::CheckSignal
 \param ms
 \return bool
*/
bool cEvent::CheckSignal(DWORD ms)
{
 if( WaitForSingleObject(m_event,ms) != WAIT_OBJECT_0 )
  {
   return FALSE;
  }
 return TRUE;
}

/*!
 \brief Check

 \fn cEvent::Check
 \param void
 \return bool
*/
bool cEvent::Check(void)
{
 if( WaitForSingleObject(m_event,0) != WAIT_OBJECT_0 )
  {
   return FALSE;
  }
 return TRUE;
}

/*!
 \brief Reset an event flag to unsignaled

 \fn cEvent::Reset
 \param void
*/
void cEvent::Reset(void)
{
 ResetEvent(m_event);
}
