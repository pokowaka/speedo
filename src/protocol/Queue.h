#pragma once
// Copyright (c) 2013 Juan Palacios juan.palacios.puyana@gmail.com
//
// Subject to the BSD 2-Clause License
// - see < http://opensource.org/licenses/BSD-2-Clause>
//

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <class T>
class Queue
{
 public:

  T pop() 
  {
    std::unique_lock<std::mutex> mlock(m_mutex);
    while (m_queue.empty())
    {
      m_cond.wait(mlock);
    }
    T val = m_queue.front();
    m_queue.pop();
    return val;
  }

  bool const empty() {
    return m_queue.empty();
  }

  void pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(m_mutex);
    while (m_queue.empty())
    {
      m_cond.wait(mlock);
    }
    item = m_queue.front();
    m_queue.pop();
  }

  void push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(m_mutex);
    m_queue.push(item);
    mlock.unlock();
    m_cond.notify_one();
  }
  Queue() { };
  
 private:
  std::queue<T> m_queue;
  std::mutex m_mutex;
  std::condition_variable m_cond;
};
