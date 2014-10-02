#pragma once
#include <stdint.h>
#include <ev.h>
#include "ProtocolHandler.h"


#define RECEIVE_BUFFER_SIZE 8 * 1024


class Eventloop {

  public:
    Eventloop();
    void loop();
    void unloop();

    // Never ever call this from the non I/O thread, (i.e. the thread that is running the loop)
    // it will likely not behave the way you expect.
    //
    // You can call this:
    //
    // 1. Before you have called the method loop.
    // 2. From the thread that is in the loop. (i.e. in your protocol handler)
    //
    // this is due to the way libev handles ev_start, ev_stop. ev_start and stop need to be executed
    // on the same thread, writing the proper code to handle this with ev_async events is non trivial and
    // is *VERY* easy to do wrong. 
    void observe(int fd, ProtocolHandler* handler);
  private:
    static void readyReadCallback(EV_P_ ev_io *w, int revents);
    bool m_teardown;

    struct ev_loop* m_loop;
};

