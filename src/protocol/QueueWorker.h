#pragma once

#include "Queue.h"
#include "Message.h"
#include <pthread.h>

class QueueWorker {

  public:
    bool init(int cNumThreads);
    void stop();

  private:
    static void* worker(void *thisWorker);

    Queue<Message*>   *m_messageQueue;
    pthread_t         *m_threads;
    int                m_cThreads;
    bool               m_stop;
};
