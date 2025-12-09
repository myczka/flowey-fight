#pragma once
#ifndef MYAUDIO_H
#define MYAUDIO_H
#define MINIAUDIO_IMPLEMENTATION

#include "miniaudio.h"
#include <iostream>
#include <vector>
#include <string>

static const string audioDir = "./sounds/";


void seekTo(ma_sound &sound, float seconds) {
    ma_uint64 totalFrames;
    ma_uint64 targetFrame;
    ma_uint32 sampleRate;

    // Get the sample rate of the sound.
    sampleRate = ma_engine_get_sample_rate(ma_sound_get_engine(&sound));

    // Get the total number of PCM frames in the sound.
    ma_sound_get_length_in_pcm_frames(&sound, &totalFrames);

    // Calculate the target frame based on the number of seconds.
    targetFrame = (ma_uint64)(seconds * sampleRate);

    // Ensure targetFrame is within the bounds of the sound length.
    if (targetFrame > totalFrames) {
        targetFrame = totalFrames;  // Clamp to the end of the sound.
    }

    // Seek to the target frame.
    ma_sound_seek_to_pcm_frame(&sound, targetFrame);
}

void init(ma_engine &engine) {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        std::cout << "Failed to initialize audio engine." << std::endl;
    }
}

void init_sound(ma_engine &engine, ma_sound &sound, std::string file, bool bin = true) {
    // Initialize the sound using the provided engine and file path
    if (bin) {
        file = audioDir + file;
    }
    else {
        file = "./" + file;
    }
    
    if (ma_sound_init_from_file(&engine, file.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, &sound) != MA_SUCCESS) {
        // Return false if initialization failed
        exit(0);
    }
}

void uninit(ma_engine &engine) {
    ma_engine_uninit(&engine);
}

void uninit(ma_sound &sound) {
    ma_sound_uninit(&sound);
}

void playSound(ma_engine &engine, std::string file, bool bin = true) {
    if (bin) {
        file = audioDir + file;
    }
    else {
        file = "./" + file;
    }

    ma_engine_play_sound(&engine, file.c_str(), NULL);
}

void volume(ma_sound &sound, float volume) {
    ma_sound_set_volume(&sound, volume);
}

void volume(ma_engine &engine, float volume) {
    ma_engine_set_volume(&engine, volume);
}

void pitch(ma_sound &sound, float pitchVal) {
    ma_sound_set_pitch(&sound, pitchVal);
}

void pitchDown(ma_sound &sound) {
    ma_sound_set_pitch(&sound, ma_sound_get_pitch(&sound) - .1);
}

void pitchUp(ma_sound &sound) {
    ma_sound_set_pitch(&sound, ma_sound_get_pitch(&sound) + .1);
}

void loop(ma_sound &sound) {
    ma_sound_set_looping(&sound, true);
}

void begin(ma_sound &sound) {
    ma_sound_seek_to_pcm_frame(&sound, 0);
    ma_sound_start(&sound);
}

void beginAt(ma_sound &sound, float seconds) {
    seekTo(sound, seconds);
    ma_sound_start(&sound);
}

void endAt(ma_sound &sound, float seconds) {
    ma_sound_set_stop_time_in_milliseconds(&sound,
        ma_engine_get_time_in_milliseconds(ma_sound_get_engine(&sound))
        + ma_sound_get_time_in_milliseconds(&sound) + seconds);
}

void play(ma_sound &sound) {
    ma_sound_start(&sound);
}

void pause(ma_sound &sound) {
    ma_sound_stop(&sound);
}

void start(ma_engine &engine) {
    ma_engine_start(&engine);
}

void stop(ma_engine &engine) {
    ma_engine_stop(&engine);
}

void reset(ma_sound &sound) {
    ma_sound_set_at_end(&sound, false);
}

#endif