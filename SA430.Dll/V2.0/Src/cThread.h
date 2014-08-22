/* --COPYRIGHT--, 
Permissive via Author: Emad Barsoum
15. September 2011
 * --/COPYRIGHT--*/
#include <windows.h>
#include <process.h>
#include <assert.h>

#include "cMutex.h"
#include "cEvent.h"

#pragma once 
//-- Thread.hpp --

/*--------------------------------------------------------------------------
                         Class Library

     Copyrights Emad Barsoum 2002. All rights reserved.
     ________________________________________________________________


     PROJECT   : General
     MODULE    : CThread, TThread
     FILENAME  : Thread.hpp
     REFERENCES: MSDN and ANSI/ISO C++ Professional Programmer's Handbook.
           BUILD     : 1

     History of Modifications:

     Date(dd/mm/yyyy)Person                Description
     ----            ------                -----------
     19/04/2002      Emad Barsoum          Initial design and coding
     23/04/2002      Emad Barsoum          Update some member function
     17/09/2002      Emad Barsoum          Remove Constructor Parameter not needed
     17/09/2002      Emad Barsoum          Add the TThread class which is a template thread class
     17/09/2002      Emad Barsoum          Add m_nInitPriority

     CLASS NAME: CThread, TThread
     CLASS TYPE: NORMAL


     DESCRIPTION:
        Those are two standard threads class, that encapsulate all the thread
      operation.
*/
/*!
 \brief

 \enum eTaskState
*/
enum eTaskState{
 INIT,
 READY,
 RUN,
 STOP,
 CLOSEING,
 FAULT
};



  /*!
   \brief

   \class cThread cThread.h "cThread.h"
  */
  class cThread
  {
  public:
    //-- The constructor --
    cThread(int nPriority = THREAD_PRIORITY_NORMAL);
    //-- Wait until the thread terminate, after this function you are sure that the thread is terminated.
    bool WaitUntilTerminate(DWORD dwMiliSec = INFINITE);
    //-- Start the thread or recreate it, if it has been terminated before --
    bool Start();
    //-- Start the thread and return when it actualy start --
    bool StartAndWait();
    //-- Pause the thread --
    bool Pause();
    //-- Check if the thread is running or not.
    bool IsRunning();
    //-- check if the thread has been terminated or not.
    bool IsTerminated();
    //-- Check for the thread is suspend or not.
    bool IsSuspend();
    //-- Set thread priority
    void SetPriority(int nLevel);
    //-- Get thread priority
    int GetPriority();
    //-- Speed up thread execution - increase priority level
    void SpeedUp();
    //-- Slow down Thread execution - decrease priority level
    void SlowDown();
    //-- Terminate immediate the thread Unsafe.
    void Terminate();

  protected:
    //-- put the initialization code here.
    virtual void OnInitInstance(){}
    //-- put the main code of the thread here.
    virtual void OnRunning() = 0; //-- Must be overloaded --
    //-- put the cleanup code here.
    virtual DWORD OnExitInstance(){return 0;}
    //-- Exit the thread safety.
    void Exit();
    //-- Thread function --
    static unsigned __stdcall _ThreadProc(LPVOID lpParameter);
    //-- Destructor --
    virtual ~cThread()
    {
      ::CloseHandle(m_hEvent);
    }

  protected:
    HANDLE m_hThread, m_hEvent; //-- Thread and Event handle -- /*!< TODO */
    int m_nInitPriority;
    unsigned int m_dwThreadID; //-- Contain the thread ID -- /*!< TODO */
    //-- Variable to know the state of the thread terminated or suspend or Running -- 
    bool m_bTerminate, m_bSuspend, m_bIsRunning;
  };

  /////////////////////////////////////////////////////////////////////////////

  //-- End of CThread Class Definition --

  //-- Begin of TThread declaration --
  template<typename T>
  /*!
   \brief Template Thread Class

   \class TThread cThread.h "cThread.h"
  */
  class TThread
  {
  public:

    /*!
     \brief The constructor

     \fn TThread
     \param thObject
     \param (T::pfnOnRunning)()
     \param nPriority
    */
    TThread(T& thObject, void (T::*pfnOnRunning)(), int nPriority = THREAD_PRIORITY_NORMAL);
    /*!
     \brief Wait until the thread terminate, after this function you are sure that the thread is terminated

     \fn WaitUntilTerminate
     \param dwMiliSec
    */
    bool WaitUntilTerminate(DWORD dwMiliSec = INFINITE);
    /*!
    \brief Start the thread or recreate it, if it has been terminated before

    \fn Start
    */
    bool Start();
    /*!
     \brief Start the thread and return when it actualy start

    \fn StartAndWait
    */
    bool StartAndWait();
    /*!
    \brief     Pause the thread

    \fn Pause
    */
    bool Pause();
    /*!
    \brief Check if the thread is running or not
    \fn IsRunning
    */
    bool IsRunning();
    /*!
    \brief check if the thread has been terminated or not

    \fn IsTerminated
    */
    bool IsTerminated();
    /*!
     \brief Check for the thread is suspend or not
     \fn IsSuspend
    */
    bool IsSuspend();
    /*!
     \brief Set thread priority

     \fn SetPriority
     \param nLevel
    */
    void SetPriority(int nLevel);
    /*!
     \brief Get thread priority

     \fn GetPriority
    */
    int GetPriority();
    /*!
     \brief Speed up thread execution - increase priority level

     \fn SpeedUp
    */
    void SpeedUp();
    /*!
     \brief Slow down Thread execution - decrease priority level

     \fn SlowDown
    */
    void SlowDown();
    /*!
     \brief Terminate immediate the thread Unsafe

     \fn Terminate
    */
    void Terminate();
    /*!
     \brief Destructor

     \fn ~TThread
    */
    virtual ~TThread()
    {
      ::CloseHandle(m_hEvent);
    }
  protected:
    /*!
     \brief Thread function

     \fn _ThreadProc
     \param lpParameter
    */
    static unsigned __stdcall _ThreadProc(LPVOID lpParameter);
    /*!
     \brief Exit the thread safety

     \fn Exit
    */
    void Exit();
    /*!
     \brief Call the running member function

     \fn OnRunning
    */
    inline void OnRunning();

  protected:
    T& m_thObject;
    void (T::*m_pfnOnRunning)();
    HANDLE m_hThread, m_hEvent; //-- Thread and Event handle --
    int m_nInitPriority;
    unsigned int m_dwThreadID; //-- Contain the thread ID --
    //-- Variable to know the state of the thread terminated or suspend or Running -- 
    bool m_bTerminate, m_bSuspend, m_bIsRunning;
  };

  /////////////////////////////////////////////////////////////////////////////

  //-- End of TThread Class Declaration --

  /*!
   \brief Begin TThread Implementation

   \fn TThread<T>::TThread
   \param thObject
   \param (T::pfnOnRunning)()
   \param nPriority
  */
  template<typename T> TThread<T>::TThread(T& thObject,void (T::*pfnOnRunning)(), int nPriority):m_thObject(thObject),m_pfnOnRunning(pfnOnRunning)
  {
    m_hThread = NULL;
    m_dwThreadID = 0;
    m_bTerminate = true;
    m_bSuspend = true;
    m_bIsRunning = false;
    m_nInitPriority = nPriority;
    m_hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);      
  }
  
  /*!
   \brief Start

   \fn TThread<T>::Start
   \return bool TThread<T>
  */
  template<typename T> bool TThread<T>::Start()
  {
    if(m_bTerminate)
    {
      m_hThread = (HANDLE)_beginthreadex(NULL,0,_ThreadProc,(LPVOID)this,0/* CREATE_SUSPENDED*/,&m_dwThreadID); 

      m_bTerminate = false;
      m_bSuspend = false;

      if(m_hThread != 0)
        return true;
      return false;
    }
    else if(m_bSuspend)
    {
      DWORD nRet = ::ResumeThread(m_hThread);
      if(nRet == 0xFFFFFFFF)
        return false;
      m_bSuspend = false;
      return true;
    }
    return true;
  }

  /*!
   \brief StartAndWait

   \fn TThread<T>::StartAndWait
   \return bool TThread<T>
  */
  template<typename T> bool TThread<T>::StartAndWait()
  {
    bool bRet = Start();
    if(bRet)
      ::WaitForSingleObject(m_hEvent,INFINITE);
    return bRet;
  }

  /*!
   \brief Pause

   \fn TThread<T>::Pause
   \return bool TThread<T>
  */
  template<typename T> bool TThread<T>::Pause()
  {
    if(m_bTerminate)
      return false;
    if(m_bSuspend)
      return true;
    DWORD nRet = ::SuspendThread(m_hThread);
    if(nRet == 0xFFFFFFFF)
      return false;
    m_bSuspend = true;
    return true;
  }

  /*!
   \brief IsRunning

   \fn TThread<T>::IsRunning
   \return bool TThread<T>
  */
  template<typename T> bool TThread<T>::IsRunning()
  {
    return m_bIsRunning;
  }

  /*!
   \brief IsTerminated

   \fn TThread<T>::IsTerminated
   \return bool TThread<T>
  */
  template<typename T> bool TThread<T>::IsTerminated()
  {
    return m_bTerminate;
  }

  /*!
   \brief IsSuspend

   \fn TThread<T>::IsSuspend
   \return bool TThread<T>
  */
  template<typename T> bool TThread<T>::IsSuspend()
  {
    return m_bSuspend;
  }

  /*!
   \brief Terminate

   \fn TThread<T>::Terminate
  */
  template<typename T> void TThread<T>::Terminate()
  {
    DWORD dwExitCode;
    ::GetExitCodeThread(m_hThread,&dwExitCode);
    if (STILL_ACTIVE == dwExitCode)
    {
      ::TerminateThread(m_hThread,dwExitCode);
      ::CloseHandle(m_hThread);
      m_hThread = NULL;
    }
    m_bIsRunning = false;
    m_bTerminate = true;
  }

  /*!
   \brief Exit

   \fn TThread<T>::Exit
  */
  template<typename T> void TThread<T>::Exit()
  {
    DWORD dwExitCode;
    ::GetExitCodeThread(m_hThread,&dwExitCode);
    if (STILL_ACTIVE == dwExitCode)
    {
      _endthreadex(dwExitCode);
      ::ExitThread(dwExitCode);
      ::CloseHandle(m_hThread);
      m_hThread = NULL;
    }
    m_bIsRunning = false;
    m_bTerminate = true;
  }

  /*!
   \brief WaitUntilTerminate

   \fn TThread<T>::WaitUntilTerminate
   \param dwMiliSec
   \return bool TThread<T>
  */
  template<typename T> bool TThread<T>::WaitUntilTerminate(DWORD dwMiliSec)
  {
    if(WaitForSingleObject(m_hThread,dwMiliSec) == WAIT_TIMEOUT)
      return false;
    m_bIsRunning = false;
    m_bTerminate = true;
    return true;
  }

  /*!
   \brief SetPriority

   \fn TThread<T>::SetPriority
   \param nLevel
  */
  template<typename T> void TThread<T>::SetPriority(int nLevel)
  {
    ::SetThreadPriority(m_hThread,nLevel); 
  }

  /*!
   \brief GetPriority

   \fn TThread<T>::GetPriority
   \return int TThread<T>
  */
  template<typename T> int TThread<T>::GetPriority()
  {
    return ::GetThreadPriority(m_hThread);
  }

  /*!
   \brief SpeedUp

   \fn TThread<T>::SpeedUp
  */
  template<typename T> void TThread<T>::SpeedUp()
  {
    int nOldLevel;
    int nNewLevel;

    nOldLevel = GetPriority();
    if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
      return;
    else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
      nNewLevel = THREAD_PRIORITY_TIME_CRITICAL;
    else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
      nNewLevel = THREAD_PRIORITY_HIGHEST;
    else if (THREAD_PRIORITY_NORMAL == nOldLevel)
      nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
      nNewLevel = THREAD_PRIORITY_NORMAL;
    else if (THREAD_PRIORITY_LOWEST == nOldLevel)
      nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
    else if (THREAD_PRIORITY_IDLE == nOldLevel)
      nNewLevel = THREAD_PRIORITY_LOWEST;

    SetPriority(nNewLevel);
  } 

  /*!
   \brief SlowDown

   \fn TThread<T>::SlowDown
  */
  template<typename T> void TThread<T>::SlowDown()
  {
    int nOldLevel;
    int nNewLevel;

    nOldLevel = GetPriority();
    if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
      nNewLevel = THREAD_PRIORITY_HIGHEST;
    else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
      nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
      nNewLevel = THREAD_PRIORITY_NORMAL;
    else if (THREAD_PRIORITY_NORMAL == nOldLevel)
      nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
    else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
      nNewLevel = THREAD_PRIORITY_LOWEST;
    else if (THREAD_PRIORITY_LOWEST == nOldLevel)
      nNewLevel = THREAD_PRIORITY_IDLE;
    else if (THREAD_PRIORITY_IDLE == nOldLevel)
      return;

    SetPriority(nNewLevel);
  }

  /*!
   \brief OnRunning

   \fn TThread<T>::OnRunning
  */
  template<typename T> void TThread<T>::OnRunning()
  {
    (m_thObject.*m_pfnOnRunning)();
  }

  /*!
   \brief

   \fn TThread<T>::_ThreadProc
   \param lpParameter
   \return unsigned TThread<T>
  */
  template<typename T> unsigned __stdcall TThread<T>::_ThreadProc(LPVOID lpParameter)
  {
    TThread<T>* pThread = reinterpret_cast<TThread<T>*>(lpParameter);
 
    pThread->SetPriority(pThread->m_nInitPriority);
    pThread->m_bIsRunning = true;
    ::SetEvent(pThread->m_hEvent);
    pThread->OnRunning();
    return 0;
  }

//-- End of Thread.hpp --
