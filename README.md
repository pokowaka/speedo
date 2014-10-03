Speedo
======

Speedo is a very simple example of an eventloop driven server. The architecture is pretty straightforward:


We have an I/O thread that does:

 IO Layer (produces bytes) --> Protocol Layer (produces "messages") --> Message Queue
 
We have a set of worker threads that pop messages of the queue and executes them.


Installing
==========

You will need to install the following dependencies:

* libev
* cmake

On linux:

`sudo apt-get install libev-dev cmake`

On mac, use homebrew:

`brew install cmake libev`

Next you can do something like this:

```
mkdir build
cd build
cmake ..
./src/speedo
```

Next you can ```telnet localhost 9292``` and just type some things. You should see it 
echo back what you typed after pressing enter.



Adding my own protocol
----------------------

The easiest example is the EchoProtocol. The echo protocol creates messages seperated by newlines, that are placed in the Queue. The message is echo'd back to the socket that send it.

A more complex example is a (not yet finisheed) simple pub sub protocol (like [mqtt](http://mqtt.org/))

You basically need to implement the following things:

* A thing that implements `ProtocolHandler`
* A thing that implements `ProtocolHandlerFactory`
* A thing that implements `Message` produced by your protocol handler above, that are placed in a Queue.


FAQ
---

* Why can't I open a huge number of connections on my Mac?

LibEv defaults to select on your mac. Select is restricted on a Mac, it is also to type of backed that gives poor performance. See [link](http://daniel.haxx.se/docs/poll-vs-select.html) for more info on this.


* Why is there only one I/O thread?

Well, it is ***very hard*** to write a multi threaded I/O loop that observes a set of FD using libev. You will be spending a lot of time figuring out how to properly signal between the various threads of when to start and stop observing a set of FD's using libev. You will do it wrong! There's a reason why on the libev page it says to not do it!

Worse, is that you will likely discover that the performance gains are not there. Especially if all your packets are coming in through one ethernet card. You can play around with scaling up the number of threads in your worker queue. Depending on your type of application it might make things faster.





