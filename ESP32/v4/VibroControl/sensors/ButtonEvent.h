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

#ifndef _BUTTON_EVENT_H_
#define _BUTTON_EVENT_H_

#include "../core/Message.h"

class ButtonEvent : public Message
{
public:
  enum Type
  {
    PRESSED,
    RELEASED
  };

  ButtonEvent();
  ButtonEvent(const unsigned long timeStamp, const Type type);

  const unsigned long& timeStamp() const;
  const Type& type() const;
};

ButtonEvent::ButtonEvent()
  : Message()
{}

ButtonEvent::ButtonEvent(const unsigned long timeStamp, const Type type)
  : Message(sizeof(unsigned long) + sizeof(Type))
{
  memcpy(data_, &timeStamp, sizeof(unsigned long));
  memcpy(&data_[sizeof(unsigned long)], &type, sizeof(Type));
}

const unsigned long& ButtonEvent::timeStamp() const
{
  return *reinterpret_cast<unsigned long*>(&data_[0]);
}

const ButtonEvent::Type& ButtonEvent::type() const
{
  return *reinterpret_cast<Type*>(&data_[sizeof(unsigned long)]);
}

#endif