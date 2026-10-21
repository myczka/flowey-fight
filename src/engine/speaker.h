#pragma once

#include "audio.h"

struct Voice {
    std::string voiceFile;
    int speed;
};

class Speaker {
    public:
        // Default constructor
        Speaker(ma_engine& engine) : speakerEngine(engine) {}

        // Getters and setters
        // ma_engine getEngine();
        // void setEngine(ma_engine& engine);

        // std::string getVoice();
        // void setVoice(std::string newVoice);

        // int getSpeed();
        // void setSpeed(int newSpeed);

        // Class functions
        void write(std::string mssg, Voice voice);
        void write(std::string mssg, Voice voice, int time);

        void writeRand(std::string mssg, std::vector<Voice> voiceVec);
        void writeRand(std::string yeet, int time, std::vector<std::string> fileVec);

        private : ma_engine& speakerEngine;
};

void playRandSnd(ma_engine& engine, std::vector<std::string> soundVec);