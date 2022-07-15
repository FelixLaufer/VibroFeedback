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

#ifndef _PRODUCER_H_
#define _PRODUCER_H_

#include "Unit.h"

template <typename T>
class Producer : public Unit<T>
{
public:
  Producer();
  Producer(void(*callback)(const T&), const unsigned int maxQueueItems = 1, const uint8_t priority = 1, const uint8_t core = 0, const unsigned int stackSize = 5000);
  virtual ~Producer();

protected:
  virtual void dequeue(const T& item) override;

  void(*callback_)(const T&);
};

template <typename T>
Producer<T>::Producer()
  : Unit<T>()
{}

template <typename T>
Producer<T>::Producer(void(*callback)(const T&), const unsigned int maxQueueItems, const uint8_t priority, const uint8_t core, const unsigned int stackSize)
  : Unit<T>(maxQueueItems, priority, core, stackSize), callback_(callback)
{}

template <typename T>
Producer<T>::~Producer()
{
  callback_ = nullptr;
}

template <typename T>
void Producer<T>::dequeue(const T& item)
{
  if (callback_ != nullptr)
  {
    callback_(item);
  }
}

#endif