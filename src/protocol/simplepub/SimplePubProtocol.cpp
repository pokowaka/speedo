#include "SimplePubProtocol.h"
#include "SimplePubMsg.h"

bool SimplePubProtocol::handle(int fd, uint8_t* buffer, ssize_t size) {
  do {
    // Looks like we are building a new message.
    if (m_currentMsg == NULL) {
      m_currentMsg = new SimplePubMsg(fd);
      m_read = 0;
      m_cBuffer = 0;
      m_buffer = NULL;
    }

    // First construct the header, we might not have the complete header available.
    while(m_read < 4 && size > 0) {
      m_currentMsg->setHeader( m_currentMsg->getHeader() << 8 | *buffer);
      buffer++;
      m_read++;
      size--;
    }

    // We haven't completely read the header yet.
    if (size == 0) {
      return true;
    }

    // We now have a valid header, allocate payload if needed.
    if (m_currentMsg->getPayload() == NULL) {
      m_cBuffer = m_currentMsg->getPayloadSize();
      m_buffer = new uint8_t[m_cBuffer];
      m_currentMsg->setPayload(m_buffer, m_currentMsg->getPayloadSize());
    }

    // Fill up the payload.
    while(size > 0 && m_cBuffer > 0) {
      *m_buffer = *buffer;
      size --;
      m_buffer++;
      buffer++;
      m_read++;
      m_cBuffer--;
    }

    if (m_cBuffer == 0) {
      // yay! message is ready.. the message queue will take it from here.
      m_messageQueue.push(m_currentMsg);
      m_currentMsg = NULL;
    }

  } while(size > 0);

  return true;
}

bool SimplePubProtocol::handleErr(int fd, int err) {
  // Booh! Bad client.. We will just kick you out.
  m_messageQueue.push(new SimplePubMsg(fd, SimplePubMsg::DISCONNECT, NULL, 0));

  // No-one will ever call us again. Since this is single threaded we don't
  // have to worry about anyone poking around the handle method above.
  delete this;
  return false;
}

