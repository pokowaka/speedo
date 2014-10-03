#include "Eventloop.h"
#include "../log/easylogging++.h"
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>

Eventloop::Eventloop() : m_loop(EV_DEFAULT), m_teardown(false) {
  // Setup signal handler so we can safely and immediately exit the loop
  // if needed. 
  m_async.data = this;
  ev_async_init (&m_async, Eventloop::readyAsyncCallback);
  ev_async_start(m_loop, &m_async);
}

void Eventloop::unloop() {
  m_teardown = true;
  ev_async_send(m_loop, &m_async);
}

void Eventloop::readyAsyncCallback(EV_P_ ev_async *w, int revents) {
  // Called when it is time to exit the loop.
  Eventloop *pThis = (Eventloop*) w->data;
  ev_break(pThis->m_loop, EVBREAK_ALL);
}

void Eventloop::loop() {

  int backend = ev_backend(m_loop);
  std::string str;
  switch (backend) {
    case EVBACKEND_SELECT :  str = "EVBACKEND_SELECT"; break;
    case EVBACKEND_POLL   :  str = "EVBACKEND_POLL  "; break;
    case EVBACKEND_EPOLL  :  str = "EVBACKEND_EPOLL "; break;
    case EVBACKEND_KQUEUE :  str = "EVBACKEND_KQUEUE"; break;
    case EVBACKEND_DEVPOLL:  str = "EVBACKEND_DEVPOL"; break;
    case EVBACKEND_PORT   :  str = "EVBACKEND_PORT  "; break;
    case EVBACKEND_ALL    :  str = "EVBACKEND_ALL   "; break;
    case EVBACKEND_MASK   :  str = "EVBACKEND_MASK  "; break;
    default: str = "Unknown backend"; break;
  };

  LINFO << "loop using backend: " << str;

  while(!m_teardown) {
    LTRACE << "ev_run";
    ev_run(m_loop, 0);
  }
}

void Eventloop::observe(int fd, ProtocolHandler* handler) {
  ev_io* w =  new ev_io();
  w->data = reinterpret_cast<void*>(handler);
  ev_io_init(w, Eventloop::readyReadCallback, fd, EV_READ);
  ev_io_start(m_loop, w);

  LINFO << "observe: " << fd;
}

void Eventloop::readyReadCallback(EV_P_ ev_io *w, int revents) {
  LTRACE << "readyReadCallback";
  uint8_t buffer[RECEIVE_BUFFER_SIZE];
  size_t buflen = RECEIVE_BUFFER_SIZE;
  ssize_t bytes = 0;
  int fd = w->fd;
  bool open = true;
  ProtocolHandler* protocolHandler = (ProtocolHandler*) w->data;

  //
  // Clean out the socket buffers 
  while( open &&  (bytes = recv(fd, buffer, buflen, 0)) > 0) {
    LTRACE << "bytes " << bytes;
    open = protocolHandler->handle(fd, buffer, bytes);
  }

  if (errno != EWOULDBLOCK) {
    open = protocolHandler->handleErr(fd, errno);
  }
  if (bytes == 0) {
    open = protocolHandler->handleErr(fd, ENOTCONN);
  }


  if (!open) {
    ev_io_stop(EV_A_ w);
    delete w;
  }
};

