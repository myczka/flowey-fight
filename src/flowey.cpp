/* 
This programs presents an interactive command line turn-based RPG fight against Flowey.
Copyright (c) 2022-2025 Jordan Myczka. All rights reserved.
*/

//#include <sstream>
//#include <random>
//#include <array>
#include "consoleaudio.h"
#include "Menu.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

    /* Global Variables and Instances*/
// From "myaudio.h"
string audioDir = "./audio/";

int tens = 0;
int hundreds = 8;
int flowey = 800;
int health = 600;
int damageMod = 0;
double mana = 100.0;

int num = 0;
int turn = 0;

int typeTime = 30;
int flowTime = 50;
int charaTime = 50;
int myArgc;

// States
bool lore = false;
bool defend = false;
bool pedo = false;

// Strings
string space = "";
string yeet = "";
string name = "Debug";
string last;
string floweyText = "snd_floweytalk1.wav";
string wolfeyText = "snd_floweytalk2.wav";
string charaText = "char_text.wav";
string menuText = "menu_text.wav";

// Vectors
vector<string> mttTexts = { "snd_mtt1.wav", "snd_mtt2.wav", "snd_mtt3.wav", "snd_mtt4.wav", "snd_mtt5.wav", "snd_mtt6.wav", "snd_mtt7.wav", "snd_mtt8.wav", "snd_mtt9.wav" };
vector<string> lores = { "sadistically", "nightmare", "your GENOCIDE" };
vector<string> serol = { "wildly", "friend", "his \"Hopes and Dreams\"" };
vector<string> items = { "Candy Bar", "Candy Bar", "Apple Pie" };
vector<string> battleMenuVec = { "Attack", "Defend", "Items", "Mercy" };

// Audio Engines and Sounds
ma_engine voiceEng;
ma_engine buffVoiceEng;
ma_engine mainEng;
ma_engine effectEng;
ma_sound ok_sound;
ma_sound happy_voice;
ma_sound good_song;
ma_sound fight;
ma_sound fallen_down;
ma_sound genius_evil;

// Root Menu
// Function to get string of player variables for battle_menu. Needs to be a function since variables will change.
// Not redundant << used more than once.
std::string getBattleStr();
Menu battle_menu(getBattleStr(), battleMenuVec, '>', true);
int bMenuSize = battleMenuVec.size();

// Sub Menus
Menu attack_menu("{Attack}", { "Fight", "Spells" }, true);
Menu mercy_menu("{Mercy}", { "Spare", "Flee" }, true);
Menu item_menu("{Items}", items);
Menu spells_menu("{Spells}  MP" , {"Bolt [10%]", "Fira [20%]", "Blizzaga [25%]", "Rude Buster [33%]", "Chaos Blast [50%]"}, false);

    /* Helper Function Declarations */
void ok();

void chara(std::string yeet, int time);

void chara(std::string yeet);

void type(std::string yeet, int time);

void type(std::string yeet);

void flow(std::string yeet, int sleeptime);

void flow(std::string yeet);

void wolf(std::string yeet, int time);

void wolf(std::string yeet);

void wolfey(std::string yeet, int sleeptime);

void mett(std::string yeet);

void playEnemyHurt();

void playRand(std::vector<std::string> soundsNames);

    /* Primary Functions */
// TODO: Get mana method to make mana reduction modular and immersive
void get_mana(int remove) {


}

// This function removes the amount passed from the players health
void get_health(int remove) {
    int emphasis = 0;

    // Verify damageMod won't make remove negative
    if (remove < damageMod) {
        remove = 0;
    }
    else {
        remove = remove - damageMod;
    }

    // Calculate the resulting player health and conduct conditional to choose between primary actions
    int afterHealth = health - remove;
    if (remove == 0) {
        type(to_string(health) + "/600", 70);
        return;
    }
    else if (afterHealth > 0) {
        if (afterHealth < 100) {
            emphasis = 30;
        }

        type(name + "  HP " + to_string(health) + "/600", 50 + emphasis);
        playSound(effectEng, "slash.wav");
        backspace(4 + to_string(health).length(), 20 + emphasis);

        if (to_string(health).length() > to_string(afterHealth).length()) {
            for (int i = 0; i < to_string(health).length() - to_string(afterHealth).length(); i++) {
                cout << " ";
                sleep(70 + emphasis);
            }
        }
        health = afterHealth;
        playSound(effectEng, "snd_damage.wav");
        type(to_string(afterHealth) + "/600", 70 + emphasis);
    }
    else {
        health = 0;
        type(name + "  HP " + to_string(health) + "/600", 200);
    }
}

// This function serves to print out floweys health decreasing over time and ends up reducing it by the value passed.
void get_flowey(int remove) {
    int goal = flowey - remove;
    int length;

    // Initial health printout and damage sfx
    playSound(effectEng, "slash.wav");
    type("Flowey's HP " + to_string(flowey) + "/800");
    sleep(400);
    backspace(4);

    // Ticks down the tens place and hundreds place
    while (flowey != goal + 10) {
        flowey -= 10;
        length = to_string(flowey).length();
        backspace(length);
        playRand(mttTexts);
        cout << flowey;
        sleep(70);
    }

    // Ticks down the ones place
    for (int i = 0; i < 10; i++) {
        length = to_string(flowey).length();
        flowey -= 1;
        backspace(length);
        playRand(mttTexts);
        cout << flowey;
        sleep(100);
    }
    playEnemyHurt();
}  

// Player's Turn Sequence
// TODO: Redundant << used once.
void player_turn() {
    battle_menu.setPrompt(getBattleStr());
    bMenuSize = battleMenuVec.size();
    
start_turn:
    clear();
    num = battle_menu.select();

    if (num == bMenuSize - 4) {
        // Attack Menu Selection
        // TODO: Math to balance the spell damage to mana percent ratio with flowey's health and default fight damage.
        num = attack_menu.select();
        if (num == 0) {
            // 5% of flowey's current max health
            get_flowey(40);
        }
        else if (num == 1) {

            switch (spells_menu.select()) {
                case 0:
                    chara("* You used Bolt.\n");
                    sleep(100);
                    get_flowey(100);
                    ok();
                    type("* Flowey tastes the electricity.");
                    mana -= 100 * .1;
                    ok();
                    break;
                case 1:
                    chara("* You used Fira.\n");
                    sleep(100);
                    get_flowey(150);
                    ok();
                    type("* Flowey practices burning in he!!.");
                    mana -= 100 * .20;
                    ok();
                    break;
                default:
                    type("* This is the default.");
                    sleep(100);
                    ok();
                    goto start_turn;
                    break;
            }
        }
        else {
            goto start_turn; 
        }
    }
    else if (num == bMenuSize - 3) {
        defend = true;
        sleep(500);
        chara("* You got into the fetal position.");
        ok();
        
    }
    else if (num == bMenuSize - 2) {
        num = item_menu.select();

        //SOLVES: Bad index accessor
        if (num == 7) goto start_turn;
        if (items.at(num) == "Apple Pie") {
            chara("* Ate an Apple Pie");
            ok();
            type("* Gained 300 HP!");
            ok();
            health += 300;
            items.erase(items.begin() + num);
        } 
        else if (items.at(num) == "Candy Bar") {
            health += 100;
            damageMod += 150;
            chara("* Ate a Candy Bar.");
            ok();
            type("* Defense increased!\n");
            type("* Gained 100 HP!");
            ok();
            items.erase(items.begin() + num);
        }
        else {
            goto start_turn;
        }

        // Sets items vector to empty to fix issue passing empty vec to setOptions and to inform player that all items have been used.
        if (items.empty()) {
            items.push_back("Empty");
        }

        item_menu.setOptions(items);
    }
    else if (num == bMenuSize - 1) {
        num = mercy_menu.select();

        if (num == 0) {
            if (lore) {
                sleep(500);
                chara("* You tried to spare TRUE Flowey!");
                ok();
                if (flowey > 50) {
                    type("* Flowey is strong enough to stick-up for himself");
                }
            }
            else if (pedo) {
                sleep(500);
                chara("* You cry out and beg for mercy");
                ok();
                wolf("You're a pathetic monster...");
                sleep(1000);
                clear();
            }
            else {
                sleep(500);
                chara("* Sociopaths Can't spare TRUE Flowey");
            }
        } 
        else if (num == 1) {
            if (pedo) {
                wolf("Like I'd let you run away you coward...");
                sleep(1000);
                clear();
            } 
            else {
                num = get_rand(0, 1);
                if (num == 1) {
                    chara("* You got away safely!");
                    sleep(500);
                    exit(0);
                }
                else {
                    type("* Lol better luck next turn");
                }
            }
        }
        else {
            goto start_turn;
        }
    }
    else {
        goto start_turn;
    }
    sleep(400);
}

//TODO: Make defend function to check for defend
void flowey_turn() {
    int random_number_three;
    
    srand(static_cast<unsigned int>(time(0)));

    if (turn > 1) {
        random_number_three = rand() % 80;
    }
    else {
        random_number_three = rand() % 69;
    }
    
    clear();
    
    if (random_number_three <= 33) {
        type("* Flowey used Real Knife!\n");
        if (defend) {
            sleep(900);
            chara("* You blocked the attack", 50);
            triple_dot();
            defend = false;
            random_number_three = 79;
            clear();
            goto exit;
        }
        sleep(700);
        get_health(200);
    }
    else if (random_number_three <= 53) {
        sleep(80);
        type("* Flowey used Piercing Vines.\n");
        if (defend) {
            sleep(900);
            chara("* You blocked the attack", 50);
            triple_dot();
            defend = false;
            random_number_three = 79;
            clear();
            goto exit;
        }
        sleep(500);
        get_health(300);
    }
    else if (random_number_three <= 69) {
        sleep(80);
        type("* Flowey used Rainbow Blast?\n");
        sleep(600);
        get_health(500);
    }
    else if (random_number_three <= 75) {
        if (lore) {

        }
        else {
            sleep(80);
            type("* Flowey erased you!!!\n");
            if (!defend) {
                // TODO: Mute mainEng
                type("9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999", 1);
                exit(0);
            }
            sleep(500);
            health = 1;
            get_health(0);
        }
        
    }
    else {
        type("* Doged the attack!\n");
        sleep(500);
        get_health(0);
    }
    ok();
    clear();

    exit:
    sleep(100);
    if (random_number_three <= 33) {
        type("* Flowey laughs " + lores[0]);
    }
    else if (random_number_three <= 53) {
        type("* Flowey claims he is your best " + lores[1]);
    }
    else if (random_number_three <= 69) {
        type("* Flowey remembers " + lores[2]);
    }
    else if (random_number_three <= 73) {
        type("* Flowey feels nothing");
    }
    else {
        type("* Flowey hates your guts");
    }
    ok();
    sleep(150);
}

int main(int argc, char **argv) {
    // TODO: Continue the 'Joe Biden Pedophile Route' lore.
    Menu yes_no("Ready?", { "Yes", "No" }, true);
    Menu maybe("It's really you?", { "Yes", "No" }, true);
    
    myArgc = argc;

    if (items.empty()) {
        if (argc > 1) {
            for (int i = 1; i < argc; ++i) {
                items.push_back(argv[i]);
            }
        }
        else {
            items.push_back("Empty");
        }
    }

    init(mainEng);
    init(voiceEng);
    init(effectEng);
    init(buffVoiceEng);
    init_sound(effectEng, ok_sound, "menu_select.wav");
    /*init_sound(mainEng, fight, "Enter Sanman_v1.wav");*/
    init_sound(mainEng, fight, "Axel Fania_v1.wav");
    init_sound(mainEng, good_song, "Your Best Friend.wav");
    init_sound(mainEng, fallen_down, "Fallen Down Lofi dithered.wav");
    init_sound(mainEng, genius_evil, "For the Damaged Coda.wav");

    pitch(ok_sound, .5);
    pitch(fallen_down, 0.75);
    
    volume(fallen_down, 0.6f);
    volume(effectEng, 0.8f);
    volume(voiceEng, 0.38f);
    volume(buffVoiceEng, 1.2f);
    volume(good_song, 1);
    volume(fight, 0.80f);
    volume(genius_evil, .5);

    loop(fallen_down);

    start(buffVoiceEng);
    start(voiceEng);
    start(mainEng);
    //goto debug;

    play(good_song);

    sleep(1000);
    flow("Howdy!");
    sleep(500);
    flow(" I'm Flowey!\n");
    sleep(600);
    flow("Flowey the Flower!");
    sleep(500);
    clear();
    flow("If I may ask,");
    sleep(350);
    flow(" what is your name?\n");

    getline(cin, name);

    if (name == "Chara") {
        lore = true;
        lores = serol;
        flowTime = 75;
        typeTime = 40;

        clear();
        pause(good_song);

        wolf("It's really you?\n", 90);
        sleep(100);
        chara(" Yes   No\n", 65);

        num = maybe.select();
        if (num == 1) {exit(0);}

        clear();
        play(fallen_down);
        sleep(600);
        flow("I've been waiting...");
        sleep(800);
        clear();
        flow("I knew you were still alive");
        triple_dot_slow();
        sleep(1000);
        clear();
        flow("I've missed you...");
        sleep(900);
        clear();
        flow("...but I can never forgive you.");
        sleep(1000);
        clear();
        flow("I can never forgive you because of what you did");
        triple_dot_slow();
        sleep(1100);
        clear();
        flow("...and who you became.");
        sleep(3000);
        clear();
        goto fight_start;
    }
    else if (name == "Elon") { 
        pause(good_song);
        uninit(good_song);
        init_sound(mainEng, good_song, "Dm9_G7_CM7_Am7.wav");
        play(good_song);
        loop(good_song);
        pitch(good_song, 0.9f);
        volume(good_song, 0.8f);
        clear();
        goto fight_start; 
    }
    else if (name == "Joe Biden") {
        battle_menu.setPrompt(name + "  HP " + to_string(health) + "/600");
        pedo = true;
        clear();
        pause(good_song);
        sleep(1000);
        playSound(effectEng, "snd_floweylaugh.wav");
        sleep(1050);
        flow("You little child toucher, you!");
        sleep(1800);
        clear();
        sleep(100);
        get_health(500);
        sleep(3000);
        clear();
        sleep(400);
        play(genius_evil);
        sleep(100);
        wolf("Not in my text-based RPG combat system.", 150);
        sleep(1500);
        clear();
        sleep(1500);
        wolfey("You know what!?", 400);
        clear();
        wolfey("Typically I just call the cops on creepy old men like you!!", 700);
        clear();
        wolfey("But today... ", 600);
        clear();
        wolfey("TODAY, ", 500);
        wolfey("IMMA TEACH YOU A LESSON!!", 1000);
        clear();
        sleep(600);
        wolf("I'm not even gonna give you the option to attack, buddy.\n", 110);
        sleep(500);
        battle_menu.setPrompt(name + "  HP " + to_string(health) + "/600");
        battle_menu.display();
        waitTill(genius_evil, 34.2f);
        clear();

        battleMenuVec.erase(battleMenuVec.begin());
        battle_menu.setOptions(battleMenuVec);
        waitTill(genius_evil, 37.475f);
        goto player_turn;
    }

    pause(good_song);
    sleep(400);
    clear();
    playSound(effectEng, "snd_screenshake.wav");
    sleep(1000);
    playSound(effectEng, "mus_f_laugh.wav");
    sleep(6000);
    play(fight);
    sleep(300);
   
    endAt(fight, 14000); 

    sleep(200);
    wolf(name + ",", 100);
    sleep(380);
    wolf(" I'm going to kill you.\n", 100);
    sleep(700);
    wolf("Actually, ", 50);
    wolf(" I'm going to do worse than kill you.\n", 60);
    sleep(300);
    clear();
    wolf("I'm going to touch you...", 80);
    sleep(1700);
    wolf("INAPPROPRIATELY :]", 150);
    sleep(1430);
    clear(); 
    uninit(fight);

//debug:
    flow("Ready?\n", 50);
    sleep(100);
    chara(" Yes   No\n", 65);

    num = yes_no.select();

    if (num == 0) {
        playSound(effectEng, "start.wav");
        wolf("Let's roll.\n");
        sleep(600);
        clear();
    }
    else {
        flow("k bye.");
        sleep(300);
        exit(0);
    }

    init_sound(mainEng, fight, "Axel Fania_v1.wav");
    loop(fight);
    volume(fight, 0.7f);
    beginAt(fight, 14.7f);
    type("* Flowey Attacks!\n", 25);
    sleep(335);
    clear();
    sleep(300);

fight_start:
    while (flowey > 0) {

        turn += 1;

        flowey_turn();

        if (health < 1) {
            break;
        }
player_turn:
        player_turn();

    }
    
    // TODO: Doesn't change dialogue for Flowey dead
    sleep(360);
    clear();
    flow("* What will you do?\n");
    pitchDown(fight);
    pitchDown(genius_evil);
    triple_dot();
    pitchDown(fight);
    //pitchDown(genius_evil);
    sleep(400);
    get_health(0);
    sleep(1000);
    triple_dot_slow();
    triple_dot();
    flow("Oh wait", 70);
    pitchDown(fight);
    pitchDown(genius_evil);
    triple_dot();
    clear();

    wolf("What's wrong?");
    sleep(1000);
    pitchDown(fight);
    clear();
    pitchDown(fight);
    triple_dot_slow();
    pitchDown(fight);
    wolf("You look pale :)", 100);
    pitchDown(fight);
    sleep(1000);
}

std::string getBattleStr() {
    return (name + "  HP " + to_string(health) + "/600" + "  MN " + to_string(int(mana)) + "%");
}

void ok() {
   
    num_enter('z', ok_sound);
}

void chara(std::string yeet, int time) {
    write(yeet, time, buffVoiceEng, charaText);
}

void chara(std::string yeet) {
    chara(yeet, charaTime);
}

void type(std::string yeet, int time) {
    write(yeet, time, voiceEng, menuText);
}

void type(std::string yeet) {
    type(yeet, typeTime);
}

void flow(std::string yeet, int sleeptime) {
    write(yeet, flowTime, voiceEng, floweyText);
    sleep(sleeptime);
}

void flow(std::string yeet) {
    flow(yeet, 0);
}

void wolf(std::string yeet, int time) {
    write(yeet, time, buffVoiceEng, wolfeyText);
}

void wolf(std::string yeet) {
    wolf(yeet, flowTime);
}

void wolfey(std::string yeet, int sleeptime) {
    wolf(yeet, flowTime + 30);
    sleep(sleeptime);
}

void mett(std::string yeet) {
    write(yeet, 60, voiceEng, mttTexts);
}

void playEnemyHurt() {
    playSound(effectEng, "snd_hurt1.wav");
    sleep(700);
}

void playRand(std::vector<std::string> soundsNames) {
    playRandSnd(voiceEng, mttTexts);
}