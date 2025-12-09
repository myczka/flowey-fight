/* 
This programs presents an interactive command line turn-based RPG fight against Flowey.
Copyright (c) 2022-2025 Jordan Myczka
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

std::string getBattleStr();

// Main Menu
Menu battle_menu(getBattleStr(), battleMenuVec, '>', true);
int bMenuSize = battleMenuVec.size();

// Sub Menus
Menu attack_menu("[Attack]", { "Fight", "Spells" }, true);
Menu mercy_menu("[Mercy]", { "Spare", "Flee" }, true);
Menu item_menu("[Items]", items);
Menu spells_menu("[Spells]", { "Fira Throw [25%]", "Bolt Lightning [15%]", "Chaos Blast [50%]", "Rude Buster [33%]"}, false);

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

void get_health(int remove) {
    remove = remove - damageMod;
    int emphasis = 0;
    int afterHealth = health - remove;

    if (remove == 0) {
        type(to_string(health) + "/600", 70);
        return;
    }

    if (afterHealth > 0) {
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

void get_flowey(int remove) {
    string hpDisplay = "Flowey's HP " + flowey;
    hpDisplay = hpDisplay + "/800";
    int length;
    int goal = flowey - remove;
    //string flowStr = to_string(remove);
    //int length = to_string(remove).length();
    //int tensRemove = int(flowStr[length - 1]);
    //int hundredsRemove = 0;
    playSound(effectEng, "slash.wav");
    type("Flowey's HP " + to_string(flowey) + "/800");
    sleep(400);
    backspace(4);

    //if (length == 3) {

    //}
    //else {
    //    if (tens - 2 > -1) {

    //        tens = tens - 7;

    //        for (int a = 9; a > -1; a--) {
    //            sleep(80);
    //            cout << "\b\b";
    //            playRand(mttTexts);
    //            cout << a << "9";
    //            if (a == tens) {
    //                break;
    //            }
    //        }
    //        for (int i = 8; i > -1; i--) {
    //            sleep(140);
    //            cout << "\b";
    //            playRand(mttTexts);
    //            cout << i;
    //        }
    //        playEnemyHurt();
    //        type("/800");
    //        flowey = flowey - 70;
    //    }
    //    else {
    //        hundreds = hundreds - 1;
    //        sleep(60);
    //        cout << "\b\b\b";
    //        //sleep(100);
    //        playRand(mttTexts);
    //        cout << hundreds << "99";

    //        tens = (tens - 7) + 10;

    //        for (int a = 9; a > -1; a--) {
    //            sleep(110);
    //            cout << "\b\b";
    //            //sleep(100);
    //            playRand(mttTexts);
    //            cout << a << "9";
    //            if (a == tens) {
    //                break;
    //            }
    //        }
    //        for (int i = 8; i > -1; i--) {
    //            sleep(170);
    //            cout << "\b";

    //            playRand(mttTexts);
    //            cout << i;
    //        }
    //        playEnemyHurt();
    //        sleep(40);
    //        std::cout << "/800";
    //        flowey = flowey - 70;
    //    }
    //    sleep(400);
    //    clear();

    while (flowey != goal + 10) {
        flowey -= 10;
        length = to_string(flowey).length();
        backspace(length);
        playRand(mttTexts);
        cout << flowey;
        sleep(70);
    }

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

void player_turn() {

    battle_menu.setPrompt(getBattleStr());
    bMenuSize = battleMenuVec.size();
    
start_turn:
    clear();
    num = battle_menu.select();

    if (num == bMenuSize - 4) {
        num = attack_menu.select();
        if (num == 0) {
            get_flowey(70);
        }
        else if (num == 1) {

            switch (spells_menu.select()) {
                case 0:
                    chara("* You used Fira.\n");
                    sleep(100);
                    get_flowey(200);
                    ok();
                    type("* Flowey practices burning in hell.");
                    mana -= 100 * .25;
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
            chara("* Ate a Candy Bar.");
            ok();
            type("* Defense increased!\n");
            type("* Gained 100 HP!");
            ok();
            health += 100;
            damageMod += 150;
            items.erase(items.begin() + num); 
        }
        else {
            goto start_turn;
        }

        //SOLVES: Trying to set empty vector for options
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

void flowey_turn() {
    int random_number_three;
    
    srand(time(0));

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
    
    volume(fallen_down, .6);
    volume(effectEng, 0.8);
    volume(voiceEng, .38);
    volume(buffVoiceEng, 1.2);
    volume(good_song, 1);
    volume(fight, .80);
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
        pitch(good_song, .9);
        volume(good_song, .8);
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
        waitTill(genius_evil, 34.2);
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
debug:
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
    volume(fight, .7);
    beginAt(fight, 14.7);
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