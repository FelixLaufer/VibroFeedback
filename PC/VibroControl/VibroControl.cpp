#include "VibroControl.h"

#include <boost/filesystem.hpp>

const std::vector<std::string> VibroControl::Cfg::hosts = { "192.168.4.1" };
const unsigned int VibroControl::Cfg::port = 4545;

VibroControl::VibroControl(const DeviceType device, const unsigned int port)
  : TcpClient(Cfg::hosts[device], VibroControl::Cfg::port, 10000, true)
  , device_(device)
{}

VibroControl::~VibroControl()
{
  for (unsigned int i = 0; i < 8; ++i)
    set(i, { 0, 0 });
}

bool VibroControl::isAlive()
{
  const std::vector<uint8_t> ret = sendReceiveData({ 1, 'a' });
  return ret.size() > 0 && ret[0] == 255;
}

void VibroControl::set(const unsigned int motorId, const ScalarType strength, const ScalarType duration)
{
  set(validMotorId(motorId) , { { strength, duration } });
}

void VibroControl::set(const unsigned int motorId, const Vibration& vibration)
{
  set(validMotorId(motorId) , { { vibration.first, vibration.second } });
}

void VibroControl::set(const unsigned int motorId, const VibrationPattern& vibrationPattern)
{
  const unsigned int size = vibrationPattern.size();
  if (size > 125)
    return;

  std::vector<uint8_t> data(4 + 2 * size);
  data[0] = 3 + 2 * size;
  data[1] = 'v';
  data[2] = validMotorId(motorId);
  data[3] = size;

  for (unsigned int s = 0; s < size; ++s)
  {
    data[4 + 2 * s] = validStrength(vibrationPattern[s].first);
    data[4 + 2 * s + 1] = validDuration(vibrationPattern[s].second);
  }

  sendData(data);
}

void VibroControl::upload(const VibrationPattern & vibrationPattern, const unsigned int patternId)
{
  const unsigned int size = vibrationPattern.size();
  if (size > 125)
    return;

  std::vector<uint8_t> data(4 + 2 * size);
  data[0] = 3 + 2 * size;
  data[1] = 'u';
  data[2] = validPatternId(patternId);
  data[3] = size;

  for (unsigned int s = 0; s < size; ++s)
  {
    data[4 + 2 * s] = validStrength(vibrationPattern[s].first);
    data[4 + 2 * s + 1] = validDuration(vibrationPattern[s].second);
  }

  sendData(data);
}

void VibroControl::play(const unsigned int motorId, const unsigned int patternId)
{
  sendData({ 3, 'p', validMotorId(motorId), validPatternId(patternId) });
}

std::string VibroControl::requestLog()
{
  std::vector<uint8_t> log = sendReceiveData({ 1, 'l' });
  if (log.size() == 0)
    return std::string();

  if (static_cast<char>(log.back()) != '\0')
    log.push_back('\0');

  return std::string(log.data(), log.data() + log.size());
}

void VibroControl::reset()
{
  sendData({ 1, 'r' });
}

uint8_t VibroControl::validMotorId(const unsigned int motorId)
{
  return motorId > 8 ? 0 : motorId;
}

uint8_t VibroControl::validStrength(const ScalarType strength)
{
  const ScalarType normDuty = strength < 0 ? 0 : strength > 1 ? 1 : strength;
  return normDuty * 255;
}

uint8_t VibroControl::validDuration(const ScalarType duration)
{
  const int unitDurations = std::round(duration * 20);
  return unitDurations < 0 ? 0 : unitDurations > 255 ? 255 : unitDurations;
}

uint8_t VibroControl::validPatternId(const unsigned int patternId)
{
  return patternId > 255 ? 0 : patternId;
}