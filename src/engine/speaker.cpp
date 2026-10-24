#include "speaker.h"

#include "console.h"
#include "util.h"

#include <iostream>

void Speaker::write(std::string mssg, Voice voice) {
    int increm = 0;
    int parsed;
    bool parsing = false;
    std::string parsedStr = "";
    for (int i = 0; i < mssg.length(); ++i) {
        // begin(flowey_voice);

        if (mssg[i] == '|') {
            if (parsing) {
                parsing = false;
                parsed = std::stoi(parsedStr);
                parsedStr = "";
                // Most important line
                sleep(parsed);
            } else {
                parsing = true;
            }
            continue;
        }
        else if (parsing) {
            parsedStr = parsedStr + mssg[i];
            continue;
        } 
        else if (mssg[i] != ' ') {
            playSound(this->speakerEngine, voice.voiceFile);
        }
        wrap(i, mssg, increm);
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