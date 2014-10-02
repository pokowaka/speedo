#pragma once
#include "io/ProtocolHandler.h"

class NullProgrocol : ProtocolHandler {
  public:
     bool handle(int fd, uint8_t* buffer, ssize_t size) {
        return true;
    }

    bool handleErr(int fd, int errno) {
      return false;
    }
}

