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

#ifndef _VIBRATION_H_
#define _VIBRATION_H_

#include "../core/Message.h"

class Vibration : public Message
{
public:
  Vibration();
  Vibration(const float amplitude, const unsigned int frequency, float duration);

  const float& amplitude() const;
  const unsigned int& frequency() const;
  const float& duration() const;
};

Vibration::Vibration()
  : Message()
{}

Vibration::Vibration(const float amplitude, const unsigned int frequency, float duration)
  : Message(sizeof(float) + sizeof(unsigned int) + sizeof(float)))
{
  memcpy(data_, &amplitude, sizeof(float));
  memcpy(&data_[sizeof(float)], &frequency, sizeof(unsigned int));
  memcpy(&data_[sizeof(float) + sizeof(unsigned int)], &duration, sizeof(float));
}

const float& Vibration::amplitude() const
{
  return *reinterpret_cast<float*>(&data_[0]);
}

const unsigned int& Vibration::frequency() const
{
  return *reinterpret_cast<unsigned int*>(&data_[sizeof(float)]);
}

const float& Vibration::duration() const
{
  return *reinterpret_cast<float*>(&data_[sizeof(float) + sizeof(unsigned int)]);
}

#endif