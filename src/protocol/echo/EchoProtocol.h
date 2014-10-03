#pragma once
#include "protocol/Message.h"
#include "protocol/Queue.h"
#include <unistd.h>

class EchoMessage : public Message {
  public:
    EchoMessage(int fd) : m_fd(fd), m_message("") { }
    ~EchoMessage() { }

    void execute() {
      write(m_fd, m_message.data(), m_message.size());
    }

    void append(char c) {
      m_message.append(&c, 1);
    }
  private:
    std::string m_message;
    int m_fd;
};

class EchoProtocol : public ProtocolHandler {
  public:
    EchoProtocol(Queue<Message*> *msgQueue) : m_msg(NULL), m_queue(msgQueue) { }
    ~EchoProtocol() { delete m_msg; }

    bool handle(int fd, uint8_t* buffer, ssize_t size) {
      while(size > 0) {
        if (!m_msg) {
          m_msg = new EchoMessage(fd);
        }

        m_msg->append(*buffer);
        if (*buffer == '\n') {
          m_queue->push(m_msg);
          m_msg  = NULL;
        }
        size--;
        buffer++;
      }
      return true;
    }

    bool handleErr(int fd, int err) {
      delete this;
      return false;
    }

  private:
    EchoMessage      *m_msg;
    Queue<Message*>  *m_queue;
};

class EchoProtocolFactory : public ProtocolHandlerFactory {
  public:
    EchoProtocolFactory(Queue<Message*> *queue) : m_queue(queue) { }
    ProtocolHandler* getProtocolHandler(int fd) {
      return new  EchoProtocol(m_queue);
    }
  private:
    Queue<Message*> *m_queue;
};
