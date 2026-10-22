# FloweyFight: Refactor and Engine Plan

Written for: you, as a learning roadmap. The goal is conventional, transferable structure (Unity/Unreal concepts), not a finished product. On approval this gets saved to `docs/REFACTOR_PLAN.md`.

## 1. Context
`src/main.cpp` (formerly `flowey.cpp`) was a 750-line monolith: globals, `goto`, story script, battle logic and audio wiring in one file. The goal is to split it into a reusable **engine** layer (console, audio, text, menus, state machine) and **game** code (combatants, abilities, battle, story), so that each concept maps onto a pattern you will meet in Unity or Unreal.

Rules that guide every step below:
- **Dependencies point one way:** `game` → `engine` → `third-party`. Engine code never mentions Flowey, Chara, `.wav` names, or game rules.
- **Data over code:** abilities, monsters, dialogue and items are tables or files; code is the interpreter.
- **Each step ends with a green build and a commit** on `modularize`.

## 2. Done so far (verified in the repo)
- History reset to one clean root commit; work is on branch `modularize`, pushed without the local `backup-old-history` branch.
- `flowey.cpp` renamed to `main.cpp`; CMake lists sources in a single `add_executable`.
- Folder layout: `src/engine/`, `third-party/` (miniaudio), `.clang-format` (LLVM base, 4 spaces, attached braces, left-bound pointers, no include sorting), whole tree formatted.
- `console` split into `.h`/`.cpp` (header is declarations only).
- `third-party/miniaudio_impl.cpp` holds the single `MINIAUDIO_IMPLEMENTATION`.
- `audio.h`/`audio.cpp` split from `myaudio.h`; `reset()` removed (it used a private miniaudio function).
- `util.h/.cpp` created (`getRand` now seeds one `static thread_local mt19937` once; `sleep`).
- `speaker.h/.cpp` drafted (`Voice` data + `Speaker` borrowing a `ma_engine&`).
- Uncommitted but sensible: `waitTill` moved into `audio` (now uses `util`); `Menu.h` includes `audio.h` instead of `miniaudio.h`; `console.cpp` no longer defines `sleep`/`getRand`.

**In progress / not green:** the working tree is uncommitted. `main.cpp` still calls `write(...)`, `playRandSnd` and `num_enter` that no longer exist, so the build is broken until R1 and R2 below.

TODO references below use current `main.cpp` line numbers. `TODO.md` is regenerated only on CMake configure, so its numbers are stale.

## 3. Remaining refactor, in order
| # | Step | Notes |
|---|---|---|
| R1 | Finish `Speaker` and wire `main.cpp` | `explicit Speaker(ma_engine&)`, delete default ctor/`getEngine`/`setEngine`, add `<string>`/`<vector>`, `const&` params, guard empty vector, rename `speed` to `delayMs`; replace `flow`/`wolf`/`chara`/`type`/`mett` with `Speaker::write`/`writeRand`. Add to CMake (done). |
| R2 | Delete `consoleaudio.h` | `ok()` becomes `pressContinue('z')` plus `play(ok_sound)` (note the old function printed `>` and played the sound before `clear()`). |
| R3 | Remove `audioDir` global | First `setAudioRoot(path)` inside `audio.cpp`; drop the `bin` flag. Later it becomes an `AudioManager` field. |
| R4 | Header hygiene | Remove duplicate `sleep`/`getRand` declarations from `console.h`; every header includes only what its declarations need; add `.gitattributes` (`* text=auto`). |
| R5 | `AudioManager` | One `ma_engine`, named sound groups (`music`, `effects`, `voice`, `loudVoice`) via `ma_sound_group` (confirmed present in your `miniaudio.h`), preloaded sound cache. Replaces the four engines in `main.cpp` and gives the "Mute mainEng" TODO (line 415) a home (`setGroupVolume(music, 0)`). **Report missing files** instead of today's silent `exit(0)` in `initSound`; several `.wav` files are git-ignored, so a fresh clone fails silently right now. Speaker then holds a group pointer. |
| R6 | Menu tree | Plain-data `MenuNode` (label, children, `std::function` action on leaves) plus a navigator that owns `select()`, cursor index and back-stack. Reuse `Menu.h`'s `display` and `readKey` code. Remove the per-`Menu` `ma_engine` (Menu.h lines 60 and 84). Dynamic submenus (items, spells by mana) via a generator callback. Stop building menus as **file-scope globals** (`main.cpp` lines ~100-107): `battle_menu` calls `getBattleStr()` during static initialization, which only works because everything is in one file. Build menus inside the battle setup instead. |
| R7 | `main` → `game` split | Create `src/game/`; replace globals and `goto` (see sections 5-6); replace `srand(time(0))`/`rand()` in `flowey_turn` with `getRand`; turn the command-line `argv` items into `ItemType` values; `main.cpp` becomes `Game g; g.run();`. |
| R8 | Cleanup | Retire the stale subtree: delete `pullsubtree`/`pushsubtree`, `git remote remove cppLibrary`, remove `lib/`; update `README.md` (still describes the subtree); make `gettodos` scan recursively (it only reads `src/*.cpp`). |
| R9 | Frame-based game loop | Replace blocking `sleep`/`_getch` flow with a fixed-timestep loop (input, update, render each frame). See section 4.1. Do it **before** the battle rewrite so `BattleManager` is written as per-frame states once, not twice. |

## 4. Engine completeness: what to build, adopt, or skip
The engine is "complete enough" when each subsystem below has a small, replaceable interface. Layering follows the usual *Game Engine Architecture* stack (platform layer, core systems, resource manager, audio, game-specific systems) as summarized in [lecture notes based on Gregory's book](https://cs.utexas.edu/~theshark/courses/cs354p/lectures/cs354p-2.pdf).

| Subsystem | Decision | Why |
|---|---|---|
| Console I/O (`console`, `input`) | **Build** (thin wrapper) | Small, and it is your platform-independence layer. Add an `input` module so the `Z`/`X` keys live in one place (currently hard-coded in `Menu::readKey`). |
| Time/util (`sleep`, RNG) | **Build** (done) | A few lines; RNG itself stays `<random>`. |
| Audio playback | **Adopt** miniaudio (done); **build** `AudioManager` wrapper | Never write decoding or mixing. The wrapper (groups, cache, volume) is the manager pattern worth practicing. |
| Text/dialogue presentation | **Build** (`Speaker`) | Core to your game and a good illustration of "data (`Voice`) vs mechanism (`Speaker`)". |
| Dialogue data and runner | **Build the runner, adopt a JSON library** | Keep the schema small (speaker, text, pause, next, choices, conditions). Use [nlohmann/json](https://json.nlohmann.me/integration/cmake): a single header with FetchContent or a vendored `third-party/json.hpp`. Do not write a JSON parser. |
| Menu/UI | **Build** (tree + navigator) | A good data-structures exercise. [FTXUI](https://alternativeto.net/software/ftxui) is an option for a full TUI, but it would replace your hand-rolled rendering, so skip it unless you want a TUI rewrite. |
| State/scene machine | **Build** | Pure pattern practice (Intro, NameEntry, Battle, Ending); maps to Unity scenes and Unreal levels/game modes. |
| Logging/assert | **Build tiny** (`LOG(...)` macro) | spdlog is fine later, but overkill now. |
| Unit tests | **Adopt** doctest or Catch2 (header-only) | Test the damage formula, stat modifiers and parser without launching the game. I did not find a doctest-vs-Catch2 comparison; either works with CMake `FetchContent`. |
| Dependency management | **FetchContent or vendored headers** | For a few small dependencies, [FetchContent is the lighter choice vs vcpkg/conan](https://discourse.cmake.org/t/fetchcontent-vs-vcpkg-conan/6578); keep miniaudio vendored in `third-party/`. |
| ECS, scripting (Lua), GUI, networking | **Skip** | No problem here that needs them. EnTT/sol2 exist if you later outgrow plain structs. |
| Game loop / timing | **Build** | See 4.1. This is the core of every engine and the main difference from your current blocking design. |

### 4.1 Frame-based game loop (replaces blocking timing)
Today the program *is* its timeline: `sleep`, `_getch` and typewriter loops block, so nothing else can run and errors in each `sleep` add up. Engines instead run one loop forever and let every system advance a little each frame (see the Game Loop and Update Method chapters of *Game Programming Patterns*, and Glenn Fiedler's "Fix Your Timestep").

```
const double dt = 1.0 / 60.0;                  // fixed step: 60 updates per second
auto next = steady_clock::now();
while (running) {
    input.poll();                              // non-blocking: _kbhit/_getch only if a key waits
    stateMachine.update(dt);                   // current state advances by dt
    renderer.present();                        // draw only what changed
    next += 16.667ms;                          // schedule against the clock, not "sleep 16"
    std::this_thread::sleep_until(next);       // late frames don't accumulate drift
}
```

What has to change, in order:
1. **`input` module polls** instead of waiting: `keyPressed('z')` is true on the frame the key goes down. `waitForInput` becomes a state that checks this every frame. `Menu::readKey` (blocking `ReadConsoleInput`) becomes polling too.
2. **Typewriter becomes an object with an update**: it keeps `elapsed` and `charIndex`; each `update(dt)` adds `dt`, and while `elapsed >= delay` it prints one character, plays its blip, and subtracts `delay`. `isDone()` says when the line finished. Pressing a key mid-line can now skip to the end, which the blocking version cannot do.
3. **Waits become timers**: `sleep(500)` in a script becomes a "wait 0.5 s" step that counts down in `update`.
4. **Scripted sequences become step lists** (`say`, `wait`, `playSound`, `clear`, `waitForConfirm`) run by a small sequencer, one step at a time. This is also the dialogue runner from section 4, so the intro and endings move there.
5. **State machine** (Intro, NameEntry, Battle, Ending) owns which update runs each frame; `BattleManager` phases are states too.
6. **Rendering:** the console has no frame buffer, so print only when something changes (new character, menu cursor moved), and flush `std::cout` after printing.

Name entry (`getline`) is the one remaining blocking call; keep it as a blocking state at first, then replace it with per-key text input when the input module supports it.

## 5. Game systems: Player and Monster

### 5.1 Combatant data (composition, not a deep hierarchy)
```
Stats      { hp, maxHp, mp, maxMp, attack, defense, magic, magicDefense, speed }
Combatant  { name, Stats base, std::vector<StatusEffect> statuses,
             std::vector<AbilityId> abilities, const Voice* voice }   // plain public struct
Player     : Combatant + inventory, equipment, limit/super gauge
Monster    : Combatant + const MonsterType* type
```
`MonsterType` follows the [Type Object pattern](https://www.gameprogrammingpatterns.com/type-object.html): base stats, ability list, attack weights and voices are **data loaded from a file**, and a `Monster` is an instance pointing at its type. Adding a monster means adding data, not a class.

Functions to put on `Combatant` (or free functions beside it): `takeDamage`, `heal`, `spendMp`/`canAfford`, `effectiveStat(stat)` (base plus modifiers), `addStatus`, `tickStatuses`, `isAlive`. These replace `get_health`, `get_mana` (TODO line 137), the `health`/`flowey`/`damageMod`/`mana` globals, and the `defend` flag.

### 5.2 Ability (spell) system, data-driven
One struct covers spells, specials and limit breaks (this follows the data-driven pattern seen in [Evennia's turn-battle magic module](https://www.evennia.com/docs/6.x/api/evennia.contrib.game_systems.turnbattle.tb_magic.html), where one function is reused for many spells with different parameters):
```
Ability { id, name, cost{kind: MP% | Gauge | None, amount},
          target: Self | Enemy | AllEnemies | Ally,
          element, power, effects[], castLine, reactionLine, sfx }
Effect  { kind: Damage | Heal | ApplyStatus | DrainMp | SkipTurns, magnitude, duration }
```
- **Resolver:** one `useAbility(caster, target, ability)` replaces the per-spell `switch` blocks (TODO lines 236-237). It checks `canAfford`, spends the cost, runs the cast line and sfx, applies each effect, then the reaction line.
- **Damage:** Final Fantasy-style spell power is a percentage of the caster's base magic damage (the [FF7 tables](https://guides.gamercorner.net/ffvii/tables/spells:materiaId.21) list entries as multiples of that base). Start with `damage = max(1, power * effectiveStat(magic) * elementMultiplier - magicDefense)`. Typical element multipliers are weak 1.5-2x, resist 0.5x, immune 0, absorb heals (sources vary, so choose your own table and keep it in data).
- **Mana as percent:** keep your UI (Bolt 10%, Blizzara 20%, Firaga 25%): cost = `amount` percent of `maxMp`.
- **Balance (TODO line 226):** put numbers in the table and test with doctest ("N casts of Bolt kills Flowey").

### 5.3 Your special schools (Buster / Limit / Chaos vectors)
Each of `sptypeBuster`, `sptypeLimit`, `sptypeChaos` becomes a **loadout**: a list of ability IDs with unlock conditions.
- Super arts and limit breaks are abilities with `cost.kind = Gauge`.
- **Chaos boost** (all attacks stronger but costlier, inhibitor rings) is a persistent status with modifiers, not special-case code.
- **Chaos Control** ("pause Flowey two turns") is a `SkipTurns` effect checked by the turn scheduler.

### 5.4 Status and buff system
A buff is a timed object attached to a combatant that modifies values or triggers code, a common RPG pattern (see [Evennia's buffs contrib](https://www.evennia.com/docs/2.x/Contribs/Contrib-Buffs.html)):
```
StatModifier  { stat, op: Add | Multiply, value }
StatusEffect  { id, durationTurns, stacking: Refresh | Stack | Replace, modifiers[] }
```
Stats are computed on demand as base + modifiers, never overwritten. This turns the Candy Bar's permanent `damageMod += 150` into a modifier. Store percentages as floats to avoid integer truncation. The same concepts are Unity ScriptableObjects and Unreal's Gameplay Ability System (from general knowledge; not verified in this search).

### 5.5 Battle flow
- `BattleManager` runs phases: PlayerChoose, Resolve, EnemyChoose, Resolve, EndOfTurn, WinCheck.
- Player actions and enemy attacks are **Command objects** (`execute()`); a turn queue ordered by `speed` makes ATB or party battles possible later without a rewrite. (Sources I found on this were thin, so rely on the Command chapter of *Game Programming Patterns*.)
- **Flowey's AI:** replace the `random_number_three <= 33 / 53 / 69 / 75` chain with a weighted attack table (name, weight, damage, blockable). The `defend` check duplicated across attacks (TODO line 362) becomes one function.
- **Undertale-style attack timing (TODO line 232):** isolate behind a function returning accuracy 0..1; implement later as a thread/atomics exercise without touching battle logic.

### 5.6 Story and dialogue
The `Checkpoint { NOSTALGIA, BETRAYAL, WHY, MAKEUP, DEATH, SPARE } loreState` enum already hints at paths. Move dialogue to data keyed by ID with conditions (path, HP, turn); game code only triggers `startDialogue("intro_chara")`. This resolves TODO lines 450 (more lore), 312 (spare/lore dialogue) and 638 (Flowey-dead dialogue). The "Redundant - used once" TODO (line 217) disappears when `player_turn` becomes a `BattleManager` phase.

## 6. Target layout
```
FloweyFight/
  CMakeLists.txt  .clang-format  .gitattributes  README.md  TODO.md
  assets/audio/                 (currently audio/)   data/*.json (abilities, monsters, dialogue)
  docs/REFACTOR_PLAN.md
  third-party/  miniaudio.h  miniaudio_impl.cpp  (json.hpp)
  src/
    main.cpp                    (thin)
    engine/  console  input  util  audio(+AudioManager)  speaker  menu_tree  state_machine  log
    game/    combatant  stats  status  ability  abilities_data  battle  items  voices  story  game
  tests/    (doctest) stats, damage, status, parser
```
Each module is one `.h`/`.cpp` pair listed in CMake; the `engine` code stays game-agnostic.

## 7. Roadmap
1. **Green build:** R1-R4 (Speaker, delete `consoleaudio`, `audioDir`, headers). Commit.
2. **Engine:** R5 `AudioManager`, add polling `input`, R9 frame loop and state machine skeleton (port the typewriter first, then the intro as a step list), then R6 menu tree written as a per-frame navigator.
3. **Data model:** `Stats`, `Combatant`, `Player`, `Monster`/`MonsterType`; replace globals. Add doctest with first stat tests.
4. **Abilities:** `Ability`/`Effect` structs, `useAbility`, spell table, then status/modifier system; migrate Bolt/Blizzara/Firaga, then the Buster/Limit/Chaos loadouts.
5. **Battle:** `BattleManager`, commands, weighted Flowey AI; remove `goto` and the last globals; slim `main.cpp`.
6. **Content:** JSON loader and dialogue runner; port the intro, Chara and empty-name paths.
7. **Later (optional):** attack-timing minigame (with the frame loop it no longer needs threads; threads stay an optional exercise), FTXUI if you want a TUI.

## 8. Verification
- Build after every step: `cmake --build build` (reconfigure after editing `CMakeLists.txt`); commit only green states.
- Play-test at checkpoints: name entry paths (normal, `Chara`, empty), a full fight, defend, each item, each spell, flee, win and lose.
- Unit tests (doctest) once `Stats`/`Ability` exist: damage formula, element multipliers, cost spending, modifier stacking, JSON schema validation.
- Housekeeping checks: `git status` clean, no duplicate `MINIAUDIO_IMPLEMENTATION`, no `game/` includes inside `engine/`.

## 9. On approval (my actions only, no code implementation)
You implement the plan yourself; I review and fill gaps when asked. On approval I will only:
1. Save this plan as `docs/REFACTOR_PLAN.md` (uncommitted, so you choose when to commit it).
2. Copy the old history to a separate local folder: `git clone --branch backup-old-history --single-branch FloweyFight ../FloweyFight-old-history`, then `git remote remove origin` inside the copy so it can never be pushed anywhere. It is a full local repo with the old commits and code. Git-ignored `.wav` files are not included. Nothing in `FloweyFight` changes; you delete `backup-old-history` yourself later with `git branch -D backup-old-history`.

## 10. Confidence notes
Verified in the repo: current layout, build setup, miniaudio sound-group API, TODO lines. Verified by search: nlohmann/json integration, FetchContent vs vcpkg tradeoffs, Type Object chapter, FF7 spell values as base-magic multiples, buff-pattern descriptions. **Thin or from general knowledge:** exact Final Fantasy damage formulas and element multipliers (sources conflict; treat as tunable data), Command-pattern battle architecture, Unreal GAS and Unity ScriptableObject mappings, PDCurses/ncurses/FTXUI comparisons. Check those against official docs before relying on them.
