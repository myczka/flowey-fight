#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <memory>

#include "audio.h"

enum class Group {
    Music,
    Sfx,
    Voice,
    Count
};

class AudioManager {
    public:
        AudioManager();

    private:
        std::unordered_map<std::string, std::unique_ptr<ma_sound>> soundMap_;
        std::array<ma_sound_group, static_cast<std::size_t>(Group::Count)> groupArray_;

        ma_engine& audio_engine;
};