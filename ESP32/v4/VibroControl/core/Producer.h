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

#ifndef _PRODUCER_H_
#define _PRODUCER_H_

#include "Unit.h"

template<typename T>
class Producer : public Unit<T>
{
public:
  Producer(void(*callback)(const T&), const unsigned int maxQueueItems = 1, const unsigned int stackSize = 5000, const uint8_t priority = 1, const uint8_t core = 0);
  virtual ~Producer();

protected:
  virtual void dequeue(const T& message) override;

  void(*callback_)(const T&);
};

template<typename T>
Producer<T>::Producer(void(*callback)(const T&), const unsigned int maxQueueItems, const unsigned int stackSize, const uint8_t priority, const uint8_t core)
  : Unit<T>(maxQueueItems, stackSize, priority, core), callback_(callback)
{}

template<typename T>
Producer<T>::~Producer()
{
  callback_ = nullptr;
}

template<typename T>
void Producer<T>::dequeue(const T& message)
{
  if (callback_ != nullptr)
  {
    callback_(message);
  }
}

#endif