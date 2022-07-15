#ifndef _VIBRO_CONTROL_H_
#define _VIBRO_CONTROL_H_

#include "TcpClient.h"
typedef double ScalarType;

#include <vector>

class VibroControl : public TcpClient
{
public:
  enum DeviceType
  {
    Pelvis = 0,
    LeftUpperLeg = 1,
    RightUpperLeg = 2,
    LeftLowerLeg = 3,
    RightLowerLeg = 4
  };

  struct Cfg
  {
    static const std::vector<std::string> hosts;
    static const unsigned int port;
  };

  // ToDo: make Vibration a struct!
  using Vibration = std::pair<ScalarType, ScalarType>;
  using VibrationPattern = std::vector<Vibration>;

  VibroControl(const DeviceType device = DeviceType::Pelvis, const unsigned int port = Cfg::port);
  ~VibroControl();

  inline DeviceType getDeviceType() const { return device_; }

  // Ping the device
  bool isAlive();
  // Set motor [1, 4] to strength factor [0, 1] (fractional) for [0, 10] (fractional) seconds (if duration = 0, vibrate forever)
  void set(const unsigned int motorId, const ScalarType strength, const ScalarType duration = 0);
  // Set motor [1, 4] to vibration with { strength = [0, 1], duration = [0, 1] }
  void set(const unsigned int motorId, const Vibration& vibration);
  // Set motor [1, 4] to vibrations pattern, i.e. sequence of vibrations
  void set(const unsigned int motorId, const VibrationPattern& vibrationPattern);
  // Upload a vibration pattern to the device and address it with pattern id [0, 255]
  void upload(const VibrationPattern& vibrationPattern, const unsigned int patternId);
  // Play an earlier uploaded vibration pattern addressed with pattern id [0, 255]
  void play(const unsigned int motorId, const unsigned int patternId);
  // request the device log
  std::string requestLog();
  // Reset the device time and log
  void reset();

private:
  DeviceType device_;

  static uint8_t validMotorId(const unsigned int motorId);
  static uint8_t validStrength(const ScalarType strength);
  static uint8_t validDuration(const ScalarType duration);
  static uint8_t validPatternId(const unsigned int patternId);
};

#endif