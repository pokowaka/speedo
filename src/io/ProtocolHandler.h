#pragma once
#include <stdint.h>
#include <sys/types.h>

class ProtocolHandler {
  public:
    // Return false if you don't care about this file descriptor anymore
    // and it is safe to unregister.
    virtual bool handle(int fd, uint8_t* buffer, ssize_t size) = 0;

    // Return false if you don't care about this file descriptor anymore
    // and it is safe to unregister.
    virtual bool handleErr(int fd, int errno) = 0;
};


class ProtocolHandlerFactory {
  public:
    virtual ProtocolHandler* getProtocolHandler(int fd) = 0;
};

template<class T>
class DefaultProtocolHandlerFactory : public ProtocolHandlerFactory {
  public:
    ProtocolHandler* getProtocolHandler(int fd) {
      return new T();
    }
};

template<class T>
class StaticDefaultProtocolHandlerFactory : public ProtocolHandlerFactory {
  public:
    StaticDefaultProtocolHandlerFactory() { 
      m_protocol = new T();
    };
    ~StaticDefaultProtocolHandlerFactory() {
      delete m_protocol;
    }

    ProtocolHandler* getProtocolHandler(int fd) {
      return m_protocol;
    }
private:
    T* m_protocol;
};

