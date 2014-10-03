#include "QueueWorker.h"
#include "log/easylogging++.h"

QueueWorker::QueueWorker(Queue<Message*> *msg)
  : m_messageQueue(msg), m_threads(NULL), m_cThreads(0), m_stop(false) {
  };

QueueWorker::~QueueWorker() {
  stop();
}

void* QueueWorker::worker(void *thisWorker) {
  QueueWorker* pThis = (QueueWorker*) thisWorker;
   while(!pThis->m_stop) {
      Message *msg = pThis->m_messageQueue->pop();
      msg->execute();
      delete msg;
   }
   return NULL;
}

void QueueWorker::stop() {
  if (m_stop)
    return;
  m_stop = true;

  // Ok, so it is possible that the queue is empty and a single thread executes ALL
  // the dummy messages.. We will assume that the OS is not that unfair in scheduling threads.
  // besides, this would only be a problem upon termination, so not that critical.
  for(int i = 0; i < m_cThreads * 10; i++)
    m_messageQueue->push(new DummyMessage());

  for (int i = 0; i < m_cThreads; i++) {
    pthread_join(m_threads[i], NULL);
  }

  // Now let's clean out the queue
  //
  while(!m_messageQueue->empty()) {
    delete m_messageQueue->pop();
  }

  // Clear out the threads
  delete m_threads;
  m_threads = NULL;
  m_cThreads = 0;
}


bool QueueWorker::init(int cThreads) {
  m_stop = false;
  m_threads =  new pthread_t[cThreads];
  m_cThreads = cThreads;
  for (int i = 0; i < cThreads; i++) {
    // Default thread attributes are fine
   int err = pthread_create(&m_threads[i], NULL, QueueWorker::worker, this);
    if (0 != err) {
      LERROR << "Faied to initialize queue worker " << err;
      return false;
    }
  }

  return true;
}
