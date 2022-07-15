#include "TcpClient.h"

#include <boost/asio/high_resolution_timer.hpp>
#include <boost/array.hpp>
#include <iostream>

TcpClient::TcpClient(const std::string& host, const unsigned int port, const unsigned int timeout, const bool verbose)
  : socket_(new boost::asio::ip::tcp::socket(ioService_))
  , connected_(false)
  , host_(host)
  , port_(port)
  , timeout_(timeout)
  , verbose_(true)
{
  connect();
}

TcpClient::~TcpClient()
{
  disconnect();
}

void TcpClient::connect()
{
  disconnect();
  socket_.reset(new boost::asio::ip::tcp::socket(ioService_));

  try
  {
    async_connect(*socket_, boost::asio::ip::tcp::resolver(ioService_).resolve({ host_, std::to_string(port_) }),
    #ifndef WINDOWS
      [&](boost::system::error_code ec, boost::asio::ip::basic_resolver_iterator<boost::asio::ip::tcp> it)
    #else
      [&](boost::system::error_code ec, boost::asio::ip::tcp::endpoint to)
    #endif
    {
      if (ec)
      {
        if (verbose_)
          std::cerr << "Unable to connect to host " << host_ << ":" << std::to_string(port_) << std::endl;
      }
      else
      {
        connected_ = true;
        socket_->set_option(boost::asio::socket_base::keep_alive(true));
        socket_->set_option(boost::asio::ip::tcp::no_delay(true));
      }
    });
    await(timeout_);
  }
  catch (const boost::system::system_error& e)
  {
    if (verbose_)
      std::cerr << e.what() << std::endl;
  }
}

void TcpClient::disconnect()
{
  if (socket_ != nullptr)
    return;

  try
  {
    if (socket_->is_open())
    {
      socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
      socket_->close();
    }
  }
  catch (const boost::system::system_error& e)
  {
    if (verbose_)
      std::cerr << e.what() << std::endl;
  }
  socket_.reset();
  connected_ = false;
}

void TcpClient::sendData(const uint8_t* data, const unsigned int size)
{
  bool err = false;
  try
  {
    async_write(*socket_, boost::asio::buffer(data, size), boost::asio::transfer_all(), [&](boost::system::error_code ec, size_t bytesTransferred)
    {
      if (ec)
      {
        err = true;
        if (verbose_)
          std::cerr << "TcpClient timeout: unable to send data."  << std::endl;
      }
    });
    await(timeout_);
  }
  catch (const boost::system::system_error& e)
  {
    err = true;
    if (verbose_)
      std::cerr << e.what() << std::endl;
  }
}

void TcpClient::sendData(const std::vector<uint8_t>& data)
{
  sendData(data.data(), data.size());
}

std::vector<uint8_t> TcpClient::receiveData()
{
  std::vector<uint8_t> ret;
  bool err = false;
  try
  {
    uint8_t lenghtSize = 0;
    socket_->async_receive(boost::asio::buffer(&lenghtSize, 1), [&](boost::system::error_code ec, size_t bytesReceived)
    {
      if (ec)
      {
        err = true;
        if (verbose_)
          std::cerr << "TcpClient timeout: unable to receive message length size byte." << std::endl;
      }
    });
    await(timeout_);
    if (err || lenghtSize == 0)
      return ret;

    std::vector<uint8_t> lengthBytes(lenghtSize);
    socket_->async_receive(boost::asio::buffer(lengthBytes, lengthBytes.size()), [&](boost::system::error_code ec, size_t bytesReceived)
    {
      if (ec)
      {
        err = true;
        if (verbose_)
          std::cerr << "TcpClient timeout: unable to receive message length bytes." << std::endl;
      }
    });
    await(timeout_);
    unsigned int bytesExpected = 0;
    memcpy(&bytesExpected, reinterpret_cast<const unsigned int*>(lengthBytes.data()), lenghtSize);
    if (err || bytesExpected == 0)
      return ret;

    ret.resize(bytesExpected);
    unsigned int totalBytesReceived = 0;
    while (!err && totalBytesReceived < bytesExpected)
    {
      socket_->async_receive(boost::asio::buffer(ret.data() + totalBytesReceived, bytesExpected - totalBytesReceived), [&](boost::system::error_code ec, size_t bytesReceived)
      {
        if (ec)
        {
          err = true;
          if (verbose_)
            std::cerr << "TcpClient timeout: unable to receive message: " << ec.message() << std::endl;
        }
        else
          totalBytesReceived += bytesReceived;
      });
      await(timeout_);
    }
  }
  catch (const boost::system::system_error& e)
  {
    err = true;
    if (verbose_)
      std::cerr << e.what() << std::endl;
  }

  return ret;
}

std::vector<uint8_t> TcpClient::sendReceiveData(const uint8_t* data, const unsigned int size)
{
  sendData(data, size);
  return receiveData();
}

std::vector<uint8_t> TcpClient::sendReceiveData(const std::vector<uint8_t>& data)
{
  return sendReceiveData(data.data(), data.size());
}

void TcpClient::await(const unsigned int timeout)
{
  try
  {
    ioService_.reset();
    {
      boost::asio::high_resolution_timer tm(ioService_, std::chrono::milliseconds(timeout));
      tm.async_wait([this](boost::system::error_code ec)
      {
        if (ec != boost::asio::error::operation_aborted)
          socket_->cancel();
      });
      ioService_.run_one();
    }
    ioService_.run();
  }
  catch (const boost::system::system_error& e)
  {
    std::cerr << e.what() << std::endl;
  }
}