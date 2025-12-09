#pragma once
#ifndef CONSOLE_H
#define CONSOLE_H
//#define MINIAUDIO_IMPLEMENTATION
#include <string>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <thread>
#include <ctime>
#include <random>

//#include <Windows.h>
//#include <ctype.h>
//#include "miniaudio.h"

int get_rand(int first, int last) {
    std::random_device rd;
    std::mt19937 gen(rd() ^ static_cast<unsigned>(std::time(0))); // random seed with time
    std::uniform_int_distribution<int> dist(first, last);
    return dist(gen);
}

void sleep(int delayMs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
}

void clear() {
    std::cout << "\033[2J\033[1;1H";
}

void triple_dot() {
    sleep(150);
    std::cout << ".";
    sleep(200);
    std::cout << ".";
    sleep(200);
    std::cout << ".";
    sleep(800);
    std::cout << "\b\b\b";
    std::cout << "   ";
    std::cout << "\b\b\b";
}

void triple_dot_slow() {
    sleep(150);
    std::cout << ".";
    sleep(200);
    std::cout << ".";
    sleep(200);
    std::cout << ".";
    sleep(1000);
    sleep(70);
    clear();
}

void wrap(int i, std::string yeet, int &increm) {
    if (i / 25 > increm && yeet[i - 1] == ' ') {
        std::cout << std::endl;
        if (yeet[0] == '*') {
            std::cout << "  ";
        }
        increm = increm + 1;
    }
}

void write(std::string yeet, int time) {
    int increm = 0;
    for (int i = 0; i < yeet.length(); ++i) {
        wrap(i, yeet, increm);
        std::cout << yeet[i];
        sleep(time);
    }
}

void backspace(int spaces) {
    for (int i = 0; i < spaces; ++i) {
        std::cout << "\b";
    }
}

void backspace(int spaces, int sleeptime) {
    for (int i = 0; i < spaces; ++i) {
        std::cout << "\b";
        sleep(sleeptime);
    }
}

void backspaceWeird(int spaces, int sleeptime) {
    std::cout << "\b";
    for (int i = 0; i < spaces; ++i) {
        std::cout << " ";
        std::cout << "\b\b";
        sleep(sleeptime);
    }
}

void num_enter(char select) {
    char choice;
    std::cout << std::endl;
    do {
        choice = _getch();
    } while (choice != select);
    clear();
}

//void seekTo(ma_sound &sound, float seconds) {
//    ma_uint64 totalFrames;
//    ma_uint64 targetFrame;
//    ma_uint32 sampleRate;
//
//    // Get the sample rate of the sound.
//    sampleRate = ma_engine_get_sample_rate(ma_sound_get_engine(&sound));
//
//    // Get the total number of PCM frames in the sound.
//    ma_sound_get_length_in_pcm_frames(&sound, &totalFrames);
//
//    // Calculate the target frame based on the number of seconds.
//    targetFrame = (ma_uint64)(seconds * sampleRate);
//
//    // Ensure targetFrame is within the bounds of the sound length.
//    if (targetFrame > totalFrames) {
//        targetFrame = totalFrames;  // Clamp to the end of the sound.
//    }
//
//    // Seek to the target frame.
//    ma_sound_seek_to_pcm_frame(&sound, targetFrame);
//}
//
//void init(ma_engine &engine) {
//    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
//        std::cout << "Failed to initialize audio engine." << std::endl;
//    }
//}
//
//void init_sound(ma_engine &engine, ma_sound &sound, std::string file) {
//    // Initialize the sound using the provided engine and file path
//    file = "./bin/" + file;
//    if (ma_sound_init_from_file(&engine, file.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, &sound) != MA_SUCCESS) {
//        // Return false if initialization failed
//        exit(0);
//    }
//}
//
//void uninit(ma_engine &engine) {
//    ma_engine_uninit(&engine);
//}
//
//void uninit(ma_sound &sound) {
//    ma_sound_uninit(&sound);
//}
//
//void playSound(ma_engine &engine, std::string file) {
//    file = "./bin/" + file;
//    
//    ma_engine_play_sound(&engine, file.c_str(), NULL);
//}
//
//void playRandSnd(ma_engine &engine, std::vector<std::string> soundsNames) {
//    playSound(engine, soundsNames.at(get_rand(0, soundsNames.size() - 1)));
//}
//
//void volume(ma_sound &sound, float volume) {
//    ma_sound_set_volume(&sound, volume);
//}
//
//void volume(ma_engine &engine, float volume) {
//    ma_engine_set_volume(&engine, volume);
//}
//
//void pitch(ma_sound &sound, float pitchVal) {
//    ma_sound_set_pitch(&sound, pitchVal);
//}
//
//void pitchDown(ma_sound &sound) {
//    ma_sound_set_pitch(&sound, ma_sound_get_pitch(&sound) - .1);
//}
//
//void pitchUp(ma_sound &sound) {
//    ma_sound_set_pitch(&sound, ma_sound_get_pitch(&sound) + .1);
//}
//
//void loop(ma_sound &sound) {
//    ma_sound_set_looping(&sound, true);
//}
//
//void begin(ma_sound &sound) {
//    ma_sound_seek_to_pcm_frame(&sound, 0);
//    ma_sound_start(&sound);
//}
//
//void beginAt(ma_sound &sound, float seconds) {
//    seekTo(sound, seconds);
//    ma_sound_start(&sound);
//}
//
//void endAt(ma_sound &sound, float seconds) {
//        ma_sound_set_stop_time_in_milliseconds(&sound, 
//            ma_engine_get_time_in_milliseconds(ma_sound_get_engine(&sound)) 
//            + ma_sound_get_time_in_milliseconds(&sound) + seconds);
//}
//
//void play(ma_sound &sound) {
//    ma_sound_start(&sound);
//}
//
//void pause(ma_sound &sound) {
//    ma_sound_stop(&sound);
//}
//
//void start(ma_engine &engine) {
//    ma_engine_start(&engine);
//}
//
//void stop(ma_engine &engine) {
//    ma_engine_stop(&engine);
//}
//
//void reset(ma_sound &sound) {
//    ma_sound_set_at_end(&sound, false);
//}
//
//void waitTill(ma_sound &sound, float seconds) {
//    while ((ma_sound_get_time_in_milliseconds(&sound) / 1000.0f) < seconds) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));
//    }
//}

#endif
