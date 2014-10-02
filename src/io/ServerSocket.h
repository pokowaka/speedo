#pragma once
#include <stdint.h>
#include <netinet/in.h>
#include <string>
#include "ProtocolHandler.h"
#include "Eventloop.h"


// The max size of the connection backlog.
#define TCP_CONNECTION_BACKLOG 0xffff


class ServerSocket : public ProtocolHandler {
  public:
    explicit ServerSocket(ProtocolHandlerFactory* handler);
    ~ServerSocket() {}
    bool listen(std::string address, uint16_t port, bool useIpV6 = false);
    void close();

    // Return false if you don't care about this file descriptor anymore
    // and it is safe to unregister.
    bool handle(int fd, uint8_t* buffer, ssize_t size);

    // Return false if you don't care about this file descriptor anymore
    // and it is safe to unregister.
    bool handleErr(int fd, int errno);



  private:

    int accept();


    int m_fd;
    sockaddr_in m_socketAddress;
    ProtocolHandlerFactory* m_protocolHandler;
    Eventloop m_loop;
};

