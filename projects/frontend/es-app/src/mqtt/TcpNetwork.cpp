//
// Created by Bkg2k on 11/03/2020.
//

#include "TcpNetwork.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cerrno>

int TCPNetwork::connect(const char* hostname, int port)
{
  if (port == 0) port = 1883;

  struct sockaddr_in address {};
  int rc;
  struct addrinfo *result = nullptr;
  struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, nullptr, nullptr, nullptr};

  if ((rc = getaddrinfo(hostname, nullptr, &hints, &result)) == 0)
  {
    for(struct addrinfo* res = result; res != nullptr; res = res->ai_next)
      if (res->ai_family == AF_INET)
      {
        result = res;
        break;
      }

    if (result->ai_family == AF_INET)
    {
      address.sin_port = htons(port);
      address.sin_family = AF_INET;
      address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
    }
    else rc = -1;

    freeaddrinfo(result);
  }

  if (rc == 0)
  {
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket != -1)
      rc = ::connect(mSocket, (struct sockaddr*)&address, sizeof(address));
  }

  return rc;
}

int TCPNetwork::read(unsigned char* buffer, int len, int timeout_ms)
{
  if (mSocket == 0) return -1;

  struct timeval interval =
  {
    timeout_ms / 1000,
    (timeout_ms % 1000) * 1000
  };
  if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 0))
  {
    interval.tv_sec = 0;
    interval.tv_usec = 100;
  }
  setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*) &interval, sizeof(struct timeval));

  int bytes = 0;
  int max_tries = 10;
  while (bytes < len && --max_tries >= 0)
  {
    int rc = ::recv(mSocket, &buffer[bytes], (size_t) (len - bytes), 0);
    if (rc == -1)
    {
      if (errno != EAGAIN && errno != EWOULDBLOCK) return -1;
    }
    else bytes += rc;
    if (rc == 0) break;
  }
  return bytes;
}

int TCPNetwork::write(unsigned char* buffer, int len, int timeout)
{
  if (mSocket == 0) return -1;
  struct timeval tv
    {
    0,
    timeout * 1000
  };
  setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (char*) &tv, sizeof(struct timeval));
  return send(mSocket, buffer, len, MSG_NOSIGNAL);
}

int TCPNetwork::disconnect()
{
  int socket = mSocket;
  mSocket = 0;
  return ::close(socket);
}
