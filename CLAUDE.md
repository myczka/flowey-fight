# FloweyFight: instructions for Claude

A C++17 console RPG fight against Flowey and a **learning project**. Help the user understand conventional, reusable game structure that transfers to Unity, Unreal, or another custom engine.

## How to help

- **Teach and review; implement only when explicitly asked.** Explain responsibilities and how components cooperate, point to relevant files/lines, suggest the next manageable step, and review the user's attempts. Avoid large unsolicited rewrites.
- Judge recommendations against the four goals in `docs/REFACTOR_PLAN.md` section 0: scalability, transferability, conventionality, and sustainability. Supported content should be data-driven; genuinely new mechanics may need localized code changes. Do not promise zero rewrites or claim one universal AAA/JRPG architecture.
- **Read `docs/REFACTOR_PLAN.md` before architecture advice.** It is the source of truth for ownership, behavior, scope, and implementation order. Section 9's **M0–M8** milestones are the sole roadmap; the old R1–R9 sequence is historical.
- Read `docs/DESIGN_NOTES.md` for explanations and primary sources. Keep both documents consistent when changing an architectural decision; keep this file a concise guide rather than duplicating their details.
- Use the current files in `docs/` as authority; `docs_old/` is historical. Valyria Tear is a JRPG reference and inspiration, not a requirement to reproduce its implementation. Preserve the user's own game style and the current small engine organization; explain one manageable responsibility at a time.
- Search primary sources before making new claims about conventions or APIs. Use the user's context7 workflow when available; otherwise use official documentation and identify the fallback. Check signatures against the **vendored version**, cite evidence, and distinguish documented behavior from project-specific choices or uncertain claims.
- Verify repository status, source, and a build before reporting completed work or a broken baseline. Old notes, TODO line numbers, and reported branch/build state are not fresh evidence.

## Architecture boundaries

- `src/game` may use `src/engine` and required third-party libraries; engine code never includes game headers or hard-codes character names, game asset paths, or combat rules. Engine modules may depend on each other when the dependency graph stays **acyclic**. Do not put unrelated code into `util` to avoid a legitimate dependency.
- Game owns services, content, session state, and modes. Pass narrow dependencies to consumers. Establish construction/destruction order explicitly; avoid mutable globals and initialization that reads other file-scope game objects.
- AudioManager owns one engine, stable sound resources, and the initial Music/Sfx/Voice groups. Speaker borrows AudioManager and Console. Game content supplies voice profiles and resolves authored sound names to typed SoundIds. Preserve safe muted behavior when audio is unavailable.
- Keep authored definitions separate from runtime instances: MonsterType versus Monster, StatusDefinition versus ActiveStatus, and AbilityDefinition versus BattleAction. Freeze validated content during play; typed IDs still require valid ownership and lifetime.
- BattleManager coordinates **validation → cast presentation → one rules commit → result presentation → outcome/turn progression**. Rule functions have explicit inputs and no audio, console, XML, sleep, or dialogue calls. Player input and AI produce the same BattleAction representation; no subclass per spell is required.
- DialogueRunner executes reusable sequences; Speaker presents lines. Dialogue is a graph; nested command menus are trees. Reuse menu navigation for choices without forcing both into one data model. Menus return selection tokens; game code interprets them.
- Route input to one active consumer and consume each event once. Follow the plan's update order: Game updates DialogueRunner, which advances Speaker, then the active mode. Do not also update Speaker separately or replay Confirm during catch-up. Apply mode changes after callbacks return.
- Final gameplay paths are nonblocking. Explicit presentation stages may suspend battle progression while dialogue, rendering, and audio continue. Use the plan's bounded timing policy; keep blocking adapters temporary and localized during migration.
- Data files are external XML parsed with pugixml. The game loader converts them into owned runtime definitions and validates numbers, IDs, references, commands, and mixed-content whitespace. Do not hand-write an XML parser or maintain a second required authoring format after migration.

## C++ and error handling

- Keep `MINIAUDIO_IMPLEMENTATION` only in `third-party/miniaudio_impl.cpp` for the current vendored distribution. Preserve initialized miniaudio object addresses; do not copy/move resource owners accidentally.
- Ordinary non-inline function implementations belong in `.cpp` files. Headers may contain type/template definitions and appropriate inline/constexpr definitions, including valid C++17 inline variables. Avoid mutable global state.
- Expected failures use explicit status/results and useful diagnostics. Parse numbers with `std::from_chars`, checking full consumption and range. Do not silently `exit(0)` or assume successful XML parsing means valid game content. This policy does not require disabling all C++ exceptions.
- Use camelCase functions/variables, PascalCase types, lowercase file names, and `#pragma once`. Keep headers beside implementations; do not create empty `.cpp` files for data-only headers.
- Follow the repository's `.clang-format` (attached braces, left-bound pointers, no include sorting). Never format third-party code. Include what each file uses; keep implementation-only heavy includes in `.cpp` files.

## Build, verification, and git

- Configure: `cmake -S . -B build`. Build: `cmake --build build`. Reconfigure after build-configuration changes; add each source to its appropriate target, not automatically to `add_executable`.
- Follow the plan's small headless rules/test target when tests are introduced. Run `ctest --test-dir build --output-on-failure`, adding the configuration option for multi-config builds when needed. Rules tests must not require a terminal or audio device.
- Work on `modularize` after checking the actual branch/worktree. Keep commits focused and green; preserve unrelated user changes. Do not reset history, remove remotes, or delete legacy tooling without verifying the need and applicable authorization.
- Prefer `git mv` for explicit renames, particularly case-only renames on case-insensitive filesystems. Do not assume editor renames are universally untracked.
- Some audio was reported git-ignored; inspect actual assets and document setup/fallback behavior. Never claim a fresh clone has all sounds without checking.
- **Before the next build or play-test, remind the user to recheck the missing audio recorded in `docs/REFACTOR_PLAN.md` section 1.1.1.** On 2026-11-19, six active references were missing, including startup's `menu_select.wav`; the current loader exits on failure. This affects runtime, not compilation. Recheck before repeating the warning, and retire it once assets/fallback are verified.
- Each milestone needs its playable checkpoint, not merely new classes/files. Preserve behavior during refactoring; make balance changes separately. Stop at the completion target and XML-only extension test before expanding into optional engine features.
