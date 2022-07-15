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

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Producer.h"
#include "Tools.h"

#include <driver/gpio.h>

struct ButtonEvent
{
  enum Type
  {
    NONE = 0,
    PRESSED = 1,
    RELEASED = 2
  };

  unsigned long timeStamp;
  Type type;
};

class Button : public Producer<ButtonEvent>
{
public:
  Button(const uint8_t pin, void(*callback)(const ButtonEvent&), const unsigned int debouncingTime = 10);

private:
  struct Init
  {
    Init()
    {
      gpio_install_isr_service(0);
    }
  };

  bool stateChanged();
  static void IRAM_ATTR buttonEventISR(void* self);

  uint8_t pin_;
  unsigned long debouncingTime_;
  unsigned long lastChangedTs_;
  volatile bool state_;
  volatile bool lastState_;
  void(*callback_)(const ButtonEvent&);
  static portMUX_TYPE mtx_;

  static Init init_;
};

portMUX_TYPE Button::mtx_ = portMUX_INITIALIZER_UNLOCKED;
Button::Init Button::init_ = Button::Init();

Button::Button(const uint8_t pin, void(*callback)(const ButtonEvent&), const unsigned int debouncingTime)
  : Producer<ButtonEvent>(callback, 2, 1, 0, 1000), pin_(pin), debouncingTime_(debouncingTime), state_(false), lastState_(false), lastChangedTs_(Tools::getMillis()), callback_(callback)
{
  gpio_config_t io_conf;
  io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = 1ULL << pin_;
  io_conf.pull_down_en = (gpio_pulldown_t)0;
  io_conf.pull_up_en = (gpio_pullup_t)1;
  gpio_config(&io_conf);

  gpio_set_intr_type((gpio_num_t)pin_, GPIO_INTR_ANYEDGE);
  gpio_isr_handler_add((gpio_num_t)pin_, buttonEventISR, this);
}

bool Button::stateChanged()
{
  const unsigned long ts = Tools::getMillis();
  state_ = !digitalRead(pin_);
  bool ret = false;
  if ((!lastState_ && state_ || lastState_ && !state_) && ts - lastChangedTs_ > debouncingTime_)
  {
    lastChangedTs_ = ts;
    lastState_ = state_;
    ret = true;
  }
  return ret;
}

void IRAM_ATTR Button::buttonEventISR(void* self)
{
  portENTER_CRITICAL_ISR(&mtx_);
  const unsigned int timeStamp = Tools::getMillis();
  Button* buttonPtr = reinterpret_cast<Button*>(self);
  if (buttonPtr->stateChanged())
  {
    ButtonEvent buttonEvent = { timeStamp, ButtonEvent::Type::NONE };
    if (buttonPtr->state_)
    {
      buttonEvent.type = ButtonEvent::Type::PRESSED;
    }
    else
    {
      buttonEvent.type = ButtonEvent::Type::RELEASED;
    }
    buttonPtr->enqueueFromISR(buttonEvent);
  }
  portEXIT_CRITICAL_ISR(&mtx_);
}

#endif