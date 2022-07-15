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

/*
 ******************************************************* 
 * Config and event handling
 *******************************************************
 */
 //#define DEBUG

 // Helpers
 #include "Tools.h"

 // Logging
 #include "Logging.h"

// Buttons
#include "Button.h"
Button button0(26, [](const ButtonEvent& event)
{
  if (event.type == ButtonEvent::Type::PRESSED)
  {
    Logging::log(event.timeStamp, "B1");

	#ifdef DEBUG
	Serial.println("Button 0 pressed.");
	#endif
  }
});

Button button1(27, [](const ButtonEvent& event)
{
  if (event.type == ButtonEvent::Type::PRESSED)
  {
    Logging::log(event.timeStamp, "B2");

	#ifdef DEBUG
	Serial.println("Button 1 pressed.");
	#endif
  }
});

// Motors
#include "Motor.h"
Motor motors[2] = { Motor(0, 15), Motor(1, 14) };

 // WiFiManager
#include "WiFiManager.h"
const static WiFiConfig wifiCfg = // WiFi config
{
  WiFiConfig::WiFiMode::AP,       // Mode: access point or station client
  String("VibroControl"),         // SSID
  String("wearhealth"),           // Passphrase
  1,                              // WiFi channel, if access point mode
  IPAddress(192, 168, 4, 1),      // IP
  IPAddress(192, 168, 4, 254),    // Gateway
  IPAddress(255, 255, 255, 0)     // Subnet
};

// TCPListener and command hadling
#include "TCPListener.h"
std::vector<VibrationPattern> uploadedPatterns(255);

TCPListener tcpListener(4545, [&](const TCPMessage& tcpMessage)
{
  const unsigned int cmdLen = tcpMessage.size;
  uint8_t* cmd = tcpMessage.data;

  if (cmdLen > 0)
  {
    switch(cmd[0])
    {
      case 'a':
	  {
	    // Alive request
        tcpListener.send({ 1, 1, 255 });
		
		#ifdef DEBUG
		Serial.println("Alive request.");
		#endif
	    break;
	  }
	  case 'v':
	  {
	    // Vibrate command
	    if (cmdLen < 3) { break; }
	    const uint8_t motorId = cmd[1];
	    if (!(0 <= motorId && motorId < 2)) { break; } 
	    const uint8_t patternSize = cmd[2];
		const VibrationPattern pattern = { patternSize, &cmd[3] }; //extractVibrationPattern(&cmd[3], patternSize);
	    motors[motorId].vibrate(pattern);

		if (patternSize == 1)
		{
		  Logging::log(Tools::getMillis(), "M" + String(motorId) + ":" + (String)cmd[3]);
		}

		#ifdef DEBUG
		Serial.println("Vibrate command.");
		#endif
	    break;
	  }
	  case 'u':
	  {    
	    // Upload pattern
	    if (cmdLen < 3) { break; }
	    const uint8_t patternId = cmd[1];
	    const uint8_t patternSize = cmd[2];
		uploadedPatterns[patternId].size = patternSize;
		if (uploadedPatterns[patternId].data != nullptr)
		{
		  delete[] uploadedPatterns[patternId].data;
		}
		uploadedPatterns[patternId].data = new uint8_t[2 * patternSize];
		memcpy(uploadedPatterns[patternId].data, &cmd[3], 2 * patternSize);
		
		#ifdef DEBUG
		Serial.println("Upload command.");
		#endif
	    break;		
	  }
	  case 'p':
	  {    
	    // Play uploaded plattern
	    if (cmdLen < 3) { break; }
	    const uint8_t motorId = cmd[1];
	    if (!(0 <= motorId && motorId < 2)) { break; } 
	    const uint8_t patternId = cmd[2];
	    motors[motorId].vibrate(uploadedPatterns[patternId]);

		Logging::log(Tools::getMillis(), "M" + String(motorId) + ":P" + String(patternId));

		#ifdef DEBUG
		Serial.println("Play command.");
		#endif
	    break;		
	  }
	  case 'r':
	  { 
	    // Reset log and time
	    Logging::init(10000);
		
		#ifdef DEBUG
		Serial.println("Time and log reset command.");
		#endif
	    break;
	  }
	  case 'l':
	  { 
	    // Request the log		
		const unsigned int logSize = Logging::logSize();
		uint8_t logSizeByteWise[4];
		memcpy(&logSizeByteWise, reinterpret_cast<const uint8_t*>(&logSize), 4);
		tcpListener.send({ 4, logSizeByteWise[0], logSizeByteWise[1], logSizeByteWise[2], logSizeByteWise[3] });
		const unsigned int maxChunkSize = 8000; // 8kB max. chunk size
		uint8_t* sendBuf = new uint8_t[maxChunkSize];
		unsigned int sentBytes = 0;
		while (sentBytes < logSize)
		{
		  const unsigned int chunkSize = std::min(maxChunkSize, logSize - sentBytes);
		  Logging::readBytes(sendBuf, sentBytes, chunkSize);
		  tcpListener.send(sendBuf, chunkSize);
		  sentBytes += chunkSize;
		}
		delete[] sendBuf;
	    
		#ifdef DEBUG
		Serial.println("Log request.");
		#endif
	    break;
	  }
	  default:
	  break;
    }
  }

  if (tcpMessage.data != nullptr)
  {
    delete[] tcpMessage.data;
  }
});

/*
 ******************************************************* 
 * Main routines
 *******************************************************
 */ 

void setup()
{
  Logging::init(10000); // 10kB

  #ifdef DEBUG
  Serial.println("VibroControl µC ready.");
  #endif

  for (unsigned int i = 0; i < uploadedPatterns.size(); ++i)
  {
    uploadedPatterns[i] = VibrationPattern({ 0, nullptr });
  }

  WiFiManager::init(wifiCfg, []() 
  {
    tcpListener.start();
	#ifdef DEBUG
    Serial.println("TCP listener waiting for commands...");
    #endif
  });
}

// Main loop
void loop()
{
}
