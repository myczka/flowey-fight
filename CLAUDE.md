# FloweyFight: instructions for Claude

A C++17 console RPG fight against Flowey. It is a **learning project**: the goal is to understand conventional game/engine structure that transfers to Unity, Unreal, or a custom engine.

## How to help
- **Teach and review; don't implement** unless explicitly asked. Explain the concept, point at files/lines, suggest an order, review the user's attempts, fill gaps when asked.
- **Read `docs/REFACTOR_PLAN.md` before giving plan or architecture advice.** It is the source of truth (done/remaining steps, engine build-vs-buy table, game systems, frame loop, roadmap). When a decision changes the plan, update that file.
- **`docs/DESIGN_NOTES.md`** records the reasoning behind the plan (architecture lessons, menu tree, layout conventions, audio, dialogue) and the sources found. Read it when a topic it covers comes up.
- **Search before answering about conventions or APIs.** Use web search for game-dev conventions, standard implementations and how real engines/games do things, and the `ctx7` docs CLI (see the user's context7 rule) for library/API documentation such as miniaudio, nlohmann/json, CMake. Cite sources, and say plainly when a source was thin or a claim is from general knowledge.

## Architecture rules
- Dependencies point one way: `src/game` → `src/engine` → `third-party`. Engine code never names Flowey, Chara, `.wav` files or game rules. Sibling engine modules (console, audio, util) don't include each other sideways; shared helpers go in `util`.
- Data over code: abilities, monsters, items and dialogue are tables/files; code interprets them. Dialogue's end goal is external JSON (nlohmann/json, never a hand-written parser) played by a dialogue runner.
- `MINIAUDIO_IMPLEMENTATION` is defined only in `third-party/miniaudio_impl.cpp`. Headers contain declarations only (plus tiny inline members); no variable definitions in headers.
- No exceptions for expected failures (parsing uses `std::from_chars`, not `stoi`); report errors, don't silently `exit(0)`.

## Conventions
- camelCase functions and variables, PascalCase types, lowercase file names, one `.h`/`.cpp` pair per module, `#pragma once`.
- Format with the repo `.clang-format` (attached braces, left-bound `int* p`, no include sorting). Never format `third-party/`.
- Include what you use; heavy includes go in the `.cpp`.

## Build and git
- Configure: `cmake -S . -B build` (rerun after editing `CMakeLists.txt`). Build: `cmake --build build`. Every new `.cpp` must be added to `add_executable`.
- Work on branch `modularize`; commit only green builds, one step per commit.
- Rename files with `git mv` (`core.ignorecase` is true, so renames done in the editor are not tracked).
- Some `.wav` files are git-ignored (copyrighted samples), so a fresh clone is missing audio.
