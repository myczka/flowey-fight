#pragma once

#include "myaudio.h"
#include "console.h"

void playRandSnd(ma_engine &engine, std::vector<std::string> soundsNames) {
  playSound(engine, soundsNames.at(get_rand(0, soundsNames.size() - 1)));
}

void waitTill(ma_sound &sound, float seconds) {
  while ((ma_sound_get_time_in_milliseconds(&sound) / 1000.0f) < seconds) {
    sleep(1);
  }
}

void write(std::string yeet, int time, ma_engine &engine,
           std::string fileName) {
  int increm = 0;
  for (int i = 0; i < yeet.length(); ++i) {
    // begin(flowey_voice);
    wrap(i, yeet, increm);
    if (yeet[i] != ' ') {
      playSound(engine, fileName);
    }
    std::cout << yeet[i];
    sleep(time);
  }
}

void write(std::string yeet, int time, ma_engine &engine,
           std::vector<std::string> fileVec) {
  int increm = 0;
  for (int i = 0; i < yeet.length(); ++i) {
    wrap(i, yeet, increm);
    if (yeet[i] != ' ') {
      playRandSnd(engine, fileVec);
    }
    std::cout << yeet[i];
    sleep(time);
  }
}

void num_enter(char select, ma_sound &sound) {
  char choice;
  std::cout << std::endl;
  std::cout << ">";
  do {
    choice = _getch();
  } while (choice != select);
  play(sound);
  clear();
}

void num_enter(char select, ma_engine &engine, std::string soundStr) {
  char choice;
  do {
    choice = _getch();
  } while (choice != select);
  playSound(engine, soundStr);
  clear();
}