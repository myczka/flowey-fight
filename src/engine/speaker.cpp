#include "speaker.h"

#include "console.h"
#include "util.h"

#include <iostream>

void Speaker::write(std::string mssg, Voice voice) {
    int increm = 0;
    for (int i = 0; i < mssg.length(); ++i) {
        // begin(flowey_voice);
        wrap(i, mssg, increm);
        if (mssg[i] != ' ') {
            playSound(this->speakerEngine, voice.voiceFile);
        }
        std::cout << mssg[i];
        std::cout << std::flush;
        sleep(voice.speed);
    }
}

void Speaker::write(std::string mssg, Voice voice, int time) {
    int increm = 0;
    for (int i = 0; i < mssg.length(); ++i) {
        // begin(flowey_voice);
        wrap(i, mssg, increm);
        if (mssg[i] != ' ') {
            playSound(this->speakerEngine, voice.voiceFile);
        }
        std::cout << mssg[i];
        sleep(time);
    }
}

void Speaker::writeRand(std::string mssg, std::vector<Voice> voiceVec) {
    int increm = 0;
    int randIndex = 0;
    for (int i = 0; i < mssg.length(); ++i) {
        wrap(i, mssg, increm);
        if (mssg[i] != ' ') {
            randIndex = getRand(0, voiceVec.size() - 1);
            playSound(speakerEngine, voiceVec.at(randIndex).voiceFile);
        }
        std::cout << mssg[i];
        sleep(voiceVec.at(randIndex).speed);
    }
}

void Speaker::writeRand(std::string yeet, int time, std::vector<std::string> fileVec) {
    int increm = 0;
    for (int i = 0; i < yeet.length(); ++i) {
        wrap(i, yeet, increm);
        if (yeet[i] != ' ') {
            playRandSnd(this->speakerEngine, fileVec);
        }
        std::cout << yeet[i];
        sleep(time);
    }
}