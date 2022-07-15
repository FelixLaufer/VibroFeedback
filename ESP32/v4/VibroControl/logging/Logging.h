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

#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "LogEntry.h"
#include "../core/Consumer.h"
#include "../core/Tools.h"

#include <Arduino.h>
#include <EEPROM.h>

class Logging
{
  // Don't reserve more than 2MB (half) of external flash
  const static unsigned int storageMaxBytes = 2000000;

public:
  static void init(const unsigned int storageMaxBytes);
  static bool log(const LogEntry& logEntry);

  static unsigned int logSize();
  static void readBytes(uint8_t* buf, const unsigned int startByte, const unsigned int size);
  static String readAll();

private:
 
  class LogWriter : public Consumer<LogEntry>
  {
  public:
    LogWriter();

  private:
    void dequeue(const LogEntry& logEntry) override;
  };
  

  friend class LogWriter;

  static void dequeue(const LogEntry& logEntry);

  static unsigned int storageBytes_;
  static unsigned int addr_;
  static LogWriter logWriter_;
  static SemaphoreHandle_t mtx_;
};

Logging::LogWriter::LogWriter()
  : Consumer<LogEntry>(255, 1, 1)
{}

void Logging::LogWriter::dequeue(const LogEntry& logEntry)
{
  Logging::dequeue(logEntry);
}

unsigned int Logging::storageBytes_ = Logging::storageMaxBytes;
unsigned int Logging::addr_ = 0;
Logging::LogWriter Logging::logWriter_ = Logging::LogWriter();
SemaphoreHandle_t Logging::mtx_ = xSemaphoreCreateMutex();

void Logging::init(const unsigned int storageBytes)
{
  xSemaphoreTake(mtx_, portMAX_DELAY);
  storageBytes_ = storageBytes > storageMaxBytes ? storageMaxBytes : storageBytes;
  addr_ = 0;
  EEPROM.begin(storageBytes_);
  xSemaphoreGive(mtx_);

  Tools::resetTime();
}

bool Logging::log(const LogEntry& logEntry)
{
  return logWriter_.enqueue(logEntry);
}

void Logging::dequeue(const LogEntry& logEntry)
{
  const unsigned int long ts = logEntry.timeStamp();
  const String tsStr = String(ts > 0 ? ts : Tools::getMillis()) + String(':');
  const unsigned int totalSize = logEntry.size() + 2;

  xSemaphoreTake(mtx_, portMAX_DELAY);
  addr_ = addr_ + totalSize > storageBytes_ ? 0 : addr_;
  addr_ += EEPROM.writeString(addr_, tsStr);
  addr_ += EEPROM.writeString(addr_, logEntry.message());
  addr_ += EEPROM.writeString(addr_, String('\n'));
  xSemaphoreGive(mtx_);
}

unsigned int Logging::logSize()
{
  xSemaphoreTake(mtx_, portMAX_DELAY);
  const unsigned int ret = addr_;
  xSemaphoreGive(mtx_);

  return ret;
}

void Logging::readBytes(uint8_t* buf, const unsigned int startByte, const unsigned int size)
{
  xSemaphoreTake(mtx_, portMAX_DELAY);
  EEPROM.readBytes(startByte, buf, size);
  xSemaphoreGive(mtx_);
}

String Logging::readAll()
{
  xSemaphoreTake(mtx_, portMAX_DELAY);
  char value[addr_ + 1];
  EEPROM.readString(0, value, addr_);
  value[addr_] = '\0';
  xSemaphoreGive(mtx_);

  return String(value);
}

#endif