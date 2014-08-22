 /* --COPYRIGHT--, 
Permissive via Author: Emad Barsoum
15. September 2011
 * --/COPYRIGHT--*/
#include "windows.h"
#include <process.h>
#include "cThread.h"


cThread::cThread(int nPriority)
{
  m_hThread = NULL;
  m_dwThreadID = 0;
  m_bTerminate = true;
  m_bSuspend = true;
  m_bIsRunning = false;
  m_nInitPriority = nPriority;
  m_hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);      
}

bool cThread::Start()
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

bool cThread::StartAndWait()
{
  bool bRet = Start();
  if(bRet)
    ::WaitForSingleObject(m_hEvent,INFINITE);

  return bRet;
}

bool cThread::Pause()
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

bool cThread::IsRunning()
{
  return m_bIsRunning;
}

bool cThread::IsTerminated()
{
  return m_bTerminate;
}

bool cThread::IsSuspend()
{
  return m_bSuspend;
}

void cThread::Terminate()
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

void cThread::Exit()
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

bool cThread::WaitUntilTerminate(DWORD dwMiliSec)
{
  if(WaitForSingleObject(m_hThread,dwMiliSec) == WAIT_TIMEOUT)
    return false;
  m_bIsRunning = false;
  m_bTerminate = true;
  return true;
}

void cThread::SetPriority(int nLevel)
{
  ::SetThreadPriority(m_hThread,nLevel); 
}

int cThread::GetPriority()
{
  return ::GetThreadPriority(m_hThread);
}

void cThread::SpeedUp()
{
  int nOldLevel=0;
  int nNewLevel=0;

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

void cThread::SlowDown()
{
  int nOldLevel=0;
  int nNewLevel=0;

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

unsigned __stdcall cThread::_ThreadProc(LPVOID lpParameter)
{
  cThread* pThread = reinterpret_cast<cThread*>(lpParameter);
   
  pThread->SetPriority(pThread->m_nInitPriority);
  pThread->m_bIsRunning = true;
  ::SetEvent(pThread->m_hEvent);
  pThread->OnInitInstance();
  pThread->OnRunning();
  return pThread->OnExitInstance();
}
