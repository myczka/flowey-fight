# Flowey Fight
This is literally just a command-line turn-based fight against Flowey, in C++, for practice.

## Dependencies
This program implements some header files from [my C++ Library repo](https://github.com/myczka/cppLibrary). The repository itself had been included in this repository as a subtree under lib/cppLibrary.
- The console headers simplify actions like sleeping and clearing the terminal window, which are essential for an immersive command-line RPG experience.
  - console.h
  - consoleaudio.h

- Also included are the miniaudio header file from the [miniaudio C++ audio library](https://github.com/mackron/miniaudio) as well as my own abstraction of the more important functions provided in the miniaudio header.
  - miniaudio.h
  - myaudio.h

- Finally, there is the Menu header file, which contains the Menu class definition as well as an inline declaration of the Menu class. I intend to turn the Menu class into a tree-based node and provide an overarching MenuTree file for the sake of practice with data structures in C++.
  - Menu.h

## Copyright Notice
The following listed files are owned by me and protected by copyright. Any attempt to claim ownership of these songs or distribute them in any manner is not permitted and is subject to legal pursuit:
- Dm9_G7_CM7_Am7.wav
- Axel Fania.wav

Though the other two songs are technically original works, they are actually very accurate recreations of the songs of the same name from UNDERTALE, thanks to the [most recent UNDERTALE Sample Spreadsheet](https://docs.google.com/spreadsheets/d/1eOyF0lWWKb_2q4eX7Hs80JIv6o9ZE9yWCksxCbmPTZs/edit?gid=767604283#gid=767604283).

## Other Notes
The project utilizes the song "For the Damaged Coda" by the Blonde Redhead. It is referred to as "For the Damaged Coda.wav" in the src file. You should probably include a WAV file or that exact song with that file name in your copy.

In addition, the following game files from UNDERTALE were utilized as sound effects, which can be found at [this repository](https://github.com/anotherhuman123456/undertale) in the sound/audio/ directory. Keep in mind I may have renamed some of the files, so some of the listed names aren't what you're looking for, gl:

- mus_f_laugh.wav
- snd_damage.wav
- snd_floweylaugh.wav
- snd_floweytalk1.wav
- snd_floweytalk2.wav
- snd_hurt1.wav
- snd_mtt1.wav
- snd_mtt2.wav
- snd_mtt3.wav
- snd_mtt4.wav
- snd_mtt5.wav
- snd_mtt6.wav
- snd_mtt7.wav
- snd_mtt8.wav
- snd_mtt9.wav
- snd_screenshake.wav

The files below are renamed versions of certain files found in the mentioned Undertale repository, and are included in the audio directory:
- char_text.wav
- menu_navigate.wav
- menu_select.wav
- menu_text.wav
- slash.wav
- start.wav

<div align=center>

Copyright (c) 2025 Jordan Myczka. All rights reserved.
<!--<img src="https://selinaalbaenglishsubjectmatterwriting.poetry.blog/wp-content/uploads/2020/03/148-1482736_flowey-sprite-flowey-undertale-1.jpg?w=820"/>-->

</div>
