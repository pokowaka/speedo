#include "Eventloop.h"
#include "../log/easylogging++.h"
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>

Eventloop::Eventloop() : m_loop(EV_DEFAULT), m_teardown(false) {

}

void Eventloop::unloop() {
  m_teardown = true;
  ev_break(m_loop, EVBREAK_ALL);
}

void Eventloop::loop() {
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

