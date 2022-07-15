#include "VibroStudy.h"

#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <boost/filesystem.hpp>

using namespace std;

#ifndef WINDOWS
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/kd.h>

void beep(unsigned int frequency, unsigned int ms)
{
  int fd = open("/dev/console", O_WRONLY);
  ioctl(fd, KIOCSOUND, (int)(1193180 / frequency));
  usleep(1000 * ms);
  ioctl(fd, KIOCSOUND, 0);
  close(fd);
}

int kbhit()
{
  termios term;
  tcgetattr(0, &term);

  termios term2 = term;
  term2.c_lflag &= ~ICANON;
  tcsetattr(0, TCSANOW, &term2);

  int byteswaiting;
  ioctl(0, FIONREAD, &byteswaiting);

  tcsetattr(0, TCSANOW, &term);

  return byteswaiting > 0;
}


char getch()
{
  char buf = 0;
  struct termios old = { 0 };
  fflush(stdout);
  if (tcgetattr(0, &old) < 0)
    perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
    perror("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsetattr ~ICANON");
  printf("%c\n", buf);
  return buf;
}

#else
#include <Windows.h>
#include <conio.h>
void beep(unsigned int frequency, unsigned int ms)
{
  Beep(frequency, ms);
}
#endif

#include <chrono>
#include <thread>
void Sleep(unsigned int ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Returns ID, if a valid value is entered
int readValidID()
{
  cout << "ID of test-person: ";
  int id;
  cin >> id;
  while (cin.fail() || id < 0)
  {
    cin.clear();
    cin.ignore(256, '\n');
    cout << "ID must be a number >0\n";
    cout << "Id of test-person: ";
    cin >> id;
  }
  cin.clear();
  cin.ignore(256, '\n');
  return id;
}

// Returns Block-length, if a valid value is entered
int readValidBlockLength()
{
  cout << "\n#Stimuli [Max 100]: ";
  int block_length;
  cin >> block_length;
  while (cin.fail() || block_length > 100 || block_length < 1)
  {
    cin.clear();
    cin.ignore(256, '\n');
    cout << "Value must be a number =< 100 & >0\n";
    cout << "#Stimuli [Max 100]: ";
    cin >> block_length;
  }
  cin.clear();
  cin.ignore(256, '\n');
  return block_length;
}

// Returns the frequency, if a valid value is entered
double readValidFrequency()
{
  double frequency;
  cout << "\nFrequency [200-250 Hz]: ";
  cin >> frequency;
  while (cin.fail() || frequency < 200 || frequency > 250)
  {
    cin.clear();
    cin.ignore(265, '\n');
    cout << "Value must be a number >=200 & <=250\n";
    cout << "\nFrequency [200-250 Hz]: ";
    cin >> frequency;
  }
  cin.clear();
  cin.ignore(265, '\n');
  return frequency;
}

// Returns the duration of one stimulus, if a valid value is entered
double readValidStimulusDuration()
{
  double stimulus_duration;
  cout << "\nStimulus-duration [s]: ";
  cin >> stimulus_duration;
  while (cin.fail() || stimulus_duration <= 0)
  {
    cin.clear();
    cin.ignore(256, '\n');
    cout << "Stimulus-duration must be >0\n";
    cout << "Stimulus-duration [s]: ";
    cin >> stimulus_duration;
  }
  cin.clear();
  cin.ignore(256, '\n');
  return stimulus_duration;
}

// Returns the offtime for the multiple vibration patterns, if a valid value is entered
double readValidOfftimeMultiple(double duration)
{
  double offtime;
  cout << "\nStimulus-offtime [s]: ";
  cin >> offtime;
  while (cin.fail() || offtime >= duration || offtime < 0)
  {
    cin.clear();
    cin.ignore(256, '\n');
    cout << "Value must be < " << duration << "s (Stimulus-duration) & >0\n";
    cout << "Stimulus-offtime [s]: ";
    cin >> offtime;
  }
  cin.clear();
  cin.ignore(256, '\n');
  return offtime;
}

// Returns the cycle-duration, if a valid value is entered
double readValidCycleDuration(double duration)
{
  double cycle_duration;
  cout << "\nCycle-duration [s]: ";
  cin >> cycle_duration;
  while (cin.fail() || cycle_duration < duration || cycle_duration <= 0)
  {
    cin.clear();
    cin.ignore(256, '\n');
    cout << "Value must be >" << duration << "s (Stimulus-duration) & >0\n";
    cout << "Cycle-duration [s]: ";
    cin >> cycle_duration;
  }
  cin.clear();
  cin.ignore(256, '\n');
  return cycle_duration;
}

// Returns the duration of the varying vibrations of one stimulus, if a valid value is entered
double readValidVaryingDuration(double duration)
{
  double varyingDuration;
  cout << "\nTime for variation of frequency (for varying vibration patterns) [s]: ";
  cin >> varyingDuration;

  int varyingDuration_ms = std::round(varyingDuration * 1000);
  int duration_ms = std::round(duration * 1000);
  while (cin.fail() || varyingDuration <= 0 || (duration_ms % varyingDuration_ms) != 0 || duration_ms / varyingDuration_ms < 2)
  {
    cin.clear();
    cin.ignore(256, '\n');
    cout << "Value must be a number >0 and a multiple of " << duration << "\n";
    cout << "Time for variation of frequency (for varying vibration patterns) [s]: ";
    cin >> varyingDuration;
    varyingDuration_ms = std::round(varyingDuration * 1000);
  }
  cin.clear();
  cin.ignore(256, '\n');
  return varyingDuration;
}

// Returns the offtime of the varying vibration mode, if a valid value is entered
double readValidOfftimeVarying(double duration)
{
  double offtimeVarying;
  cout << "\nOfftime between vibrations (for varying vibration patterns) [s]: ";
  cin >> offtimeVarying;
  while (cin.fail() || offtimeVarying <= 0 || offtimeVarying > duration)
  {
    cin.clear();
    cin.ignore(256, '\n');
    cout << "Value must be a number <" << duration << "(frequency variation time) & >0\n";
    cout << "Offtime between vibrations (for varying vibration patterns) [s]: ";
    cin >> offtimeVarying;
  }
  cin.clear();
  cin.ignore(256, '\n');
  return offtimeVarying;
}

VibroStudy::VibroStudy()
  : VibroControl()
{}

VibroStudy::~VibroStudy()
{}

double VibroStudy::convert_frequency(double freq)
{
  double converted_frequency;
  if (freq < 225)
  {
    converted_frequency = (freq + 170) / 500;
  }
  else
  {
    converted_frequency = (freq - 125) / 125;
  }
  return converted_frequency;

}

void VibroStudy::read_parameters()
{
  cout << "Enter the parameters for the test-blocks\n";
  cout << "----------------------------------------\n";
  para.id = readValidID();
  para.block_length = readValidBlockLength();
  para.frequency = readValidFrequency();
  para.stimu_duration = readValidStimulusDuration();
  para.offtime_multiple = readValidOfftimeMultiple(para.stimu_duration);
  para.cycle_duration = readValidCycleDuration(para.stimu_duration);
  para.varying_duration = readValidVaryingDuration(para.stimu_duration);
  para.offtime_varying = readValidOfftimeVarying(para.varying_duration);
  cout << "----------------------------------------\n";

  upload_pattern();
  upload_varying_patterns();
  generate_blocklist();
}

void VibroStudy::change_parameters()
{
  cout << "Change parameter(s) (current value)\n";
  cout << "----------------------------------------\n";
  cout << "1: All\n";
  cout << "2: ID (" << para.id << ")\n";
  cout << "3: #Stimuli (" << para.block_length << ")\n";
  cout << "4: Frequency (" << para.frequency << "Hz)\n";
  cout << "5: Stimulus-duration (" << para.stimu_duration << "s) -> change via All\n";
  cout << "6: Stimulus-offtime (" << para.offtime_multiple << "s)\n";
  cout << "7: Cycle-duration (" << para.cycle_duration << "s)\n";
  cout << "8: Frequence-time for vibrations with varying frequency (" << para.varying_duration << "s)\n";
  cout << "9: Offtime between vibrations with varying frequency (" << para.offtime_varying << "s)\n";
  cout << "0: Cancel\n";
  cout << "----------------------------------------\n";

  bool repeat;

  do
  {
    repeat = false;

    char input;
    cin >> input;

    switch (input)
    {
    case '1': cin.clear(); cin.ignore(256, '\n'); read_parameters();																					break;
    case '2': cin.clear(); cin.ignore(256, '\n'); para.id = readValidID();																				break;
    case '3': cin.clear(); cin.ignore(256, '\n'); para.block_length = readValidBlockLength();								generate_blocklist();		break;
    case '4': cin.clear(); cin.ignore(256, '\n'); para.frequency = readValidFrequency();									upload_pattern();			break;
      //case '5': cin.clear(); cin.ignore(256, '\n'); para.stimu_duration = readValidStimulusDuration();						upload_pattern();			break;
    case '6': cin.clear(); cin.ignore(256, '\n'); para.offtime_multiple = readValidOfftimeMultiple(para.stimu_duration);	upload_pattern();			break;
    case '7': cin.clear(); cin.ignore(256, '\n'); para.cycle_duration = readValidCycleDuration(para.stimu_duration);									break;
    case '8': cin.clear(); cin.ignore(256, '\n'); para.varying_duration = readValidVaryingDuration(para.stimu_duration);	upload_varying_patterns(); upload_pattern();	break;
    case '9': cin.clear(); cin.ignore(256, '\n'); para.offtime_varying = readValidOfftimeVarying(para.varying_duration);	upload_varying_patterns(); upload_pattern();	break;
    case '0': cin.clear(); cin.ignore(256, '\n'); cout << "Aborted...\n";  break;
    default: cin.clear(); cin.ignore(256, '\n'); cout << "Invalid Choice!\n"; repeat = true;
    }

  } while (repeat);


}

void VibroStudy::vibration_commands()
{
  cout << "1: Motor 0, Frequency " << para.frequency << ", Duration " << para.stimu_duration << "\n";
  cout << "2: Motor 1, Frequency " << para.frequency << ", Duration " << para.stimu_duration << "\n";
  cout << "3: Motor 0, Frequency " << para.frequency << ", Pattern {On: " << (para.stimu_duration - para.offtime_multiple) / 2 << ", Off: " << para.offtime_multiple << ", On: " << (para.stimu_duration - para.offtime_multiple) / 2 << "}\n";
  cout << "4: Motor 0, Varying frequencies (200 - 225 - 250 Hz), Random order\n";
  cout << "5: Motor 0, Varying frequencies (200 - 225 - 250 Hz), Systematic order\n";
  cout << "6: Motor 0, Varying frequencies (200 - 225 - 250 Hz), Random order with offtime\n";
  cout << "7: Motor 0, Varying frequencies (200 - 225 - 250 Hz), Systematic order, with offtime\n";
  cout << "8: Motor 0, Multiple-Vibration\n";
}

void VibroStudy::upload_pattern()
{
  cout << "Uploading pattern...\n";

  // Pattern for double-impulse
  double len = (para.stimu_duration - para.offtime_multiple) / 2;
  double freq = convert_frequency(para.frequency);

  upload(
    {
      {freq, len},
      {0, para.offtime_multiple},
      {freq, len}
    }, 0
  );

  // Pattern for multiple vibrations with offtime, but without variation of frequency
  int stimu_count = para.stimu_duration / para.varying_duration;
  VibrationPattern vec(2 * stimu_count - 1);

  // First vibration & offtime
  vec[0] = { freq, para.varying_duration - para.offtime_varying / 2 };
  vec[1] = { 0, para.offtime_varying };

  // Second Vibration & offtime to last but one
  for (int i = 2; i < 2 * stimu_count - 2; i += 2)
  {
    vec[i] = { freq, para.varying_duration - para.offtime_varying };
    vec[i + 1] = { 0, para.offtime_varying };
  }

  // Last vibration
  vec[2 * stimu_count - 2] = { freq , para.varying_duration - para.offtime_varying / 2 };

  upload(vec, 5);
  for (int i = 0; i < vec.size(); ++i)
    std::cout << "{" << vec[i].first << ", " << vec[i].second << "} ";
  std::cout << endl;
  cout << "Pattern 5 uploaded.\n";
}

void VibroStudy::varpatt_ran()
{
  int stimu_count = para.stimu_duration / para.varying_duration;
  double strength[3] = { 0.74, 0.8, 1 };
  int count = 0;

  VibrationPattern vec(stimu_count);

  for (int i = 0; i < stimu_count; i++)
  {
    if (count == 0)
    {
      vec[i] = { strength[count] , para.varying_duration };
      count = (rand() % 2) + 1;// cout << "0";
    }
    else
    {
      if (count == 1)
      {
        vec[i] = { strength[count] , para.varying_duration };
        count = (rand() % 2) * 2;// cout << "1";
      }
      else
      {
        if (count == 2)
        {
          vec[i] = { strength[count] , para.varying_duration };
          count = rand() % 2;//	cout << "2";
        }
      }
    }
  }

  upload(vec, 1);
  for (int i = 0; i < vec.size(); ++i)
    std::cout << "{" << vec[i].first << ", " << vec[i].second << "} ";
  std::cout << endl;
  cout << "Pattern 1 var_ran uploaded\n";

}

void VibroStudy::varpatt_sys()
{
  int stimu_count = para.stimu_duration / para.varying_duration;
  double strength[3] = { 0.74, 0.8, 1 };
  int count = 0;
  bool up = true;

  VibrationPattern vec(stimu_count);

  for (int i = 0; i < stimu_count; i++)
  {
    vec[i] = { strength[count] , para.varying_duration };

    if (count < 2 && up)
    {
      count++;
    }
    else
    {
      if (count == 2 && up)
      {
        count--;
        up = false;
      }
      else
      {
        if (count == 0 && !up)
        {
          count++;
          up = true;
        }
        else
        {
          count--;
          up = true;
        }
      }
    }
  }

  upload(vec, 2);
  for (int i = 0; i < vec.size(); ++i)
    std::cout << "{" << vec[i].first << ", " << vec[i].second << "} ";
  std::cout << endl;
  cout << "Pattern 2 var_sys uploaded\n";
}

void VibroStudy::varpatt_ran_off()
{
  int stimu_count = para.stimu_duration / para.varying_duration;
  double strength[3] = { 0.74, 0.8, 1 };
  int count = 0;

  VibrationPattern vec(2 * stimu_count - 1);

  // First vibration & offtime
  vec[0] = { strength[0], para.varying_duration - para.offtime_varying / 2 };
  vec[1] = { 0, para.offtime_varying };

  // Vibrations & offtimes from 2nd to last but one
  for (int i = 2; i < 2 * stimu_count - 2; i += 2)
  {
    if (count == 0)
    {
      count = (rand() % 2) + 1;
      vec[i] = { strength[count] , para.varying_duration - para.offtime_varying };
    }
    else
    {
      if (count == 1)
      {
        count = (rand() % 2) * 2;
        vec[i] = { strength[count] , para.varying_duration - para.offtime_varying };
      }
      else
      {
        if (count == 2)
        {
          count = rand() % 2;
          vec[i] = { strength[count] , para.varying_duration - para.offtime_varying };
        }
      }
    }
    vec[i + 1] = { 0 , para.offtime_varying };

  }

  // Last vibration
  if (count == 0)
  {
    count = (rand() % 2) + 1;
    vec[2 * stimu_count - 2] = { strength[count] , para.varying_duration - para.offtime_varying / 2 };
  }
  else
  {
    if (count == 1)
    {
      count = (rand() % 2) * 2;
      vec[2 * stimu_count - 2] = { strength[count] , para.varying_duration - para.offtime_varying / 2 };
    }
    else
    {
      if (count == 2)
      {
        count = rand() % 2;
        vec[2 * stimu_count - 2] = { strength[count] , para.varying_duration - para.offtime_varying / 2 };
      }
    }
  }

  upload(vec, 3);
  for (int i = 0; i < vec.size(); ++i)
    std::cout << "{" << vec[i].first << ", " << vec[i].second << "} ";
  std::cout << endl;
  cout << "Pattern 3 var_ran_off uploaded\n";

}

void VibroStudy::varpatt_sys_off()
{
  int stimu_count = para.stimu_duration / para.varying_duration;
  double strength[3] = { 0.74, 0.8, 1 };
  int count = 0;
  bool up = true;

  VibrationPattern vec(2 * stimu_count - 1);

  // First vibration & offtime
  vec[0] = { strength[0], para.varying_duration - para.offtime_varying / 2 };
  vec[1] = { 0, para.offtime_varying };

  // Vibrations & offtimes from 2nd to last but one
  for (int i = 2; i < 2 * stimu_count - 2; i += 2)
  {

    if (count < 2 && up)
    {
      count++;
      vec[i] = { strength[count] , para.varying_duration - para.offtime_varying };
    }
    else
    {
      if (count == 2 && up)
      {
        count--;
        up = false;
        vec[i] = { strength[count] , para.varying_duration - para.offtime_varying };
      }
      else
      {
        if (count == 0 && !up)
        {
          count++;
          up = true;
          vec[i] = { strength[count] , para.varying_duration - para.offtime_varying };
        }
        else
        {
          count--;
          up = true;
          vec[i] = { strength[count] , para.varying_duration - para.offtime_varying };
        }
      }
    }
    vec[i + 1] = { 0, para.offtime_varying };
  }

  // Last vibration
  if (count < 2 && up)
  {
    count++;
    vec[2 * stimu_count - 2] = { strength[count] , para.varying_duration - para.offtime_varying / 2 };
  }
  else
  {
    if (count == 2 && up)
    {
      count--;
      up = false;
      vec[2 * stimu_count - 2] = { strength[count] , para.varying_duration - para.offtime_varying / 2 };
    }
    else
    {
      if (count == 0 && !up)
      {
        count++;
        up = true;
        vec[2 * stimu_count - 2] = { strength[count] , para.varying_duration - para.offtime_varying / 2 };
      }
      else
      {
        count--;
        up = true;
        vec[2 * stimu_count - 2] = { strength[count] , para.varying_duration - para.offtime_varying / 2 };
      }
    }
  }

  upload(vec, 4);
  for (int i = 0; i < vec.size(); ++i)
    std::cout << "{" << vec[i].first << ", " << vec[i].second << "} ";
  std::cout << endl;
  cout << "Pattern 4 var_sys_off uploaded\n";

}

void VibroStudy::upload_varying_patterns()
{
  cout << "Uploading patterns...\n";

  varpatt_ran();
  varpatt_ran_off();
  varpatt_sys();
  varpatt_sys_off();

  cout << "All patterns uploaded.\n";
}

void VibroStudy::generate_blocklist()
{
  cout << "Generate blocklist...\n";
  for (int condition = 0; condition < 2; condition++) {
    for (int style = 0; style < 4; style++) {
      blocklist[condition + 2 * style] = generate_block(condition, style);
    }
  }
  cout << "Blocklist generated.\n";
};

VibroStudy::block VibroStudy::generate_block(int cond, int st)
{

  block b;

  for (int i = 0; i < para.block_length; i++)
  {
    b.motor[i] = rand() % 2;
  }

  b.condition = cond;
  b.style = st;

  return b;

}

void VibroStudy::display_block(block b)
{
  cout << "\nBlock: " << (b.condition + 2 * b.style) + 1 << "\n";
  cout << "----------------------------------------\n";
  cout << "Condition: ";
  if (b.condition == 0)
  {
    cout << "Standing\n";
  }
  else {
    cout << "Movement\n";
  }

  cout << "Style: ";
  if (b.style == 0)	cout << "Continous\n";
  if (b.style == 1)	cout << "Double\n";
  if (b.style == 2)	cout << "Varying\n";
  if (b.style == 3)	cout << "Multiple\n";


  cout << "Motor: ";
  for (int i = 0; i < para.block_length; i++)
  {
    cout << b.motor[i] << " ";
  }
  cout << "\n";
  cout << "----------------------------------------\n";
}

void VibroStudy::display_blocks()
{
  cout << "Current blocklist:\n";

  for (int i = 0; i < 8; i++)
  {
    display_block(blocklist[i]);
  }

  cout << "\nID: " << para.id << "\n";
  cout << "Frequency: " << para.frequency << "Hz\n";
  cout << "Stimulus-duration: " << para.stimu_duration << "s\n";
  cout << "Stimulus-offtime: " << para.offtime_multiple << "s\n";
  cout << "Duration of single impulses in varying frequency: " << para.varying_duration << "s\n";
  cout << "Offtime between varying vibration: " << para.offtime_varying << "s\n";
  cout << "Cycle-duration: " << para.cycle_duration << "s\n";
  cout << "#Stimuli: " << para.block_length << "\n";
  cout << "----------------------------------------\n";
}

void VibroStudy::choose_block()
{
  cout << "Choose which block to be played\n";
  cout << "----------------------------------------\n";
  cout << "1: Standing & Continuous\n";
  cout << "2: Movement & Continuous\n";
  cout << "3: Standing & Double\n";
  cout << "4: Movement & Double\n";
  cout << "5: Standing & Varying\n";
  cout << "6: Movement & Varying\n";
  cout << "7: Standing & Multiple\n";
  cout << "8: Movement & Multiple\n";
  cout << "0: Cancel\n";

  bool repeat;
  do
  {
    repeat = false;
    cout << "Choice: ";
    char input;
    cin >> input;

    int c;

    switch (input)
    {
    case '1': cin.clear(); cin.ignore(256, '\n'); display_block(blocklist[0]);	c = 0; play_block(c);			break;
    case '2': cin.clear(); cin.ignore(256, '\n'); display_block(blocklist[1]);	c = 1; play_block(c);			break;
    case '3': cin.clear(); cin.ignore(256, '\n'); display_block(blocklist[2]);	c = 2; play_block(c);			break;
    case '4': cin.clear(); cin.ignore(256, '\n'); display_block(blocklist[3]);	c = 3; play_block(c);			break;
    case '5': cin.clear(); cin.ignore(256, '\n'); display_block(blocklist[4]);  c = 4; choose_varying_block(c); break;
    case '6': cin.clear(); cin.ignore(256, '\n'); display_block(blocklist[5]);	c = 5; choose_varying_block(c); break;
    case '7': cin.clear(); cin.ignore(256, '\n'); display_block(blocklist[6]);	c = 6; play_block(c);			break;
    case '8': cin.clear(); cin.ignore(256, '\n'); display_block(blocklist[7]);	c = 7; play_block(c);			break;
    case '0': cin.clear(); cin.ignore(256, '\n'); cout << "Aborted...\n"; break;
    default:  cin.clear(); cin.ignore(256, '\n'); cout << "Invalid choice!\n"; repeat = true;
    }
  } while (repeat);
}

void VibroStudy::play_block(int choice)
{
  cout << "----------------------------------------\n";
  int block = choice;

  cout << "1: Start block\n";
  cout << "0: Cancel\n";

  int i = 0;
  bool exit = false;
  bool repeat;

  do {

    repeat = false;
    cout << "Choice: ";

    char input;
    cin >> input;

    switch (input)
    {
    case '1':
      cin.clear(); cin.ignore(256, '\n');

      cout << "Reset log device-time...\n";
      reset();

      beep(440, 500);
      //beep(440, 500);
      //beep(440, 500);

      cout << "Playing block...(Press c to cancel)\n";
      Sleep(static_cast<unsigned int>(500));

      if (blocklist[block].style == 0)
      {
        while (!exit && i < para.block_length)
        {

          if (rand() % 2 == 1)
          {
            set(blocklist[block].motor[i], convert_frequency(para.frequency), para.stimu_duration);
            i++;
            cout << i << "\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }
          else
          {
            cout << "sleep...\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }

          if (kbhit())
          {
            char c = getch();
            switch (c)
            {
            case 'c': exit = true;	break;
            }
          }
        }
      }
      if (blocklist[block].style == 1)
      {
        while (!exit && i < para.block_length)
        {

          if (rand() % 2 == 1)
          {
            play(blocklist[block].motor[i], 0);
            i++;
            cout << i << "\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }
          else
          {
            cout << "sleep...\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }

          if (kbhit())
          {
            char c = getch();
            switch (c)
            {
            case 'c': exit = true;	break;
            }
          }
        }
      }

      if (blocklist[block].style == 3)
      {
        while (!exit && i < para.block_length)
        {

          if (rand() % 2 == 1)
          {
            play(blocklist[block].motor[i], 5);
            i++;
            cout << i << "\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }
          else
          {
            cout << "sleep...\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }

          if (kbhit())
          {
            char c = getch();
            switch (c)
            {
            case 'c': exit = true;	break;
            }
          }
        }
      }

      if (exit == true)
      {
        cout << "Playing cancelled.\n";
      }
      else
      {
        cout << "Block finished\n";
        beep(440, 500);
        //beep(440, 500);
        //beep(440, 1500);
        lbp.condition = blocklist[block].condition;
        lbp.style = blocklist[block].style;
        save_log();
      }

      break;

    case '0': cin.clear(); cin.ignore(256, '\n'); cout << "Aborted1...\n"; break;
    default: cout << "Invalid choice!\n"; repeat = true; cin.clear(); cin.ignore(256, '\n');

    }
  } while (repeat);
}

void VibroStudy::choose_varying_block(int choice)
{
  cout << "----------------------------------------\n";
  int with_offtime;
  int play_order;

  cout << "Play the block with offtime between the modulation of frequency?\n";
  cout << "1: Yes, with offtime\n";
  cout << "2: No, without offtime\n";
  cout << "0: Cancel\n";


  bool repeat;

  do {
    cout << "Choice: ";
    repeat = false;

    char offt;
    cin >> offt;

    switch (offt)
    {
    case '1':
      cin.clear(); cin.ignore(256, '\n');
      with_offtime = 1;
      cout << "----------------------------------------\n";
      cout << "Vary between the different frequencies in random or systematic order?\n";
      cout << "1: Random order\n";
      cout << "2: Systematic order\n";
      cout << "0: Cancel\n";

      do {
        repeat = false;

        cout << "Choice: ";


        char order1;
        cin >> order1;

        switch (order1)
        {
        case '1': play_order = 0; play_varying_block(choice, with_offtime, play_order); break;
        case '2': play_order = 1; play_varying_block(choice, with_offtime, play_order); break;
        case '0': cin.clear(); cin.ignore(256, '\n'); cout << "Aborted...\n"; break;
        default: cin.clear(); cin.ignore(256, '\n'); cout << "Invalid choice!\n"; repeat = true;
        }

      } while (repeat);
      break;

    case '2':

      with_offtime = 0;
      cout << "----------------------------------------\n";
      cout << "Vary between the different frequencies in random or systematic order?\n";
      cout << "1: Random order\n";
      cout << "2: Systematic order\n";
      cout << "0: Cancel\n";

      do {
        repeat = false;

        cout << "Choice: ";


        char order2;
        cin >> order2;

        switch (order2)
        {
        case '1': play_order = 0; play_varying_block(choice, with_offtime, play_order); break;
        case '2': play_order = 1; play_varying_block(choice, with_offtime, play_order); break;
        case '0': cin.clear(); cin.ignore(256, '\n'); cout << "Aborted...\n"; break;
        default: cin.clear(); cin.ignore(256, '\n'); cout << "Invalid choice!\n"; repeat = true;
        }

      } while (repeat);
      break;

    case '0': cin.clear(); cin.ignore(256, '\n'); cout << "Aborted...\n"; break;
    default: cin.clear(); cin.ignore(256, '\n'); cout << "Invalid choice!\n"; repeat = true;

    }
  } while (repeat);

}

void VibroStudy::play_varying_block(int choice, int with_offtime, int play_order)
{
  cout << "----------------------------------------\n";
  cout << "1: Start block\n";
  cout << "0: Cancel\n";

  int i = 0;
  bool exit = false;
  bool repeat;

  do {
    cin.clear();
    cin.ignore(256, '\n');

    repeat = false;
    cout << "Choice: ";

    char input;
    cin >> input;

    switch (input)
    {
    case '1':

      cin.clear(); cin.ignore(256, '\n');

      cout << "Reset log device-time...\n";
      reset();

      beep(440, 500);
      //beep(440, 500);
      //beep(440, 500);
      cout << "Playing block...(Press c to cancel)\n";
      Sleep(static_cast<unsigned int>(500));

      // Without offtime, random order
      if (with_offtime == 0 && play_order == 0)
      {

        while (!exit && i < para.block_length)
        {
          if (rand() % 2 == 1)
          {
            play(blocklist[choice].motor[i], 1);
            i++;
            cout << i << "\n";
            Sleep(static_cast<unsigned int>(std::round(para.stimu_duration * 1000)));
            varpatt_ran();
            Sleep(static_cast<unsigned int>(std::round((para.cycle_duration - para.stimu_duration) * 1000)));
          }
          else
          {
            cout << "sleep...\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }

          if (kbhit())
          {
            char c = getch();
            switch (c)
            {
            case 'c': exit = true;	break;
            }
          }
        }
      }

      // Without offtime, systematic order
      if (with_offtime == 0 && play_order == 1)
      {

        while (!exit && i < para.block_length)
        {
          if (rand() % 2 == 1)
          {
            play(blocklist[choice].motor[i], 2);
            i++;
            cout << i << "\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }
          else
          {
            cout << "sleep...\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }

          if (kbhit())
          {
            char c = getch();
            switch (c)
            {
            case 'c': exit = true;	break;
            }
          }
        }
      }

      // With offtime, random order
      if (with_offtime == 1 && play_order == 0)
      {
        while (!exit && i < para.block_length)
        {
          if (rand() % 2 == 1)
          {
            play(blocklist[choice].motor[i], 3);
            i++;
            cout << i << "\n";
            Sleep(static_cast<unsigned int>(std::round(para.stimu_duration * 1000)));
            varpatt_ran_off();
            Sleep(static_cast<unsigned int>(std::round((para.cycle_duration - para.stimu_duration) * 1000)));
          }
          else
          {
            cout << "sleep...\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }

          if (kbhit())
          {
            char c = getch();
            switch (c)
            {
            case 'c': exit = true;	break;
            }
          }
        }
      }

      // With offtime, systematic order
      if (with_offtime == 1 && play_order == 1)
      {
        while (!exit && i < para.block_length)
        {
          if (rand() % 2 == 1)
          {
            play(blocklist[choice].motor[i], 4);
            i++;
            cout << i << "\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }
          else
          {
            cout << "sleep...\n";
            Sleep(static_cast<unsigned int>(std::round(para.cycle_duration * 1000)));
          }

          if (kbhit())
          {
            char c = getch();
            switch (c)
            {
            case 'c': exit = true;	break;
            }
          }
        }
      }

      if (exit == true)
      {
        cout << "Playing cancelled.\n";
      }
      else
      {
        cout << "Block finished\n";
        beep(440, 1000);
        //beep(440, 500);
        //beep(440, 1500);
        lbp.condition = blocklist[choice].condition;
        lbp.style = blocklist[choice].style;
        lbp.order = play_order;
        lbp.with_offtime = with_offtime;
        save_log();
      }
      break;

    case '0': cin.clear(); cin.ignore(256, '\n'); cout << "Aborted...\n"; break;
    default: cout << "Invalid choice!\n"; repeat = true;
    }

  } while (repeat);
}

void VibroStudy::save_log()
{
  cout << "Saving log...\n";

  string sc;

  if (lbp.condition == 0)
  {
    sc += "Standing";
  }
  else
  {
    sc += "Moving";
  }

  if (lbp.style == 0) sc += " - Continous";
  if (lbp.style == 1) sc += " - Double";
  if (lbp.style == 3) sc += " - Multiple";

  if (lbp.style == 2)
  {
    sc += " - Varying";
    if (lbp.order == 0)
    {
      sc += " - Random";
    }
    else
    {
      sc += " - Systematic";
    }
    if (lbp.with_offtime == 0)
    {
      sc += " - Without offtime";
    }
    else
    {
      sc += " - With offtime";
    }
  }


  string first_line = "Frequency: " + std::to_string(para.frequency) +
    "\nStimulus-duration: " + std::to_string(para.stimu_duration) +
    "\nOfftime between stimuli (multiple): " + std::to_string(para.offtime_multiple) +
    "\nTime of Movement-cycle: " + std::to_string(para.cycle_duration) +
    "\nDuration of varying frequencies: " + std::to_string(para.varying_duration) +
    "\nOfftime between varying frequency vibrations: " + std::to_string(para.offtime_varying);


  string log_device = requestLog();
  string log = first_line + "\n\n" + log_device;

  bool directory_exists = false;

  do {

    string path = "./VibroLogs/" + std::to_string(para.id) + "/";
    string filename = std::to_string(para.id) + " - " + sc + " - VibroTest.log";

    if (!boost::filesystem::exists(path))
    {
      boost::filesystem::create_directories(path);
      if (!boost::filesystem::exists(path))
      {
        cout << "Unable to create the directory... no permissions?\n";
      }
      std::ofstream logfile(path + filename, std::ios_base::out);
      logfile << log << std::endl;
      directory_exists = true;
      cout << "Log saved to new directory with ID " << para.id << "\n";
    }
    else
    {
      string complete_path = path + filename;
      if (boost::filesystem::exists(complete_path))
      {
        cout << "Directory & file already exist!\n";
        cout << "1: Overwrite\n";
        cout << "2: Change ID\n";

        bool repeat;
        do
        {
          repeat = false;
          cout << "Choice: ";
          char input;
          cin >> input;

          switch (input)
          {
          case '1':
          {
            std::ofstream logfile(path + filename, std::ios_base::out);
            logfile << log << std::endl;
            directory_exists = true;
            cin.clear(); cin.ignore(256, '\n');
            cout << "File saved.\n";
            break;
          }
          case '2':
          {
            para.id = readValidID();
            cin.clear(); cin.ignore(256, '\n');
            break;
          }
          default: cout << "Invalid choice!\n"; repeat = true; cin.clear(); cin.ignore(256, '\n'); break;
          }
        } while (repeat);

      }
      else
      {
        cout << "Directory already exists! (file not)\n";
        cout << "1: Save\n";
        cout << "2: Change ID\n";

        bool repeat;
        do
        {
          repeat = false;
          cout << "Choice: ";
          char input;
          cin >> input;

          switch (input)
          {
          case '1':
          {
            std::ofstream logfile(path + filename, std::ios_base::out);
            logfile << log << std::endl;
            directory_exists = true;
            cin.clear(); cin.ignore(256, '\n');
            cout << "File saved.\n";
            break;
          }
          case '2':
          {
            para.id = readValidID();
            cin.clear(); cin.ignore(256, '\n');
            break;
          }
          default: cout << "Invalid choice!\n"; repeat = true; cin.clear(); cin.ignore(256, '\n'); break;
          }
        } while (repeat);
      }

    }

  } while (!directory_exists);
}