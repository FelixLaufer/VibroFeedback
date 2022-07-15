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

#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "Consumer.h"

#include <vector>
#include <Arduino.h">
#include <driver/ledc.h>
#include <driver/gpio.h>

struct Vibration
{
  uint8_t strength;
  uint8_t duration;
};

struct VibrationPattern
{
  unsigned int size;
  uint8_t* data;
};

class Motor : public Consumer<Vibration>
{
  const static unsigned int pwmFrequency = 10000; // 10kHz
  const static uint8_t safeMinMotorStrength = 100; // ~= 40% @ 3.3Volt Vcc ~= 1.3Volt

public:
  Motor(const uint8_t id, const uint8_t pin);
  bool vibrate(const uint8_t strength);
  bool vibrate(const Vibration& vibration);
  bool vibrate(const VibrationPattern& vibrationPattern);

private:
  struct GPIOConfig
  {
    GPIOConfig(const uint8_t pin)
    {
      gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
      gpio_set_level((gpio_num_t)pin, 0);
    }
  };

  struct LedcChannelConfig
  {
    ledc_channel_t channel;
    ledc_channel_config_t channelConfig;

    LedcChannelConfig(const uint8_t id, const uint8_t pin)
    {
      channel = (ledc_channel_t)id;
      channelConfig = { (gpio_num_t)pin, LEDC_HIGH_SPEED_MODE, channel, LEDC_INTR_DISABLE, LEDC_TIMER_0, 0 };
      ledc_channel_config(&channelConfig);
    }
  };

  struct LedcTimerConfig
  {
    ledc_timer_config_t ledcTimer;

    LedcTimerConfig()
    {
      ledcTimer = { LEDC_HIGH_SPEED_MODE, LEDC_TIMER_10_BIT, LEDC_TIMER_0, Motor::pwmFrequency };
      ledc_timer_config(&ledcTimer);
    }
  };

  uint8_t get() const;
  void set(const uint8_t strength) const;
  void dequeue(const Vibration& vibration) override;

  uint8_t id_;
  uint8_t pin_;
  SemaphoreHandle_t mtx_;
  GPIOConfig gpioConfig_;
  LedcChannelConfig ledcChannelConfig_;
  static LedcTimerConfig ledcTimerConfig_;
};

Motor::LedcTimerConfig Motor::ledcTimerConfig_ = Motor::LedcTimerConfig();

Motor::Motor(const uint8_t id, const uint8_t pin)
  : Consumer<Vibration>(255, 1, 1, 5000), id_(id), pin_(pin), mtx_(xSemaphoreCreateMutex()), gpioConfig_(GPIOConfig(pin)), ledcChannelConfig_(LedcChannelConfig(id, pin))
{}

uint8_t Motor::get() const
{
  const uint32_t duty = ledc_get_duty(LEDC_HIGH_SPEED_MODE, ledcChannelConfig_.channel);
  return lroundf(((float)duty / (float)((1 << LEDC_TIMER_10_BIT) - 1)) * (float)((1 << 8) - 1));
}

void Motor::set(const uint8_t strength) const
{
  const uint8_t targetMotorDuty = (strength < safeMinMotorStrength) ? 0 : strength;
  const uint32_t maxDuty = (1 << LEDC_TIMER_10_BIT) - 1;
  const uint32_t duty = lroundf(((float)targetMotorDuty / (float)((1 << 8) - 1)) * (float)maxDuty);

  xSemaphoreTake(mtx_, portMAX_DELAY);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, ledcChannelConfig_.channel, duty);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, ledcChannelConfig_.channel);
  xSemaphoreGive(mtx_);
}

bool Motor::vibrate(const uint8_t strength)
{
  return vibrate(Vibration({ strength, 0 }));
}

bool Motor::vibrate(const Vibration& vibration)
{
  return Consumer<Vibration>::enqueue(vibration);
}

bool Motor::vibrate(const VibrationPattern& vibrationPattern)
{
  bool ret = true;
  for (unsigned int i = 0; i < vibrationPattern.size; ++i)
  {
    ret &= Consumer<Vibration>::enqueue(Vibration({ vibrationPattern.data[2 * i],  vibrationPattern.data[2 * i + 1] }));
  }
  return ret;
}

void Motor::dequeue(const Vibration& vibration)
{
  if (vibration.duration > 0)
  {
    set(vibration.strength);
    vTaskDelay(50 * vibration.duration / portTICK_PERIOD_MS);
    set(0);
    return;
  }

  set(vibration.strength);
}

#endif