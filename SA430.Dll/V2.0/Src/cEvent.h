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
#ifndef CEVENT_H
#define CEVENT_H
#include <Windows.h>

/*!
 \brief

 \class cEvent cEvent.h "cEvent.h"
*/
class cEvent
{
public:
  /*!
   \brief Constructor

   \fn cEvent
   \param manual
  */
  cEvent(bool manual);
  /*!
   \brief Destructor

   \fn ~cEvent
   \param void
  */
  ~cEvent(void);

  /*!
   \brief Set an event to signaled

   \fn Signal
   \param void
  */
  void Signal(void);
  /*!
   \brief Wait for an event -- wait for an event object to be set to signaled

   \fn Wait
   \param void
  */
  bool Wait(void);
  /*!
   \brief CheckSignal

   \fn CheckSignal
   \param ms
  */
  bool CheckSignal(DWORD ms);
  /*!
   \brief Check

   \fn Check
   \param void
  */
  bool Check(void);
  /*!
   \brief Reset an event flag to unsignaled

   \fn Reset
   \param void
  */
  void Reset(void);

  bool m_bCreated;  /*!< TODO */
  HANDLE m_event;   /*!< TODO */
private:

};
#endif // CEVENT_H
