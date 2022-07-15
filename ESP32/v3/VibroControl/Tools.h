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

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "Arduino.h"

#include <nvs_flash.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>

class Tools
{
public:
  static void setPowerSaveMode(const uint8_t powerSaveLevel = 1);
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

void Tools::setPowerSaveMode(const uint8_t powerSaveLevel)
{
  if (powerSaveLevel > 0)
  { 
    // Dynamically clock down CPU frequency (80MHz max)
    esp_pm_config_esp32_t pmConfig =
    {
      .max_cpu_freq = RTC_CPU_FREQ_80M,
      .min_cpu_freq = RTC_CPU_FREQ_XTAL
    };
    esp_pm_configure(&pmConfig);
  }
    
  // Enable WiFi power save mode
  if (powerSaveLevel > 1)
  {
    esp_wifi_set_ps((wifi_ps_type_t)1);
  }
}

unsigned long Tools::getMillis()
{
  xSemaphoreTake(mtx_, portMAX_DELAY);
  const unsigned long ret = (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS) - zeroTimeStamp_;
  xSemaphoreGive(mtx_);
  return ret;
}

void Tools::resetTime()
{
  xSemaphoreTake(mtx_, portMAX_DELAY);
  zeroTimeStamp_ = (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS);
  xSemaphoreGive(mtx_);
}

void Tools::restart()
{
  esp_restart();
}


#endif