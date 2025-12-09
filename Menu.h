#ifndef MENU_H
#define MENU_H
#define MINIAUDIO_IMPLEMENTATION

/* Copyright (c) 2025 Jordan Myczka */

#include <string>
#include <iostream>
#include <Windows.h>
#include "miniaudio.h" // Include miniaudio for audio playback
#include <vector>

class Menu {
public:
    Menu(std::string prompt, const std::vector<std::string> &options, bool sideways = false);
    Menu(std::string prompt, const std::vector<std::string> &options, char select, bool sideways = false);
    ~Menu();
    void clear();
    void display();
    void display(bool val);
    int select();
    int readKey();
    void setOptions(std::vector<std::string> &options);
    void setPrompt(std::string prompt);
    
private:
    char selected = '*';
    const char empty = ' ';
    std::vector<std::string> options;
    //std::vector<std::vector<std::string>> vecStr;
    int index = 0;
    int key = -1;
    int size = 0;
    int truesize;
    bool sideways = false;
    std::string selectedStr;
    std::string prompt;

    // Miniaudio-specific variables
    ma_engine engine;
    void playSelectSound(std::string ohYeah);
};

Menu::Menu(std::string prompt, const std::vector<std::string> &options, bool sideways)
    : sideways(sideways), options(options), prompt(prompt) {
    size = options.size();
    truesize = size - 1;

    if (sideways) {
        for (int i = 0; i < size; ++i) {
            Menu::options.at(i) = " " + Menu::options.at(i);
        }
    }
    else {
        for (int i = 0; i < size; ++i) {
            Menu::options.at(i) = "  " + Menu::options.at(i);
        }
    }

    Menu::options[0][0] = selected;

    // Initialize the miniaudio engine
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
    }
}

Menu::Menu(std::string prompt, const std::vector<std::string> &options, char select, bool sideways)
    :  selected(select), sideways(sideways), options(options), prompt(prompt) {
    size = options.size();
    truesize = size - 1;
    selected = select;

    if (sideways) {
        for (int i = 0; i < size; ++i) {
            Menu::options.at(i) = " " + Menu::options.at(i);
        }
    }
    else {
        for (int i = 0; i < size; ++i) {
            Menu::options.at(i) = "  " + Menu::options.at(i);
        }
    }

    Menu::options[0][0] = selected;

    // Initialize the miniaudio engine
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
    }
}

Menu::~Menu() {
    // Uninitialize the miniaudio engine when done
    ma_engine_uninit(&engine);
    clear();
}

void Menu::clear() {
    std::cout << "\033[2J\033[1;1H";
}

void Menu::setOptions(std::vector<std::string> &options) {
    this->options = options;
    size = options.size();
    truesize = size - 1;

    if (sideways) {
        for (int i = 0; i < size; ++i) {
            Menu::options.at(i) = " " + Menu::options.at(i);
        }
    }
    else {
        for (int i = 0; i < size; ++i) {
            Menu::options.at(i) = "  " + Menu::options.at(i);
        }
    }

    Menu::options[0][0] = selected;
}

void Menu::setPrompt(std::string prompt) {
    this->prompt = prompt;
}

void Menu::playSelectSound(std::string ohYeah) {
    // Play the sound using miniaudio
    ohYeah = "./audio/" + ohYeah;
    ma_engine_play_sound(&engine, ohYeah.c_str(), NULL);
}

void Menu::display() {
    clear();

    std::cout << Menu::prompt << std::endl;
    playSelectSound("menu_navigate.wav");
    
    if (sideways) {
        for (int i = 0; i < size; ++i) {
            std::cout << options[i] << "  ";
        }
    }
    else {
        for (int i = 0; i < size; ++i) {
            std::cout << options[i] << std::endl;
        }
    }
}

void Menu::display(bool val) {
    if (!val) {
        clear();
    }

    std::cout << Menu::prompt << std::endl;
    playSelectSound("menu_navigate.wav");

    if (sideways) {
        for (int i = 0; i < size; ++i) {
            std::cout << options[i] << "  ";
        }
    }
    else {
        for (int i = 0; i < size; ++i) {
            std::cout << options[i] << std::endl;
        }
    }
}

int Menu::select() {
    index = 0;
    do {
        display();
        key = readKey();
        if (key == 2 || key == 4) {
            if (index != size - 1) {
                options[index][0] = empty;
                index++;
                options[index][0] = selected;
            }
            else {
                options[index][0] = empty;
                index -= truesize;
                options[index][0] = selected;
            }
        }
        else if (key == 1 || key == 3) {
            if (index != 0) {
                options[index][0] = empty;
                index--;
                options[index][0] = selected;
            }
            else {
                options[index][0] = empty;
                index += truesize;
                options[index][0] = selected;
            }
        }
        else if (key == 7) {
            break;
        }
        else {
            selectedStr = options[index];
            options[index][0] = empty;
            selectedStr[0] = empty;
            options[0][0] = selected;
            /*for (int i = 0; i < selectedStr.length(); ++i) {
                selectedStr[i] = selectedStr[i + 1];
            }*/
        }

    } while (key != 0);

    clear();
    if (key == 7) {
        return key;
    }
    else {
        return index;
    }
}

int Menu::readKey() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD fdwMode = 0;
    DWORD fdwOldMode; // To store the original console mode

    // Save the current console mode
    GetConsoleMode(hStdin, &fdwOldMode);

    // Set the console mode to enable input processing for key events
    GetConsoleMode(hStdin, &fdwMode);
    SetConsoleMode(hStdin, fdwMode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

    INPUT_RECORD irInBuf[128];
    DWORD cNumRead;

    while (true) {
        // Read input events
        ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead);

        for (DWORD i = 0; i < cNumRead; i++) {
            if (irInBuf[i].EventType == KEY_EVENT && irInBuf[i].Event.KeyEvent.bKeyDown) {
                //if (Menu::sideways) {
                    switch (irInBuf[i].Event.KeyEvent.wVirtualKeyCode) {
                        case VK_UP: return 1;
                        case VK_DOWN: return 2;
                        case VK_LEFT: return 3;
                        case VK_RIGHT: return 4;
                        case 0x5A: 
                            SetConsoleMode(hStdin, fdwOldMode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));
                            playSelectSound("menu_select.wav"); return 0;
                        case 0x58: 
                            SetConsoleMode(hStdin, fdwOldMode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));
                            return 7 ;
                    }
                /* }
                else {
                    switch (irInBuf[i].Event.KeyEvent.wVirtualKeyCode) {

                        case VK_UP: return 1;
                        case VK_DOWN: return 2;

                        case 0x5A: 
                            SetConsoleMode(hStdin, fdwOldMode);
                            playSelectSound("menu_select.wav"); return 0;
                        case 0x58: 
                            SetConsoleMode(hStdin, fdwOldMode);
                            return 7;
                    }
                }*/
            }
        }
    }
}

//Menu::Menu(std::string prompt, const std::vector<std::vector<std::string>> &vecStr)
//    : prompt(prompt), vecStr(vecStr) {
//    //size = N;  // deduce array size
//
//    truesize = size - 1;
//
//    for (int i = 0; i < vecStr.size(); ++i) {
//        for (int j = 0; j < vecStr.at(i).size(); ++i) {
//            Menu::vecStr.at(i).at(j) = "  " + Menu::vecStr.at(i).at(j);
//        }
//    }
//
//    Menu::vecStr[0][0][0] = selected;
//
//    // Initialize the miniaudio engine
//    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
//        std::cerr << "Failed to initialize audio engine." << std::endl;
//    }
//}

#endif
