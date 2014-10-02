#pragma once
#include "io/ProtocolHandler.h"
#include "log/easylogging++.h"

#include <stdio.h>

class LogProtocol : public ProtocolHandler {
  public:
     bool handle(int fd, uint8_t* buffer, ssize_t size) {
        LINFO  << " fd: " << fd << ", size: " << size << ", payload: " << buffer;
        return true;
    }
    
     bool handleErr(int fd, int err) {
        LINFO  << " fd: " << fd << ", err: " << err << " str: " << strerror(err);
        return false;

     }
};
