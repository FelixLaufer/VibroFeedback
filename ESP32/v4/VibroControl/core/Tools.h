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

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "Arduino.h"

#include <vector>
#include <nvs_flash.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>

enum PowerSaveMode
{
  CLOCKDOWN,
  WIFILOWPOWER
};

class Tools
{
public:
  static void setPowerSaveMode(const PowerSaveMode& powerSaveMode);
  static void setPowerSaveModes(const std::vector<PowerSaveMode>& powerSaveModes);
  static unsigned long getMillis();
  static void resetTime();
  static void restart();

private:
  struct Init
  {
    Init()
    {
      Serial.begin(9600);
    }
  };

  static Init init_;
  static unsigned long zeroTimeStamp_;
  static SemaphoreHandle_t mtx_;
};

Tools::Init Tools::init_ = Tools::Init();
unsigned long Tools::zeroTimeStamp_ = 0;
SemaphoreHandle_t Tools::mtx_ = xSemaphoreCreateMutex();

void Tools::setPowerSaveMode(const PowerSaveMode& powerSaveMode)
{
  switch (powerSaveMode)
  {
  case PowerSaveMode::CLOCKDOWN:
  {
    esp_pm_config_esp32_t pmConfig =
    {
      .max_cpu_freq = RTC_CPU_FREQ_80M,
      .min_cpu_freq = RTC_CPU_FREQ_XTAL
    };
    esp_pm_configure(&pmConfig);
    break;
  }
  case PowerSaveMode::WIFILOWPOWER:
  {
    esp_wifi_set_ps((wifi_ps_type_t)1);
  }
  default:
    break;
  }
}

void Tools::setPowerSaveModes(const std::vector<PowerSaveMode>& powerSaveModes)
{
  for (const PowerSaveMode& powerSaveMode : powerSaveModes)
  {
    setPowerSaveMode(powerSaveMode);
  }
}

unsigned long Tools::getMillis()
{
  xSemaphoreTake(mtx_, portMAX_DELAY);
  const unsigned long ret = static_cast<unsigned long>(xTaskGetTickCount() * portTICK_PERIOD_MS) - zeroTimeStamp_;
  xSemaphoreGive(mtx_);
  return ret;
}

void Tools::resetTime()
{
  xSemaphoreTake(mtx_, portMAX_DELAY);
  zeroTimeStamp_ = static_cast<unsigned long>(xTaskGetTickCount() * portTICK_PERIOD_MS);
  xSemaphoreGive(mtx_);
}

void Tools::restart()
{
  esp_restart();
}


#endif