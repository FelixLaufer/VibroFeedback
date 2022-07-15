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

#ifndef _UNIT_H_
#define _UNIT_H_

enum EnqueueOption
{
  TRYQUEUING,
  OVERWRITE,
  BLOCKING
};

template<typename T>
class Unit
{
public:
  Unit();
  Unit(const unsigned int maxQueueMessages = 1, const unsigned int stackSize = 5000, const uint8_t priority = 1, const uint8_t core = 0);
  virtual ~Unit();

protected:
  virtual bool enqueue(const T& message, const EnqueueOption& eOption = EnqueueOption::TRYQUEUING, const bool fromISR = false);
  virtual bool enqueueFromISR(const T& message, const EnqueueOption& eOption = EnqueueOption::TRYQUEUING);
  virtual void dequeue(const T& message) = 0;
  
private:
  void worker();

  QueueHandle_t messageQueue_;
  TaskHandle_t workerTaskHandle_;
};

template<typename T>
Unit<T>::Unit()
  : messageQueue_(nullptr), workerTaskHandle_(nullptr)
{}

template<typename T>
Unit<T>::Unit(const unsigned int maxQueueMessages, const unsigned int stackSize, const uint8_t priority, const uint8_t core)
  : messageQueue_(xQueueCreate(maxQueueMessages, sizeof(T))), workerTaskHandle_(nullptr)
{
  xTaskCreatePinnedToCore([](void* self) { reinterpret_cast<Unit<T>*>(self)->worker(); }, "Unit::worker()", stackSize, this, priority, &workerTaskHandle_, core);
}

template<typename T>
Unit<T>::~Unit()
{
  if (workerTaskHandle_ != nullptr)
  {
    vTaskDelete(workerTaskHandle_);
  }

  if (messageQueue_ != nullptr)
  {
    vQueueDelete(messageQueue_);
  }
}

template<typename T>
bool Unit<T>::enqueue(const T& message, const EnqueueOption& eOption, const bool fromISR)
{
  if (messageQueue_ == nullptr || workerTaskHandle_ == nullptr)
  {
    return false;
  }

  if (fromISR)
  {
    BaseType_t xHigherPriorityTaskWoken;
    switch (eOption)
    {
    case EnqueueOption::OVERWRITE:
      return xQueueOverwriteFromISR(messageQueue_, &message, &xHigherPriorityTaskWoken) == pdPASS;
    default:
      return xQueueSendFromISR(messageQueue_, &message, &xHigherPriorityTaskWoken) == pdTRUE;
    }
  }
  else
  {
    BaseType_t xHigherPriorityTaskWoken;
    switch (eOption)
    {
    case EnqueueOption::BLOCKING:
      return xQueueSend(messageQueue_, &message, portMAX_DELAY) == pdTRUE;
    case EnqueueOption::OVERWRITE:
      return xQueueOverwrite(messageQueue_, &message) == pdPASS;
    default:
      return xQueueSend(messageQueue_, &message, 0) == pdTRUE;
    }
  }

  return false;
}

template<typename T>
bool Unit<T>::enqueueFromISR(const T& message, const EnqueueOption& eOption)
{
  return enqueue(message, eOption, true);
}

template<typename T>
void Unit<T>::worker()
{
  T message;
  while (true)
  {
    xQueueReceive(messageQueue_, &message, portMAX_DELAY);
    dequeue(message);
    message.destroy();
  }
  vTaskDelete(nullptr);
}

#endif