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

#ifndef _WIFI_MANAGER_H_
#define _WIFI_MANAGER_H_

#include "Arduino.h"
#include "WiFi.h"

#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <esp_event.h>
#include <esp_event_loop.h>

struct WiFiConfig
{
  enum WiFiMode
  {
    AP,
    STATION
  };

  WiFiMode mode;
  String ssid;
  String password;
  uint8_t channel;
  IPAddress ip;
  IPAddress gateway;
  IPAddress subnet;
};

class WiFiManager
{
public:
  static void init(const WiFiConfig& wifiConfig, void(*callback)());

private:
  static void accessPointInit();
  static void stationInit();

  static WiFiConfig wifiConfig_;
  static void(*callback_)();
};

WiFiConfig WiFiManager::wifiConfig_ = {};
void(*WiFiManager::callback_)() = nullptr;

void WiFiManager::init(const WiFiConfig& wifiConfig, void(*callback)())
{
  wifiConfig_ = wifiConfig;
  callback_ = callback;

  if (wifiConfig_.mode == WiFiConfig::WiFiMode::AP)
  {
    accessPointInit();
  }
  else if (wifiConfig_.mode == WiFiConfig::WiFiMode::STATION)
  {
    stationInit();
  }
}

void WiFiManager::accessPointInit()
{
  WiFi.softAPConfig(wifiConfig_.ip, wifiConfig_.gateway, wifiConfig_.subnet);
  WiFi.softAP(wifiConfig_.ssid.c_str(), wifiConfig_.password.c_str(), wifiConfig_.channel);

  callback_();
}

void WiFiManager::stationInit()
{
  tcpip_adapter_init();

  esp_event_loop_init([&](void* ctx, system_event_t* e)
  {
    switch (e->event_id)
    {
    case SYSTEM_EVENT_STA_START:
    {
      esp_wifi_connect();
      break;
    }
    case SYSTEM_EVENT_STA_CONNECTED:
    {
      break;
    }
    case SYSTEM_EVENT_STA_DISCONNECTED:
    {
      esp_wifi_connect();
      break;
    }
    case SYSTEM_EVENT_STA_GOT_IP:
    {
      callback_();
      break;
    }
    default:
      break;
    }

    return ESP_OK;

  }, NULL);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);

  wifi_config_t sta_config = {};
  strcpy((char*)sta_config.sta.ssid, wifiConfig_.ssid.c_str());
  strcpy((char*)sta_config.sta.password, wifiConfig_.password.c_str());
  sta_config.sta.bssid_set = false;

  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config);
  esp_wifi_start();

  tcpip_adapter_ip_info_t info;
  info.ip.addr = wifiConfig_.ip;
  info.gw.addr = wifiConfig_.gateway;
  info.netmask.addr = wifiConfig_.subnet;
  tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
  tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &info);
}

#endif