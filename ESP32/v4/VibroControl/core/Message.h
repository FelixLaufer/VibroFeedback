/*
 *******************************************************
 * VibroControl µC Framework
 *
 * Version 4.0, 04.01.2019
 * Author: Felix Laufer
 * Contact: laufer@cs.uni-kl.de
 *
 * AG wearHEALTH
 * Department of Computer Science
 * TU Kaiserslautern (TUK), Germany
 *******************************************************
 */

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

class Message
{
public:
  Message();
  Message(const unsigned int size);
  Message(const uint8_t* data, const unsigned int size);
  Message(const Message& other);
  virtual Message& operator=(const Message& other);
  Message(Message&& other);
  virtual Message& operator=(Message&& other);
  virtual void destroy();

protected:
  uint8_t* data_;
  unsigned int size_;
};

Message::Message()
  : data_(nullptr), size_(0)
{}

Message::Message(const unsigned int size)
  : data_(new uint8_t[size]), size_(size)
{}

Message::Message(const uint8_t* data, const unsigned int size)
  : data_(new uint8_t[size]), size_(size)
{
  memcpy(data_, data, size);
}

Message::Message(const Message& other)
  : data_(new uint8_t[other.size_]), size_(other.size_)
{
  memcpy(data_, other.data_, other.size_);
}

Message& Message::operator=(const Message& other)
{
  if (this != &other)
  {
    destroy();
    data_ = new uint8_t[other.size_];
    size_ = other.size_;
    memcpy(data_, other.data_, other.size_);
  }
  return *this;
}

Message::Message(Message&& other)
  : data_(other.data_), size_(other.size_)
{
  other.size_ = 0;
  other.data_ = nullptr;
}

Message& Message::operator=(Message&& other)
{
  if (this != &other)
  {
    destroy();
    data_ = other.data_;
    size_ = other.size_;
    other.data_ = nullptr;
    other.size_ = 0;
  }
  return *this;
}

void Message::destroy()
{
  if (data_ != nullptr)
  {
    delete[] data_;
    data_ = nullptr;
  }
  size_ = 0;
}

#endif