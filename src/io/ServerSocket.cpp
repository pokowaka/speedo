#include "ServerSocket.h"
#include "log/easylogging++.h"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

ServerSocket::ServerSocket(ProtocolHandlerFactory* handler) :
  m_fd(0), m_protocolHandler(handler) {
    memset(&m_socketAddress, 0, sizeof(sockaddr_in));
}


void ServerSocket::close() {
  ::close(m_fd);
  m_loop.unloop();
  m_fd = -1;
}

int ServerSocket::accept() {
  sockaddr newSocketAddr = {0};
  socklen_t newSocketAddrLen = {0};
  int newSocketId = ::accept(m_fd, &newSocketAddr, &newSocketAddrLen);
  if (0 > newSocketId) {
    if (errno != EWOULDBLOCK)
      LERROR << "accept: Failed to accept new connection: fd:" << m_fd << ", errno:" << errno << ", error:" << strerror(errno);
    return -1;
  }



#if defined(SO_NOSIGPIPE)
  // ONLY ON FREEBSD
  int set = 1;
  if (setsockopt(newSocketId, SOL_SOCKET, SO_NOSIGPIPE, reinterpret_cast<char*>(&set), sizeof(set)) < 0) {
    LERROR << "accept:  fd:" << m_fd << ", errno:" << errno << ", error:" << strerror(errno);
  }
#endif

  fcntl(newSocketId, F_SETFL, fcntl(newSocketId, F_GETFL, 0) | O_NONBLOCK);
  return newSocketId;
}

bool ServerSocket::handle(int fd, uint8_t* buffer, ssize_t size) {
  return true;
}

bool ServerSocket::handleErr(int fd, int err) {
  if (err != 0 && err != ENOTCONN)  {
    return false;
  }
  int newFd = 0;
  while( (newFd = accept()) > 0) {
    m_loop.observe(newFd, m_protocolHandler->getProtocolHandler(newFd));
  }

  return true;
}

bool ServerSocket::listen(std::string address, uint16_t port, bool useIpV6) {
  bool res = false;
  char service[10];
  int error = 0;
  int optval = 1;
  addrinfo hints = {0};
  addrinfo* info = NULL;

  // Let's translate this address & port into an actual ip address.
  if (port < 1 || port > 65535) {
    LERROR << "Port out of range, the port needs to be in [1, 65535] :" << port;
    goto Error;
  }
  snprintf(service, sizeof(service), "%d", port);
  hints.ai_socktype =  SOCK_STREAM;
  hints.ai_family = useIpV6 ? AF_INET6 : AF_INET;

  error = getaddrinfo(address.c_str(), service, &hints, &info);
  if (error != 0) {
    LERROR << "listen: Failed to bind server socket: fd:" << m_fd << ", errno:" << error << ", error:" << gai_strerror(error);
    goto Error;
  }

  m_fd = socket((useIpV6) ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_fd < 0) {
     LERROR << "Failed to create socket";
    goto Error;
  }

  // set SO_REUSEADDR on a socket to true (1):
  setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  memcpy(&m_socketAddress, info->ai_addr, sizeof(m_socketAddress));

  // Non blocking please!
  fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK);

  if (::bind(m_fd, (struct sockaddr *) &m_socketAddress, sizeof(m_socketAddress)) < 0) {
    // int error = errno;
    LERROR << "listen: Failed to bind server socket: fd:" << m_fd << ", errno:" << error << ", error:" << strerror(error);
    goto Error;
  }

  if (::listen(m_fd, TCP_CONNECTION_BACKLOG) < 0) {
    // int error = errno;
    LERROR << "listen: Failed to listen on server socket: fd:" << m_fd << ", errno:" << error << ", error:" << strerror(error);
    goto Error;
  }

  // We made it to here, so we succeed!
  res = true;

  m_loop.observe(m_fd, this);
  m_loop.loop();

 Error:

  freeaddrinfo(info);
  return res;
}
