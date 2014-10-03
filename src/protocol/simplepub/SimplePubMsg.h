#pragma once
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "protocol/Message.h"

class SimplePubMsg : public Message {
  public:
  enum Type {
    PUB = 1,
    SUB = 2,
    UNSUB = 3,
    DISCONNECT = 4
  };

  SimplePubMsg(int fd) : m_fd(fd), m_header(0), m_payload(NULL) { };

  SimplePubMsg(int fd, Type type, uint8_t* payload, size_t cPayload) : m_fd(fd) {
    setType(type);
    setPayload(payload, cPayload);
  };

  void execute();

  ~SimplePubMsg() {
    delete m_payload;
  }

  inline void setType(Type tp) {
    m_header = m_header | (tp << 24);
  }

  inline Type getType() {
      return static_cast<Type>((m_header & 0xff000000) >> 6);
  }

  inline uint8_t* getPayload() {
    return m_payload;
  }

  uint32_t getPayloadSize() {
    return ntohl(m_header & 0x00ffffff);
  }

  inline void setHeader(uint32_t headers) { 
    m_header = headers;
  };


  inline uint32_t getHeader() {
    return m_header;
  }

  inline void setPayload(uint8_t* payload, size_t cPayload) {
      assert (cPayload < 0x00ffffff);
      delete m_payload;
      m_payload = payload;
      m_header = m_header | htonl(cPayload);
  }

  private:
    int       m_fd;
    uint32_t  m_header;

    // TODO(ErwinJ): [0] based in the future?
    uint8_t*  m_payload;
};
