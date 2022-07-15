/*
 *******************************************************
 * VibroControl µC Framework
 *
 * Version 3.0, 23.01.2019
 * Author: Felix Laufer
 * Contact: laufer@cs.uni-kl.de
 *
 * AG wearHEALTH
 * Department of Computer Science
 * TU Kaiserslautern (TUK), Germany
 *******************************************************
 */

#ifndef _TCP_LISTENER_H_
#define _TCP_LISTENER_H_

#include "Producer.h"

#include <lwip/sockets.h>
#include <vector>

struct TCPMessage
{
  unsigned int size;
  uint8_t* data;
};

class TCPListener : public Producer<TCPMessage>
{
  // Stack size for listener task: 10kB
  const static unsigned int listenerStackSize = 10000;

public:
  TCPListener(unsigned int port, void(*callback)(const TCPMessage&), const unsigned int maxMessages = 255, const unsigned int maxMessageLen = 255);
  bool start();
  void stop();
  bool send(const uint8_t* data, const unsigned int size);
  bool send(const std::vector<uint8_t>& data);

private:
  void listen();
  void setSocketFlags(const unsigned int socket);

  unsigned int maxMessageLen_;
  unsigned int port_;
  int sock_;
  int clientFd_;
  TaskHandle_t listenTaskHandle_;
};

TCPListener::TCPListener(unsigned int port, void(*callback)(const TCPMessage&), const unsigned int maxMessages, const unsigned int maxMessageLen)
  : Producer<TCPMessage>(callback, maxMessages, 1, 0, 10000), maxMessageLen_(maxMessageLen), port_(port), sock_(0), clientFd_(0), listenTaskHandle_(nullptr)
{}

bool TCPListener::start()
{
  stop();

  const int sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    return false;
  }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port_);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (lwip_bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
  {
    lwip_close_r(sock);
    return false;
  }

  if (lwip_listen(sock, 1) != 0)
  {
    lwip_close_r(sock);
    return false;
  }

  sock_ = sock;
  setSocketFlags(sock_);
  xTaskCreatePinnedToCore([](void* thisPtr) { reinterpret_cast<TCPListener*>(thisPtr)->listen(); }, "TCPListener::listen()", listenerStackSize, this, 1, &listenTaskHandle_, 0);
  
  return true;
}

void TCPListener::stop()
{
  if (clientFd_ > 0)
  {
    lwip_close_r(clientFd_);
    clientFd_ = 0;
  }

  if (sock_ > 0)
  {
    lwip_close_r(sock_);
    sock_ = 0;
  }

  if (listenTaskHandle_ != nullptr)
  {
    vTaskDelete(listenTaskHandle_);
  }
}

bool TCPListener::send(const uint8_t* data, const unsigned int size)
{
  if (clientFd_ > 0 && lwip_send(clientFd_, data, size, 0) > 0)
  {
    return true;
  }

  return false;
}

bool TCPListener::send(const std::vector<uint8_t>& data)
{
  send(data.data(), data.size());
}

void TCPListener::listen()
{
  if (clientFd_ > 0)
  {
    lwip_close_r(clientFd_);
    clientFd_ = 0;
  }

  uint8_t buf[maxMessageLen_];
  while (true)
  {
    struct sockaddr_in siOther;
    unsigned int sLen = sizeof(siOther);
    clientFd_ = lwip_accept(sock_, (struct sockaddr*)&siOther, &sLen);

    if (clientFd_ < 0)
    {
      break;
    }

    setSocketFlags(clientFd_);

    uint8_t b;
    while (lwip_recv(clientFd_, &b, 1, MSG_PEEK) > 0)
    {
      uint8_t messageLen = 0;
      if (lwip_recv(clientFd_, &messageLen, 1, 0) <= 0)
      {
        break;
      }

      unsigned int recvLen = 0;
      do
      {
        int chunkLen = lwip_recv(clientFd_, &buf[recvLen], messageLen - recvLen, 0);
        if (chunkLen <= 0)
        {
          break;
        }
        recvLen += chunkLen;
      } while (recvLen < messageLen);

      if (recvLen != messageLen)
      {
        break;
      }
     
      TCPMessage message = { messageLen, new uint8_t[messageLen] };
      memcpy(message.data, buf, message.size);
      if (!enqueue(message))
      {
        delete[] message.data;
      }
      memset(buf, 0, maxMessageLen_);
    }

    lwip_close_r(clientFd_);
    clientFd_ = 0;
  }
  vTaskDelete(nullptr);
}

void TCPListener::setSocketFlags(const unsigned int socket)
{
  int optFlag = 1;
  //setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&optFlag, sizeof(int)); Maybe evil?
  setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optFlag, sizeof(int));
  #ifdef TCP_QUICKACK
  setsockopt(socket, IPPROTO_TCP, TCP_QUICKACK, (char*)&optFlag, sizeof(int));
  #endif
}

#endif