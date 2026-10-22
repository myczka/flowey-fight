#pragma once

#include "audio.h"

struct Voice {
    std::string voiceFile;
    int speed;
};

class Speaker {
    public:
        explicit Speaker(ma_engine& engine) : speakerEngine(engine) {}

        // Class functions
        void write(std::string mssg, Voice voice);
        void write(std::string mssg, Voice voice, int time);

        void writeRand(std::string mssg, std::vector<Voice> voiceVec);
        void writeRand(std::string yeet, int time, std::vector<std::string> fileVec);

    private : 
        ma_engine& speakerEngine;
};