#pragma once
#include "io/ProtocolHandler.h"
#include "SimplePubMsg.h"
#include "protocol/Queue.h"
#include "log/easylogging++.h"

#include <stdio.h>

class SimplePubProtocol : public ProtocolHandler {
  public:
     bool handle(int fd, uint8_t* buffer, ssize_t size);
     bool handleErr(int fd, int err);

   private:
     Queue<SimplePubMsg*> m_messageQueue;
     SimplePubMsg*        m_currentMsg;
     uint32_t             m_read;
     uint32_t             m_cBuffer;
     uint8_t*             m_buffer;
};

