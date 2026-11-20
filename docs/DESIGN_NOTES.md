# FloweyFight: Design Notes

Revision: 2026-11-19 (repository audit and learning-goal clarification; architecture revision originally 2026-10-25). Companion to [REFACTOR_PLAN.md](REFACTOR_PLAN.md).

The plan is the source of truth for decisions and milestone order. This file explains how the pieces cooperate, why the boundaries exist, and which claims are supported by sources. The older notes mixed current choices, abandoned alternatives, and unverified conventions; this revision resolves those conflicts while retaining useful learning notes.

Sections describing Game, AudioManager, and DialogueRunner explain the target design, not currently implemented systems. The inspected source/build baseline and missing-audio reminder live in REFACTOR_PLAN sections 1.1–1.1.1. The current source has a working compile, blocking Speaker/menu code, and an unused AudioManager header draft. Use `docs_old/` only for historical context; the duplicate `_gpt.md` downloads contained the same text as the active documents and were removed.

## 1. Read the architecture through one play session

### Startup: create services, then load the game

`main` starts Game. Game sets up the terminal/input backend and AudioManager. The game loader reads XML, builds definitions, resolves authored names to typed IDs, and reports problems before publishing ContentDatabase. The loader prepares sound objects and produces voice profiles. Game then connects the dialogue runner's generic handlers to StoryState and requests the first mode.

This order removes dependence on file-scope initialization. A global menu that calls a function reading battle state during startup is fragile because its required objects may not exist yet. A Game-owned setup procedure makes the order visible. It also gives failures a place to return to rather than printing a message and continuing with half-initialized resources.

Audio is optional to successful gameplay: missing samples or an unavailable device can yield silence with diagnostics. Invalid ability targets or missing dialogue destinations make the game content unusable; startup should reject those definitions. These are separate policies, even though both arise during loading.

### Intro: authored story uses generic presentation

Game's `playDialogue("intro_flowey")` looks up a DialogueSequence. The runner begins its first line and asks Speaker to present the segments with the resolved voice. Speaker reveals text during updates. After the line completes, the runner waits for confirmation or moves to the next step according to that sequence's advance policy.

The runner does not know who Flowey is. The game loader knows the authored speaker name; it resolves that name to a profile containing blip SoundIds and timing/gain defaults. Likewise, a name-entry step returns text to a registered game handler. The handler updates StoryState; the runner does not include `player.h`.

When a script requests Battle, the handler queues a mode transition. Game applies it after the current update returns, stops the old sequence, releases its input ownership, and enters Battle. Destroying the caller from inside its own callback is avoided through that deferred transition.

### Battle: rules and presentation meet at explicit stages

BattleManager owns runtime combatants and one pending BattleAction. It builds a menu from legal action data. MenuNavigator returns a selection token. The game maps the token to the action; the menu itself does not know about MP or enemies.

BattleManager first validates, then presents casting, then commits the rules once, then presents the result. The result stores actual damage/heal/status outcomes so text and HP animation do not recompute them. Once presentation finishes, the manager schedules the next turn or ends the battle. A terminal result is detected at the commit and cannot schedule a defeated actor while its defeat line is playing.

On shutdown, stop modes and callbacks before their state disappears, then release presentations/content borrowers and audio resources, then restore the terminal. Ownership is useful because it defines this order, not merely because globals are unfashionable.

## 2. What modularization changes

The old monolith mixed state, rule decisions, timed presentation, authored content, and application flow. Split by these reasons to change. Changing Bolt's cost should change a definition. Changing the damage calculation should change a rule function. Changing how a line types should change Speaker. Changing which ending plays should change story data or a story decision.

Folders do not enforce this separation. Include direction, public APIs, ownership, and eventually build targets do. An engine module may depend on another engine module: a typewriter using audio is a natural dependency. The problematic cases are cycles or a low-level service knowing its game's rules.

For example, `Speaker` can include the audio wrapper's handle/API declarations. AudioManager never includes Speaker or Combatant. A game presentation adapter includes battle results and presentation services; the pure damage helper includes neither. Moving every shared declaration into `util.h` would hide these relationships rather than improve them.

The same reasoning applies to file count. Plain structs and small inline functions can live entirely in a header. A `.h`/`.cpp` pair is a useful default for implementations, not a requirement to create an empty source file per concept.

### C++ corrections carried forward

- Ordinary non-inline external function definitions in a commonly included header can violate the one-definition rule. Templates, class definitions, and valid inline definitions are normal header content; “headers contain declarations only” was too absolute. See N13.
- An implementation macro belongs in exactly the translation unit required by that library's distribution. miniaudio/stb-style switches do not imply that every single-header library needs such a switch; nlohmann/json is not used that way.
- A reference member can express borrowing correctly. It normally prevents default assignment, which matters for a plain data value you want to assign. This does not forbid building that object from data. Use references for long-lived non-owning service dependencies when lifetime is controlled; use values/IDs for copyable content.
- Use `enum class`, end the declaration with `};`, and convert to an index explicitly in one helper where needed. Prefer `Music`/`Count` over macro-like all-caps names.
- Keep mutable application objects out of namespace-scope initialization. Immutable compile-time tables are a different case.
- Check actual rename behavior; `git mv` makes intent clear, particularly for case-only renames on case-insensitive filesystems. Editor renames are not universally invisible to Git.

## 3. Definitions, instances, and IDs

A definition answers “what kind of thing is this?” An instance answers “what is happening to this particular thing right now?”

| Shared definition | Runtime instance |
|---|---|
| MonsterType: authored HP, ability list, weighted AI | Monster's current HP, pending action, active statuses |
| AbilityDefinition: cost, effects, target rule | BattleAction: caster, chosen ability, selected targets |
| StatusDefinition: modifiers, stacking/expiry policy | ActiveStatus: source, stacks, remaining opportunities |
| DialogueSequence: nodes and authored lines | DialogueRunner's current node, timer, choice/input progress |
| Voice profile: samples and timing defaults | Speaker's line index and currently playing blip |

This split allows several monsters to share a definition without sharing damage or status timers. It also avoids rewriting authored base stats whenever an instance is buffed. The Type Object pattern is a documented example of moving supported type variation into data [N4]; it does not remove the need to implement new behavior.

Use typed IDs to reference frozen definitions. Distinguish authored string names from runtime handles. For example, XML uses `bolt`; loading resolves it to an AbilityId. It is fine for those handles to differ after the next launch. A future save file should preserve stable authored IDs and reconstruct handles on load.

An ID is not automatic lifetime safety. For this project the rule is simple: no unload/reload during play, and the owning database outlives every ID consumer. Validate IDs before use. If unloading is added later, reconsider stale-handle detection rather than assuming indexes remain safe.

## 4. Four kinds of progress, each with one owner

| Progress being tracked | Owner | Example |
|---|---|---|
| Which major part of the application is active? | ModeController | Intro → Battle → GameOver |
| Which part of a turn/action is active? | BattleManager | ChooseAction → PresentCast → CommitAction |
| Which authored step is active? | DialogueRunner | Line → Wait → Choice → Jump |
| How much of one line is visible? | Speaker | Segment 2, character 7, next reveal due |

These are separate state machines because they answer different questions. They do not all need polymorphic State subclasses. An enum and `switch` is an adequate initial representation for a small fixed set of phases.

“Dialogue is a system” means there is one reusable runner rather than one C++ class per cutscene. It does not mean battle is forbidden from being in a waiting state. A runner can be active while the battle waits in PresentCast, and its own internals necessarily track progress. The old either/or distinction is retired.

A modal state stack is a valid later tool for nested pause/inventory screens [N5]. Do not add it until resuming nested interaction becomes a real problem. A graph-driven dialogue runner, an application state machine, and a stack solve related but different flow problems.

### Who updates during dialogue?

Game keeps the services and active presentation updating. BattleManager waits for a particular presentation request to finish; it does not stop the whole loop. Input is routed to the active dialogue interaction rather than the underlying battle menu. A later overworld can freeze movement while keeping ambient effects alive.

There must be only one update path to Speaker. The chosen order is Game → DialogueRunner (which advances Speaker) and then Game → current mode/BattleManager. The mode can observe a completed run and start the next one, which advances on a following step. Game must not separately call Speaker again. This small ordering rule prevents a line typing twice as fast merely because two owners both thought they should update it.

Start with one active dialogue sequence. A start request while busy returns Busy. Completion, cancellation, and failure all have a defined result. This makes an action's waiting stage understandable and prevents an invisible replacement sequence from leaving it waiting forever.

## 5. Choosing an action is different from resolving it

The player and AI should both produce a BattleAction. The action says what was chosen; the resolver applies the rules; the presentation says what the player sees and hears. The Command chapter explicitly uses a common action boundary for input and AI [N3]. Our initial representation can be a struct, not a class hierarchy.

The resolver should be callable from a headless test. Pass battle state, definitions, targets, and a controlled RNG explicitly. It returns a structured result and updates combat state only on successful commit. Do not make a unit test wait through “Flowey casts…” to discover whether MP was spent correctly.

Pure validation alone is not enough: validate again at commit, before any deductions. A stale menu selection or changed target must not cause partial mutation. For a valid action, ordered effects have explicitly defined behavior; skipped effects on a dead target are a rule outcome, not an excuse to leave costs half-applied. Expected malformed definitions should have been rejected at load time.

### Timing and cancellation

Casting and reactions are nonblocking presentations surrounding the commit. If the player reveals a line early, only the presentation changes. If the whole battle is aborted before commit, no gameplay cost/effect has happened. After commit, presentation cancellation does not refund or repeat the action. Exit/error handling discards pending UI safely; it never rolls forward a half-finished action accidentally.

The same lifecycle covers actions other than spells: an item checks its quantity and consumes it at commit; Defend applies protection; Flee yields an escape result according to the existing rules. Once the manager reports victory, defeat, or escape, the owning game mode chooses the story sequence. This keeps ending selection in game code while the reusable runner only presents the chosen content.

For an attack minigame, collect the accuracy before commit. No combat mutex is required when its input/timing and the battle stage are on the same thread. Audio may have its own internal callback thread; that does not require the game to become multithreaded.

### Numerical rules need names and examples

The old minimum-damage formula turned immunity into a one-point hit and could not represent absorption. The revised plan handles those outcomes explicitly. Its example formula is a design proposal, not an FF7 formula. The initial migration preserves legacy balance; tuning gets a separate change.

Percentage costs need one shared calculation. With a proposed ceiling policy, 10% of 37 maxMP costs 4 MP; the menu must show the same cost the resolver checks and spends. A 20% buff is multiplier 1.2, not an integer division that truncates to zero. Decide rounding/clamping at the rules boundary.

Status duration is similarly a rule, not just a field. “Two turns” must identify whose turns and the relevant hook. In the plan, ordinary durations count recipient opportunities after arming; skip effects count missed BeginTurn opportunities; defense has an explicit until-next-BeginTurn lifetime. A self-buff should not vanish at the end of the turn that created it. These are project choices to test, not a universal RPG standard.

## 6. Timing and input: make waits visible

The Game Loop and Update Method patterns replace a long blocking timeline with small updates [N1]. A timer or character index stores where the task resumes next frame. This is the important conceptual transfer; a fixed rate by itself does not make a loop responsive.

The revised plan retains a 60 Hz fixed update for the learning exercise, but measures elapsed time into an accumulator, limits catch-up, and separates rendering. It states what happens after a stall. This addresses the overload issue discussed by Fiedler [N2]. The initial limits are tunable policy; a console UI could also use measured variable elapsed time if that better fits a later version.

Input is polled once per outer frame and routed once. If five updates run to catch up, one Confirm must not execute five selections. Similarly, while revealing a line, Confirm should not reveal it and also dismiss it in the same event. Keep action presses, held/repeated navigation, and typed text as distinct input concepts.

Windows console records expose press/release and repeat information; an availability check is not equivalent to a fresh press [N11]. Keep this detail in Input's backend. Do not scatter raw `Z`, `X`, `_getch`, or `ReadConsoleInput` calls through battle, menus, and dialogue.

Console output is a presentation backend. Redrawing only dirty regions is a practical strategy, not a claim that terminals have no screen buffer or cannot support an application-side back buffer. Flush batches when visible output changes. `sleep_until` is pacing and can wake late; it is not a guarantee of hard real-time timing.

## 7. Audio ownership and the typewriter

The miniaudio manual explicitly requires initialized object addresses to remain stable. It also documents engine-created mixing groups and separate loading modes for short decoded sounds versus streams [N10]. These API facts support the wrapper; they do not prescribe our exact C++ class design.

AudioManager owns the resources; SoundIds identify them. Choose stable allocation for initialized sound objects and delete AudioManager's copy/move operations. Keep explicit initialization-success tracking so cleanup never uninitializes a resource that failed to initialize. If output is unavailable, the wrapper remains a valid muted service.

The game owns the names and paths passed into loading. Engine code may accept `assets/audio/foo.wav`; it must not hard-code Flowey's sound list. A string-keyed authored manifest and typed runtime handles solve different problems and can coexist. A handle improves API clarity; it does not make external asset names compile-time checked.

### How to approach the current AudioManager header

The existing `soundMap` and fixed group array are a useful ownership sketch. What is missing first is the owned engine, initialization/failure tracking, cleanup, and one working load/play path. Add that behavior in `audio_manager.cpp` before designing a large interface. A declared constructor and a successful app build do not validate this unused header.

For the initial design, the game translates authored names such as `flowey_blip` into SoundIds; the manager owns stable resources behind those IDs. These are two lookups at different boundaries, not competing claims that either all strings or all IDs are wrong. A vector of individually owned resources is enough initially. Keep the filename map only where a caller actually needs name resolution.

The manager owns device/resource lifetime; Speaker owns which blip is currently part of its line; game content owns what each voice sounds like. A group controls a category's volume; it is not a separate device or character. This division explains the members and methods without requiring a singleton, generic asset framework, or public miniaudio objects in every caller. REFACTOR_PLAN section 5.1 gives the incremental implementation order and failure checks.

### A voice is data, Speaker is the mechanism

Voice supplies blip samples, reveal delay, and pitch/gain defaults. Speaker supplies progress and playback coordination. A narrator can use no blips. Different lines can choose different profiles even for the same combatant. Combatants do not each own a Speaker or audio device.

One Speaker serves one active text channel in this version. If several simultaneous dialogue boxes become necessary, that is a new channel/lifetime requirement, not a reason to create one Speaker per character now.

With one sample, restarting its owned sound cuts off its previous instance. With several randomized samples, the previous sample must be stopped even when the next choice is a different handle. This closes an overlap hole in the previous notes. Set per-line playback parameters explicitly so a loud/high-pitched line does not accidentally change the next one.

The blip stream uses cosmetic randomness separate from the battle RNG. Revealing/skipping text should not change the enemy's next attack. This is valuable even before implementing replays.

### Preserve useful parser work without multiplying formats

The earlier pause-tag exercise can still teach scanning and `from_chars`: find the closing delimiter, require full numeric consumption and a sensible range, then produce a Pause segment. The 2026-11-19 source has only commented-out experimental parsing and leftover locals; there is no working segment parser to preserve as completed functionality. If a temporary adapter is implemented, malformed legacy tags remain literal during migration. The earlier bugs—generic-character checks shadowing tag handling, printing/voicing tag bytes, unused parsed values, stale buffers, and uninitialized integers—are useful review examples.

The lasting boundary is the segment list. XML mixed-content loading and a temporary inline-tag parser can both produce it; Speaker needs neither parser. After XML migration, maintain one required content format. Keep tests for any adapter that is still used, then remove the adapter when its last caller disappears.

### What dialogue looks like to its author and its player

The author writes lines, pauses, destinations, and choices. The player still gets the game's familiar typed text, blips, pacing, and prompts. The implementation stores where each operation resumes instead of blocking until an entire scene finishes. REFACTOR_PLAN section 5.4 traces an intro from greeting through name entry to Battle.

Keep three questions separate: the sequence says which line/choice comes next; Speaker says how much of the current line is visible; story code says what a name or choice means. None of this requires a visual graph editor. Begin with one short C++ sequence, draw the expected console interaction, and trace the input/update order before finalizing types. XML later supplies the same sequence data. A future graphical presenter adds text boxes, portraits, and layout without putting damage or story branching inside the typewriter.

## 8. Menu tree versus dialogue graph

For a nested command menu, each node can contain any number of children. A navigator holds the ancestor path and cursor. A leaf has a selection token; the game decides what selecting it means. The tree structure resembles Composite, but storing child nodes does not require claiming the full object-oriented Composite pattern.

Returning a token makes UI behavior testable without damage or item callbacks. Dynamic spell/item menus are assembled by game code when opened. Do not mutate/reallocate their storage while the navigator holds element pointers. ID/index navigation or a rebuild-at-closed-boundary rule is sufficient.

Dialogue can branch, rejoin, and loop. That is a graph. Reuse MenuNavigator to display a set of choices; map the selected token to a graph destination. Do not make choices inherit a menu's ancestor/back semantics. Canceling a dialogue choice should follow authored policy, not automatically navigate backward through the story.

For a small menu, vector storage is an appropriate starting point. First-child/next-sibling links, arenas, small-buffer vectors, and a hand-written MyVector remain optional data-structure exercises. None needs to block the playable milestone.

## 9. XML is the authoring format, not the gameplay object model

Keep XML because it fits the chosen mixed text/markup use and the user's learning goals. Do not justify it as mandatory AAA practice. pugixml supplies DOM traversal and parse status; our loader supplies game semantics. Its default whitespace behavior and non-validating scope are documented [N9]. No XML-library comparison is needed to reopen this settled choice.

Convert XML into our own definitions before play. For example, a `line` with a `pause` child becomes Text/Pause/Text segments. Do not pass pugi nodes throughout the engine, borrow their strings after document destruction, or query XML repeatedly to calculate damage.

### Whitespace and validation examples

Inside a line, spaces between inline elements can be meaningful. Parse options must preserve intentional text, including whitespace-only pieces where needed; then ignore indentation at structural levels. Do not trim every text node or read only the first text child. Add a fixture with two inline tags separated by one meaningful space.

A document can parse successfully and still contain two definitions named `bolt`, a monster referencing nonexistent `firaga`, or a pause of `50x` milliseconds. Those are content errors. Resolve references in a later loading pass so file order does not determine validity. Return diagnostics identifying file/definition/field. Publish content only after the required data is valid.

Dialogue graph cycles can be intentional. A cycle made only of immediate jumps can otherwise consume an entire frame forever. Limit immediate steps per update and report a stalled/error condition. Waiting for input or a timer naturally yields progress back to Game.

### How story commands cross the boundary

The game registers allowed command/query handlers. An authored command says “set this story flag” or “request Battle”; the engine passes validated typed arguments to the registered handler. It does not reach into a global Combatant or execute arbitrary function names. Conditions initially cover the needed named flags/comparisons. Growing this into a language, editor, or general scripting VM would be a separate project.

Yarn Spinner's documented runner/presenter and command-handler boundaries provide a useful reference [N8]. We are borrowing the separation of responsibilities, not promising Yarn-compatible XML or rebuilding its feature set.

## 10. Transfer to Unity, Unreal, and another custom game

| Concept learned here | Relationship to an established engine |
|---|---|
| Shared MonsterType/Ability definitions | Unity ScriptableObject assets are a way to author/share runtime data [N6]. Our XML loader is a different authoring path. |
| StatusDefinition plus ActiveStatus | Unreal distinguishes authored Gameplay Effects and runtime Specs/active effects [N7]. Our subset omits replication, prediction, tags, and most GAS facilities. |
| ModeController and battle phases | State-machine ideas transfer. Unity scenes are containers for scene objects, not synonyms for states. Unreal GameMode defines game rules and can include match flow [N14]. |
| DialogueRunner plus Speaker/menu presenters | The orchestration/presentation distinction appears in Yarn Spinner [N8]. |
| AudioManager plus mixing groups | Ownership, grouped controls, and asset references transfer; this wrapper does not reproduce either engine's audio tooling. |
| Small platform layer | Replacing terminal presentation later is easier because damage and content interpretation do not call terminal APIs. UI layouts still need porting. |

Learn the problem each component solves. Commercial engines supply editors, serialization, asset pipelines, platform support, and runtime services that this project deliberately narrows or omits. A small custom engine is viable for this genre without proving that commercial tools are bloated.

The user's direction is a Valyria Tear-inspired JRPG with their own style. The inspected Valyria Tear [audio interface](https://github.com/ValyriaTear/ValyriaTear/blob/master/src/engine/audio/audio.h) centralizes audio operations; its [common dialogue header](https://github.com/ValyriaTear/ValyriaTear/blob/master/src/common/dialogue.h) separates dialogue data/options and supervision. These are useful examples to compare responsibilities. Its audio code uses OpenAL and singleton access; those details do not override this project's chosen Game ownership and miniaudio wrapper. Inspect one responsibility when useful rather than copying the whole engine.

Preparing for Unity/Unreal means learning to adapt these responsibilities to their services. Graphics, editor use, engine-managed object lifetime, asset importing/serialization, UI, scripting, and packaging will still need practice. The payoff is being able to identify what belongs in data, what owns runtime state, and what should update or present it. It is not a promise that graphics will be the only unfamiliar part.

Object pooling, ECS, event buses, scripting, and graphics are not universal prerequisites for a real-time game. Adopt them when the game's needs justify them. The old claim about ZUN's private engine is not evidence for our design and is not retained as an architectural requirement.

## 11. Why the roadmap is ordered this way

First restore the existing build. Then establish resource ownership. Then make one intro responsive. Then complete one minimal battle with explicit rules and presentation stages. Only after those objects cooperate should XML loading and broad content migration expand the system.

This order avoids debugging XML, battle redesign, typewriter timing, audio lifetime, and the whole story at once. It also avoids implementing a blocking BattleManager and rewriting it immediately for the loop. The early in-memory fixture and the later XML path feed the same runtime types.

### Keep the work demonstrable

At each milestone, be able to describe both a code responsibility and a player-visible result. “Added StateMachine.h” is not a checkpoint. “The intro types while quit/skip still work” is. “Added an ability class” is not a checkpoint. “One battle completes, and invalid casts spend no MP” is.

Use a small doctest target when meaningful rule/sequencing behavior appears. Modern Catch2 v3 is a compiled library by default, so the old generic “doctest or Catch2, header-only” statement was inaccurate [N15]. A package-manager comparison is not needed to choose one small test dependency.

Keep dependency versions/licenses explicit. CMake FetchContent makes content available during configuration, but it is an option rather than a universally superior package manager. Vendoring a few pinned dependencies is adequate here. `SYSTEM` include directories affect header treatment, not every warning in vendor source files [N12].

Housekeeping comes after its replacement works. Confirm stale subtree scripts/remotes before removing them. Do not reset history or create an old-history copy merely to finish a source refactor. Git-ignored audio needs documented setup or silent fallback, not an assumption that a fresh clone has every file.

Stop when the completion target and XML-only extension test pass. An overworld, party targeting, ATB, replays, graphical presentation, and a second game each have separate acceptance criteria. They should reuse demonstrated seams and can still require localized changes.

### A small reading path when time permits

Use [Game Programming Patterns](https://gameprogrammingpatterns.com/contents.html) beside the next task, not as homework that blocks implementation. This is a suggested learning order, not another feature roadmap:

| When | Chapter | Question to bring back to the code |
|---|---|---|
| M2 ownership | [Singleton](https://gameprogrammingpatterns.com/singleton.html) | What dependencies would global service access hide? Reading the tradeoffs does not require adopting it. |
| M3 loop | [Game Loop](https://gameprogrammingpatterns.com/game-loop.html) | What keeps input, progress, and presentation running? |
| M3 Speaker/runner | [Update Method](https://gameprogrammingpatterns.com/update-method.html) | What progress must survive until the next update? |
| M3–M4 flow | [State](https://gameprogrammingpatterns.com/state.html) | Which transitions are legal, and who owns them? |
| M4 actions | [Command](https://gameprogrammingpatterns.com/command.html) | Can input and AI describe the same chosen action? |
| M4–M5 definitions | [Type Object](https://gameprogrammingpatterns.com/type-object.html) | What varies by definition, and what belongs to one instance? |

Read a short section when its question becomes concrete, then explain the answer using this game's code. The current engine layout can remain recognizable while these responsibilities become clearer.

## 12. Evidence, confidence, and primary sources

Sources were consulted online for this revision on 2026-10-25. Official API manuals establish documented library behavior; pattern chapters give worked architectural examples. Neither proves a universal AAA implementation. The owner table, stage names, input policy, numerical rules, initial timing limits, and milestone order are **project-specific design recommendations** derived from the goals.

For the original 2026-10-25 architecture review, the context7 CLI mentioned in CLAUDE.md was unavailable, so official online documentation was used instead; that review did not inspect a repository/build. For N3/N4, search-retrieved primary-source excerpts supported the narrow claims; full page retrieval was unavailable. N5 was also checked through the author's repository text. Exact API signatures must still be checked against pinned dependencies.

The 2026-11-19 follow-up read the supplied planning-review chat, inspected the actual source/assets/git state, and successfully built Debug with Visual Studio's bundled CMake. No interactive gameplay was tested. This follow-up updates baseline facts without claiming that every original external source was revalidated. Valyria Tear's audio/dialogue headers and the book author's Game Loop/Update Method source chapters were inspected online; the book website could not be retrieved directly, so its [author-maintained chapter sources](https://github.com/munificent/game-programming-patterns/tree/master/book) supplied those checks. Other entries in the optional reading path are recommendations, not claims of a fresh full-book review.

| ID | Primary source | What it supports |
|---|---|---|
| N1 | Robert Nystrom: [Game Loop](https://gameprogrammingpatterns.com/game-loop.html), [Update Method](https://gameprogrammingpatterns.com/update-method.html) | Repeated nonblocking input/update/render work and resumable per-object progress. |
| N2 | Glenn Fiedler: [Fix Your Timestep!](https://gafferongames.com/post/fix_your_timestep/) | Measured elapsed time, accumulators, catch-up, and overload tradeoffs. It does not mandate our initial limits. |
| N3 | Robert Nystrom: [Command](https://gameprogrammingpatterns.com/command.html) | Representing chosen actions separately; common action boundary for AI/input. |
| N4 | Robert Nystrom: [Type Object](https://gameprogrammingpatterns.com/type-object.html) | Representing supported type variation as data instead of subclasses. |
| N5 | Robert Nystrom: [State chapter source](https://github.com/munificent/game-programming-patterns/blob/master/book/state.markdown) | Finite-state, hierarchical-state, and pushdown alternatives. |
| N6 | Unity: [ScriptableObject data assets](https://docs.unity3d.com/Manual/class-ScriptableObject.html) | Shared data assets; a conceptual mapping, not a built-in buff implementation. |
| N7 | Epic: [Gameplay Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine) | Definitions, runtime Specs, duration, modifiers, and stacking as distinct concerns. |
| N8 | Yarn Spinner: [Dialogue Runner](https://www.yarnspinner.dev/docs/unity/04-showing-dialogue/01-dialogue-runner/), [Commands and Functions](https://docs.yarnspinner.dev/yarn-spinner-for-unity/creating-commands-functions) | Runner versus presenters and registered game integration. |
| N9 | pugixml: [Manual](https://pugixml.org/docs/manual.html), especially DOM, loading errors, parsing options, and exception configuration | Mixed-content traversal, parse status, whitespace flags, document ownership, and library scope. |
| N10 | miniaudio: [Programming manual](https://miniaud.io/docs/manual/), Introduction, Engine, Resource Management | Stable object addresses, initialization, groups, decode/stream distinction and sound lifetime. |
| N11 | Microsoft: [KEY_EVENT_RECORD](https://learn.microsoft.com/en-us/windows/console/key-event-record-str), [GetNumberOfConsoleInputEvents](https://learn.microsoft.com/en-us/windows/console/getnumberofconsoleinputevents), [ReadConsoleInput](https://learn.microsoft.com/en-us/windows/console/readconsoleinput) | Press/release/repeat records and availability-aware input handling. |
| N12 | CMake: [target_include_directories](https://cmake.org/cmake/help/latest/command/target_include_directories.html), [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) | Target-scoped include paths, SYSTEM treatment, configure-time dependencies. |
| N13 | C++ working draft: [One-definition rule](https://eel.is/c++draft/basic.def.odr) | The distinction between problematic duplicate definitions and valid header definitions. This is a living draft, not a pinned C++17 edition. |
| N14 | Unity: [Scenes](https://docs.unity.com/en-us/engine/6000.5/manual/working-with-scenes); Epic: [Game Mode and Game State](https://dev.epicgames.com/documentation/en-us/unreal-engine/game-mode-and-game-state-in-unreal-engine) | Scene contents versus game rules/match flow. |
| N15 | [doctest repository](https://github.com/doctest/doctest); Catch2: [Migrating from v2 to v3](https://catch2-temp.readthedocs.io/en/latest/migrate-v2-to-v3.html) | Testing-framework integration/distribution facts. |

Historical code-review observations remain useful leads, but the original broken-call claim is superseded by the successful 2026-11-19 build. Global menu construction, per-menu engines, missing audio, and stale TODO generation were observed in this checkout. Wrapping/defense correctness and full gameplay paths still need focused checks. Subtree cleanup must be based on actual remaining files/configuration, not an assumption imported from old notes. See the plan's dated snapshot before reporting progress.
