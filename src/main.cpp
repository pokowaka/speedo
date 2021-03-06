// © Copyright 2012 TokBox. All Rights Reserved.

// In ev.h, EV_ERROR is defined as an enum value:
// EV_ERROR = 0x80000000 /* sent when an error occurs */
// but in systems which have kqueue, EV_ERROR is defined for
// the use of kqueue itself, hence we need to include threadpool first
#include <stdio.h>
#include <stdlib.h>
// #include <log4cxx/propertyconfigurator.h>
#include <string>
#include "io/ServerSocket.h"
#include "protocol/LogProtocol.h"
#include "log/easylogging++.h"
#include "protocol/echo/EchoProtocol.h"
#include "protocol/Queue.h"
#include "protocol/QueueWorker.h"

ServerSocket* server;

_INITIALIZE_EASYLOGGINGPP


void signal_handler(int sig) {
  // TODO(ErwinJ): Do some proper clean up here..
  LWARNING << "Closing server.."; 
  if (server)
    server->close();
}


int main(int argc, char* argv[]) {
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  Queue<Message*> queue;
  QueueWorker worker(&queue);
  worker.init(1);

  EchoProtocolFactory epf(&queue);
  server = new ServerSocket(&epf);

  // This blocks until we call close on the server.
  server->listen("0.0.0.0", 9292);
  worker.stop();

  LINFO << "finished!";
  delete server;
  return 0;
}
