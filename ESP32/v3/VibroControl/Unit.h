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

#ifndef _UNIT_H_
#define _UNIT_H_

enum EnqueueOption
{
  TRYQUEUING,
  OVERWRITE,
  BLOCKING
};

template <typename T>
class Unit
{
  // Default stack size for worker tasks: 5kB
  //const static unsigned int stackSize = 5000;

public:
  Unit();
  Unit(const unsigned int maxQueueItems = 1, const uint8_t priority = 1, const uint8_t core = 1, const unsigned int stackSize = 5000);
  virtual ~Unit();

protected:
  virtual bool enqueue(const T& item, const EnqueueOption& eOption = EnqueueOption::TRYQUEUING);
  virtual bool enqueueFromISR(const T& item, const EnqueueOption& eOption = EnqueueOption::TRYQUEUING);
  virtual void dequeue(const T& item) = 0;
  
private:
  void worker();

  QueueHandle_t itemQueue_;
  TaskHandle_t workerTaskHandle_;
};

template <typename T>
Unit<T>::Unit()
  : itemQueue_(nullptr), workerTaskHandle_(nullptr)
{}

template <typename T>
Unit<T>::Unit(const unsigned int maxQueueItems, const uint8_t priority, const uint8_t core, const unsigned int stackSize)
  : itemQueue_(xQueueCreate(maxQueueItems, sizeof(T))), workerTaskHandle_(nullptr)
{
  xTaskCreatePinnedToCore([](void* self) { reinterpret_cast<Unit<T>*>(self)->worker(); }, "Unit::worker()", stackSize, this, priority, &workerTaskHandle_, core);
}

template <typename T>
Unit<T>::~Unit()
{
  if (workerTaskHandle_ != nullptr)
  {
    vTaskDelete(workerTaskHandle_);
  }

  if (itemQueue_ != nullptr)
  {
    vQueueDelete(itemQueue_);
  }
}

template <typename T>
bool Unit<T>::enqueue(const T& item, const EnqueueOption& eOption)
{
  if (itemQueue_ == nullptr || workerTaskHandle_ == nullptr)
  {
    return false;
  }

  BaseType_t xHigherPriorityTaskWoken;
  switch (eOption)
  {
  case EnqueueOption::BLOCKING:
    return xQueueSend(itemQueue_, &item, portMAX_DELAY) == pdTRUE;
  case EnqueueOption::OVERWRITE:
    return xQueueOverwrite(itemQueue_, &item) == pdPASS;
  default:
    return xQueueSend(itemQueue_, &item, 0) == pdTRUE;
  }
}

template <typename T>
bool Unit<T>::enqueueFromISR(const T& item, const EnqueueOption& eOption)
{
  if (itemQueue_ == nullptr || workerTaskHandle_ == nullptr)
  {
    return false;
  }

  BaseType_t xHigherPriorityTaskWoken;
  switch (eOption)
  {
  case EnqueueOption::OVERWRITE:
    return xQueueOverwriteFromISR(itemQueue_, &item, &xHigherPriorityTaskWoken) == pdPASS;
  default:
    return xQueueSendFromISR(itemQueue_, &item, &xHigherPriorityTaskWoken) == pdTRUE;
  }
}

template <typename T>
void Unit<T>::worker()
{
  T item;
  while (true)
  {
    xQueueReceive(itemQueue_, &item, portMAX_DELAY);
    dequeue(item);
  }
  vTaskDelete(nullptr);
}

#endif