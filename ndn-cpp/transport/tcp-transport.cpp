/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../node.hpp"
#include "../c/util/ndn_realloc.h"
#include "tcp-transport.hpp"

using namespace std;

namespace ndn {

void TcpTransport::connect(Node &node)
{
  ndn_Error error;
  if ((error = ndn_TcpTransport_connect(&transport_, (char *)node.getHost(), node.getPort())))
    throw std::runtime_error(ndn_getErrorString(error)); 

  // TODO: This belongs in the socket listener.
  const unsigned int initialLength = 1000;
  // Automatically cast ndn_ to (struct ndn_ElementListener *)
  ndn_BinaryXmlElementReader_init
    (&elementReader_, &node, (unsigned char *)malloc(initialLength), initialLength, ndn_realloc);
  
  // TODO: Properly indicate connected status.
  node_ = &node;
}

void TcpTransport::send(const unsigned char *data, unsigned int dataLength)
{
  ndn_Error error;
  if ((error = ndn_TcpTransport_send(&transport_, (unsigned char *)data, dataLength)))
    throw std::runtime_error(ndn_getErrorString(error));  
}

void TcpTransport::processEvents()
{
  int receiveIsReady;
  ndn_Error error;
  if ((error = ndn_TcpTransport_receiveIsReady(&transport_, &receiveIsReady)))
    throw std::runtime_error(ndn_getErrorString(error));  
  if (!receiveIsReady)
    return;

  unsigned char buffer[8000];
  unsigned int nBytes;
  if ((error = ndn_TcpTransport_receive(&transport_, buffer, sizeof(buffer), &nBytes)))
    throw std::runtime_error(ndn_getErrorString(error));  

  ndn_BinaryXmlElementReader_onReceivedData(&elementReader_, buffer, nBytes);
}

void TcpTransport::close()
{
  ndn_Error error;
  if ((error = ndn_TcpTransport_close(&transport_)))
    throw std::runtime_error(ndn_getErrorString(error));  
}

TcpTransport::~TcpTransport()
{
  if (elementReader_.partialData.array)
    // Free the memory allocated in connect.
    free(elementReader_.partialData.array);
}

}
