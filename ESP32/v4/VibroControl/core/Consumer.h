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

#ifndef _CONSUMER_H_
#define _CONSUMER_H_

#include "Unit.h"

template<typename T>
class Consumer : public Unit<T>
{
public:
  Consumer(const unsigned int maxQueueItems = 1, const unsigned int stackSize = 5000, const uint8_t priority = 1, const uint8_t core = 1);
  virtual ~Consumer();
  virtual bool enqueue(const T& message, const EnqueueOption& eOption = EnqueueOption::TRYQUEUING, const bool fromISR = false) override;
  virtual bool enqueueFromISR(const T& message, const EnqueueOption& eOption = EnqueueOption::TRYQUEUING) override;
};

template<typename T>
Consumer<T>::Consumer(const unsigned int maxQueueItems, const unsigned int stackSize, const uint8_t priority, const uint8_t core)
  : Unit<T>(maxQueueItems, stackSize, priority, core)
{}

template<typename T>
Consumer<T>::~Consumer()
{}

template<typename T>
bool Consumer<T>::enqueue(const T& message, const EnqueueOption& eOption, const bool fromISR)
{
  return Unit<T>::enqueue(message, eOption, fromISR);
}

template<typename T>
bool Consumer<T>::enqueueFromISR(const T& message, const EnqueueOption& eOption)
{
  return Unit<T>::enqueueFromISR(message, eOption);
}

#endif