#include "VibroControl/VibroControl.h"

#include <iostream>

int main(int argc, char** argv)
{
  VibroControl vibroControl;

  for (unsigned int i = 0; i < 10; ++i)
  {
    vibroControl.upload({ {0.5, 0.1}, {0, 0.2} ,{0.8, 0.1}, {0, 0.2}, {1, 0.1}, {0, 0.2}, {0.5, 0.1}, {0, 0.2}, {0.8, 0.1} }, i);
  }
  
  vibroControl.play(0, 5);

  return 0;
}