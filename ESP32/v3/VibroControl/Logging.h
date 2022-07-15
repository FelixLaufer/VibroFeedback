#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "Arduino.h"
#include "EEPROM.h"
#include "Consumer.h"
#include "Producer.h"
#include "Tools.h"

struct LogEntry
{
  unsigned int size;
  uint8_t* data;
  unsigned long timeStamp;
};

class Logging
{
  // Don't reserve more than 2MB (half) of external flash
  const static unsigned int storageMaxBytes = 2000000;

public:
  static void init(const unsigned int storageMaxBytes);
  static bool log(const LogEntry& logEntry);
  static bool log(const unsigned long timeStamp, const String& message);

  static String readAll();

  static unsigned int logSize();
  static void readBytes(uint8_t* buf, const unsigned int startByte, const unsigned int size);

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

bool Logging::log(const unsigned long timeStamp, const String& message)
{
  LogEntry logEntry = { message.length(), new uint8_t[message.length()], timeStamp };
  memcpy(logEntry.data, message.c_str(), message.length());
  return log(logEntry);
}

void Logging::dequeue(const LogEntry& logEntry)
{
  const unsigned long ts = (logEntry.timeStamp > 0 ? logEntry.timeStamp : Tools::getMillis());
  const String tsStr = String(logEntry.timeStamp) + String(':');
  const unsigned int totalSize = tsStr.length() + logEntry.size + 1;

  xSemaphoreTake(mtx_, portMAX_DELAY);
  addr_ = addr_ + totalSize > storageBytes_ ? 0 : addr_;
  addr_ += EEPROM.writeString(addr_, tsStr);
  addr_ += EEPROM.writeBytes(addr_, logEntry.data, logEntry.size);
  addr_ += EEPROM.writeString(addr_, String('\n'));
  xSemaphoreGive(mtx_);

  if (logEntry.data != nullptr)
  {
    delete[] logEntry.data;
  }
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