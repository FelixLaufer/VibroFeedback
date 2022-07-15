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

#ifndef _DRV_2605L_H_
#define _DRV_2605L_H_

#define DRV2605_ADDR 0x5A

#define DRV2605_REG_STATUS 0x00
#define DRV2605_REG_MODE 0x01
#define DRV2605_MODE_INTTRIG  0x00
#define DRV2605_MODE_EXTTRIGEDGE  0x01
#define DRV2605_MODE_EXTTRIGLVL  0x02
#define DRV2605_MODE_PWMANALOG  0x03
#define DRV2605_MODE_AUDIOVIBE  0x04
#define DRV2605_MODE_REALTIME  0x05
#define DRV2605_MODE_DIAGNOS  0x06
#define DRV2605_MODE_AUTOCAL  0x07

#define DRV2605_REG_RTPIN 0x02
#define DRV2605_REG_LIBRARY 0x03
#define DRV2605_REG_WAVESEQ1 0x04
#define DRV2605_REG_WAVESEQ2 0x05
#define DRV2605_REG_WAVESEQ3 0x06
#define DRV2605_REG_WAVESEQ4 0x07
#define DRV2605_REG_WAVESEQ5 0x08
#define DRV2605_REG_WAVESEQ6 0x09
#define DRV2605_REG_WAVESEQ7 0x0A
#define DRV2605_REG_WAVESEQ8 0x0B

#define DRV2605_REG_GO 0x0C
#define DRV2605_REG_OVERDRIVE 0x0D
#define DRV2605_REG_SUSTAINPOS 0x0E
#define DRV2605_REG_SUSTAINNEG 0x0F
#define DRV2605_REG_BREAK 0x10
#define DRV2605_REG_AUDIOCTRL 0x11
#define DRV2605_REG_AUDIOLVL 0x12
#define DRV2605_REG_AUDIOMAX 0x13
#define DRV2605_REG_RATEDV 0x16
#define DRV2605_REG_CLAMPV 0x17
#define DRV2605_REG_AUTOCALCOMP 0x18
#define DRV2605_REG_AUTOCALEMP 0x19
#define DRV2605_REG_FEEDBACK 0x1A
#define DRV2605_REG_CONTROL1 0x1B
#define DRV2605_REG_CONTROL2 0x1C
#define DRV2605_REG_CONTROL3 0x1D
#define DRV2605_REG_CONTROL4 0x1E
#define DRV2605_REG_VBAT 0x21
#define DRV2605_REG_LRARESON 0x22

#include <Arduino.h>
#include <Wire.h>

class DRV2605L
{
public:
  struct Init()
  {
    // Wire begin...
  }

  DRV2605L();

  void writeRegister8(uint8_t reg, uint8_t val);
  uint8_t readRegister8(uint8_t reg);
  void setWaveform(uint8_t slot, uint8_t w);
  void selectLibrary(uint8_t lib);
  void go();
  void stop();
  void setMode(uint8_t mode);
  void setRealtimeValue(uint8_t rtp);
  void useERM();
  void useLRA();
};

DRV2605L::DRV2605L()
{
  Wire.begin(); //  Wire.begin(21, 22, 100000);
  uint8_t id = readRegister8(DRV2605_REG_STATUS);

  // ToDo: Adapt this!!!
  writeRegister8(DRV2605_REG_MODE, 0x00); // out of standby
  writeRegister8(DRV2605_REG_RTPIN, 0x00); // no real-time-playback
  writeRegister8(DRV2605_REG_WAVESEQ1, 1); // strong click
  writeRegister8(DRV2605_REG_WAVESEQ2, 0);
  writeRegister8(DRV2605_REG_OVERDRIVE, 0); // no overdrive

  writeRegister8(DRV2605_REG_SUSTAINPOS, 0);
  writeRegister8(DRV2605_REG_SUSTAINNEG, 0);
  writeRegister8(DRV2605_REG_BREAK, 0);
  writeRegister8(DRV2605_REG_AUDIOMAX, 0x64);

  // ERM open loop

  // turn off N_ERM_LRA
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
  // turn on ERM_OPEN_LOOP
  writeRegister8(DRV2605_REG_CONTROL3, readRegister8(DRV2605_REG_CONTROL3) | 0x20);

  return true;
}


void DRV2605L::setWaveform(uint8_t slot, uint8_t w)
{
  writeRegister8(DRV2605_REG_WAVESEQ1 + slot, w);
}

void DRV2605L::selectLibrary(uint8_t lib)
{
  writeRegister8(DRV2605_REG_LIBRARY, lib);
}

void DRV2605L::go()
{
  writeRegister8(DRV2605_REG_GO, 1);
}

void DRV2605L::stop()
{
  writeRegister8(DRV2605_REG_GO, 0);
}

void DRV2605L::setMode(uint8_t mode)
{
  writeRegister8(DRV2605_REG_MODE, mode);
}

void DRV2605L::setRealtimeValue(uint8_t rtp)
{
  writeRegister8(DRV2605_REG_RTPIN, rtp);
}

uint8_t DRV2605L::readRegister8(uint8_t reg)
{
  // use i2c
  Wire.beginTransmission(DRV2605_ADDR);
  Wire.write((byte)reg);
  Wire.endTransmission();
  Wire.requestFrom((byte)DRV2605_ADDR, (byte)1);
  return Wire.read();
}

void DRV2605L::writeRegister8(uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(DRV2605_ADDR);
  Wire.write((byte)reg);
  Wire.write((byte)val);
  Wire.endTransmission();
}

void DRV2605L::useERM()
{
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
}

void DRV2605L::useLRA()
{
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) | 0x80);
}

#endif