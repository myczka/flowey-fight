#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <memory>

#include "audio.h"



class AudioManager {
    public:
        AudioManager();

        enum class Group {
            MUSIC,
            SFX,
            VOICE,
            COUNT
        };

    private:
        std::unordered_map<std::string, std::unique_ptr<ma_sound>> soundMap;
        std::array<ma_sound_group, static_cast<std::size_t>(Group::COUNT)> groupArray;
};