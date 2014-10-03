#pragma once
#include <queue>
#include <pthread.h>

template <class T>
class Queue
{
  public:

    Queue() {
      pthread_mutex_init(&m_lock, NULL);
      pthread_cond_init(&m_cond, NULL);
    }

    ~Queue() {
      pthread_mutex_destroy(&m_lock);
      pthread_cond_destroy(&m_cond);
    }

    T pop() {
      pthread_mutex_lock(&m_lock);
      while (m_queue.empty()) {
        pthread_cond_wait(&m_cond, &m_lock);
      }
      T val = m_queue.front();
      m_queue.pop();
      pthread_mutex_unlock(&m_lock);
      return val;
    }

    bool const empty() {
      return m_queue.empty();
    }

    void pop(T& item) {
      pthread_mutex_lock(&m_lock);
      while (m_queue.empty()) {
        pthread_cond_wait(&m_cond, &m_lock);
      }
      item = m_queue.front();
      m_queue.pop();
      pthread_mutex_unlock(&m_lock);
    }

    void push(const T& item) {
      pthread_mutex_lock(&m_lock);
      m_queue.push(item);
      pthread_cond_signal(&m_cond);
      pthread_mutex_unlock(&m_lock);
    }

  private:
    std::queue<T>   m_queue;
    pthread_cond_t  m_cond;
    pthread_mutex_t m_lock;
};
