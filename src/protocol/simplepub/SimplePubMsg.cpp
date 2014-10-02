#include "SimplePubMsg.h"
#include "log/easylogging++.h"

void SimplePubMsg::execute() {
   LINFO << "Executing fd: " << m_fd << ", payload: " << m_payload;
}
