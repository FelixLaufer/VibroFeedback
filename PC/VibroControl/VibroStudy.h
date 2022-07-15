#ifndef _VIBRO_STUDY_H_
#define _VIBRO_STUDY_H_

#include "VibroControl.h"

// ToDo & FixMe: everything!

class VibroStudy : public VibroControl
{
public:
  struct block
  {
    int condition; // Standing vs. moving
    int style; // Continous vs. multiple
    int motor[100]; // Front vs. back
  }blocklist[8];

  struct parameter
  {
    int id;
    int block_length;
    double frequency;
    double stimu_duration;
    double offtime_multiple;
    double varying_duration;
    double offtime_varying;
    double cycle_duration;
  }para;

  struct last_block_played
  {
    int condition;
    int style;
    int with_offtime;
    int order;
  }lbp;

  VibroStudy();
  ~VibroStudy();

  // Shows possible vibration commands
  void vibration_commands();
  // Reads in the parameters
  void read_parameters();
  // Changes one or all parameters
  void change_parameters();
  // Uploads pattern for multiple vibrations
  void upload_pattern();
  // Creates & uploads a pattern for varying frequency in random order
  void varpatt_ran();
  // Creates & uploads a pattern for varying frequency in systematic order
  void varpatt_sys();
  // Creates & uploads a pattern for varying frequency in random order with offtime
  void varpatt_ran_off();
  // Creates & uploads a pattern for varying frequency in systematic order with offtime
  void varpatt_sys_off();
  // Executes the functions for creating & uploading patterns for varying frequency vibration
  void upload_varying_patterns();
  // Generates a test-block with n randomly generated stimuli
  block generate_block(int condition, int style);
  // Generates 4 blocks, one for each combination of condition & style
  void generate_blocklist();
  // Prints the generated block to the console
  void display_block(block b);
  // Displays all blocks
  void display_blocks();
  // Opens menue to choose the block to be played
  void choose_block();
  // Plays the generated block, random decision whether a stimuli is played or not
  void play_block(int choice);
  // Menu for deciding for the parameters (offtime, random/systematic order) of varying vibration
  void choose_varying_block(int choice);
  // Plays the generated block with varying vibration, random decision whether a stimuli is played or not
  void play_varying_block(int choice, int with_offtime, int play_order);
  // Saves the log into a predefined path with the corresponding ID
  void save_log();
  // Converts frequency [Hz] to valid motor strength value
  double convert_frequency(double freq);
};

#endif