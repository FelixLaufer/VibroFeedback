#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include <boost/asio.hpp>
#include <atomic>
#include <vector>

class TcpClient
{
public:
  TcpClient(const std::string& host, const unsigned int port, const unsigned int timeout = 10000, const bool verbose = false);
  ~TcpClient();

  void connect();
  void disconnect();
  bool isConnected() const { return connected_; }

  void sendData(const uint8_t* data, const unsigned int size);
  void sendData(const std::vector<uint8_t>& data);
  std::vector<uint8_t> TcpClient::receiveData();
  std::vector<uint8_t> sendReceiveData(const uint8_t* data, const unsigned int size);
  std::vector<uint8_t> sendReceiveData(const std::vector<uint8_t>& data);

protected:
  void await(const unsigned int timeout);

  boost::asio::io_service ioService_;
  std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
  bool connected_;
  std::string host_;
  unsigned port_;
  unsigned int timeout_;
  bool verbose_;
};

#endif