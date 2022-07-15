/*
 *******************************************************
 * VibroControl µC Framework
 *
 * Version 4.0, 04.02.2019
 * Author: Felix Laufer
 * Contact: laufer@cs.uni-kl.de
 *
 * AG wearHEALTH
 * Department of Computer Science
 * TU Kaiserslautern (TUK), Germany
 *******************************************************
 */

#ifndef _LOG_ENTRY_H_
#define _LOG_ENTRY_H_

#include "../core/Message.h"

#include <Arduino.h>

class LogEntry : public Message
{
public:
  LogEntry();
  LogEntry(const unsigned long timeStamp, const String& message);

  const unsigned int& timeStamp() const;
  const String& message() const;
};

LogEntry::LogEntry()
  : Message()
{}

LogEntry::LogEntry(const unsigned long timeStamp, const String& message)
  : Message(sizeof(unsigned long) + sizeof(char) * message.length())
{
  memcpy(data_, &timeStamp, sizeof(unsigned long));
  memcpy(&data[sizeof(unsigned long)], message.c_str(), sizeof(char) * message.length());
}

const unsigned int& LogEntry::timeStamp() const
{
  return *reinterpret_cast<unsigned int*>(&data_[0]);
}

const String& LogEntry::message() const
{
  return *reinterpret_cast<String*>(&data_[sizeof(unsigned long)]);
}

#endif