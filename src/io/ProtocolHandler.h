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
