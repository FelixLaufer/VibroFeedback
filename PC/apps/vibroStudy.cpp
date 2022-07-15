#include "VibroControl/VibroStudy.h"

#include <fstream>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <iostream>
using namespace std;

// Displays legend of possible inputs
void legend(VibroControl & vibroControl) {
  cout << "\n";
  cout << "a: Alive request\n";
  cout << "c: Change parameters\n";
  cout << "g: Generates new blocks\n";
  cout << "p: Play block\n";
  cout << "d: Displays current blocks\n";
  cout << "r: Resets device-time\n";
  cout << "v: Displays different manual vibration options\n";
  cout << "q: exit\n";
  cout << "\n";
};

int main(int argc, char** argv)
{
  VibroStudy vibroControl;

  srand(time(NULL));

  vibroControl.read_parameters();

  //vibroControl.upload_varying_patterns();

  legend(vibroControl);
  bool exit = false;
  while (!exit)
  {
    int i = 0;
    bool repeat;

    do {

      repeat = false;
      cout << "Choice: ";

      char input;
      cin >> input;

      switch (input) {

      case 'a': cin.clear(); cin.ignore(256, '\n'); if (vibroControl.isAlive()) { cout << "Alive\n"; }
                else { cout << "Dead\n"; }  			legend(vibroControl); break;
      case 'c': cin.clear(); cin.ignore(256, '\n'); vibroControl.change_parameters(); 														legend(vibroControl); break;
      case 'g': cin.clear(); cin.ignore(256, '\n'); vibroControl.generate_blocklist();		 												legend(vibroControl); break;
      case 'p': cin.clear(); cin.ignore(256, '\n'); vibroControl.choose_block();																legend(vibroControl); break;
      case 'd': cin.clear(); cin.ignore(256, '\n'); vibroControl.display_blocks();															legend(vibroControl); break;
      case 'r': cin.clear(); cin.ignore(256, '\n'); vibroControl.reset();																        legend(vibroControl); break;
      case 'v': cin.clear(); cin.ignore(256, '\n'); vibroControl.vibration_commands();														legend(vibroControl); break;
      case 'q': exit = true; break;

      case '1': cin.clear(); cin.ignore(256, '\n'); vibroControl.set(0, vibroControl.convert_frequency(vibroControl.para.frequency), vibroControl.para.stimu_duration);	break;
      case '2': cin.clear(); cin.ignore(256, '\n'); vibroControl.set(1, vibroControl.convert_frequency(vibroControl.para.frequency), vibroControl.para.stimu_duration);	break;
      case '3': cin.clear(); cin.ignore(256, '\n'); vibroControl.play(0, 0);																								break;
      case '4': cin.clear(); cin.ignore(256, '\n'); vibroControl.play(0, 1);																								break;
      case '5': cin.clear(); cin.ignore(256, '\n'); vibroControl.play(0, 2);																								break;
      case '6': cin.clear(); cin.ignore(256, '\n'); vibroControl.play(0, 3);																								break;
      case '7': cin.clear(); cin.ignore(256, '\n'); vibroControl.play(0, 4);																								break;
      case '8': cin.clear(); cin.ignore(256, '\n'); vibroControl.play(0, 5);																								break;

      default: cin.clear(); cin.ignore(256, '\n'); cout << "Invalid choice!\n"; repeat = true;

      }

    } while (repeat);
  }

  return 0;
}