
#ifndef __MYOS__MULTITHREAD_H
#define __MYOS__MULTITHREAD_H

#include <common/types.h>
#include <gdt.h>
#include <multitasking.h> // for CPUState

namespace myos
{

  class Thread
  {
    friend class ThreadManager;

  private:
    common::uint8_t stack[4096]; // 4 KiB
    CPUState *cpustate;
    int id;

  public:
    Thread(GlobalDescriptorTable *gdt, void entrypoint(), int id);
    bool getYield();
    void setYield(bool);
    bool getJoin();
    int getId();
    void setJoin(bool);
    int getYieldCounter();
    void setYieldCounter(int);
    void incrementYieldCounter();

    ~Thread();
  };

  class ThreadManager
  {
  private:
    Thread *threads[256];
    int numThreads;
    int currentThread;

  public:
    ThreadManager();
    ~ThreadManager();
    bool CreateThread(Thread *thread);
    bool Yield(int id);
    bool Join(int id);
    CPUState *Schedule(CPUState *cpustate);
    bool yieldModeOpen = false;
    int yieldedThread = -1;
    int tempYieldId = -1;
    int yieldCounter = 0;
    bool joinModeOpen = false;
    int joinedThread = -1;
    int notJoinedThread = -1;
  };

}

#endif