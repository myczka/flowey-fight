#pragma once

#include "miniaudio.h"
#include <string>
#include <vector>

extern std::string audioDir;

void seekTo(ma_sound& sound, float seconds) ;

void init(ma_engine& engine);

void initSound(ma_engine& engine, ma_sound& sound, std::string file, bool bin = true);

void uninit(ma_engine& engine);

void uninit(ma_sound& sound);

void playSound(ma_engine& engine, std::string file, bool bin = true);

void volume(ma_sound& sound, float volume);

void volume(ma_engine& engine, float volume);

void pitch(ma_sound& sound, float pitchVal);

void pitchDown(ma_sound& sound);

void pitchUp(ma_sound& sound);

void loop(ma_sound& sound);

void begin(ma_sound& sound);

void beginAt(ma_sound& sound, float seconds);

void endAt(ma_sound& sound, float seconds);

void waitTill(ma_sound& sound, float seconds);

void play(ma_sound& sound);

void pause(ma_sound& sound);

void start(ma_engine& engine);

void stop(ma_engine& engine);