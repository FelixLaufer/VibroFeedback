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

#ifndef _CONSUMER_H_
#define _CONSUMER_H_

#include "Unit.h"

template <typename T>
class Consumer : public Unit<T>
{
public:
  Consumer();
  Consumer(const unsigned int maxQueueItems = 1, const uint8_t priority = 1, const uint8_t core = 1, const unsigned int stackSize = 5000);
  virtual ~Consumer();
  virtual bool enqueue(const T& item, const EnqueueOption& eOption = EnqueueOption::TRYQUEUING) override;
  virtual bool enqueueFromISR(const T& item, const EnqueueOption& eOption = EnqueueOption::TRYQUEUING) override;
};

template <typename T>
Consumer<T>::Consumer()
  : Unit<T>()
{}

template <typename T>
Consumer<T>::Consumer(const unsigned int maxQueueItems, const uint8_t priority, const uint8_t core, const unsigned int stackSize)
  : Unit<T>(maxQueueItems, priority, core, stackSize)
{}

template <typename T>
Consumer<T>::~Consumer()
{}

template <typename T>
bool Consumer<T>::enqueue(const T& item, const EnqueueOption& eOption)
{
  return Unit<T>::enqueue(item, eOption);
}

template <typename T>
bool Consumer<T>::enqueueFromISR(const T& item, const EnqueueOption& eOption)
{
  return Unit<T>::enqueueFromISR(item, eOption);
}

#endif