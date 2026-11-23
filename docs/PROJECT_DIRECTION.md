# Project direction

Updated: 2026-11-22. These are the user's goals and the current architectural direction, not a claim that the planned systems already exist.

This file states what we are making and why. [REFACTOR_PLAN.md](REFACTOR_PLAN.md) owns implementation order and acceptance criteria through its single M0–M8 roadmap. [DESIGN_NOTES.md](DESIGN_NOTES.md) explains the design and records sources. Update them together when a decision changes; do not create a competing roadmap.

## What we are making

A code-first, genre-focused C++17 JRPG engine and framework, learned by building a complete console boss-battle slice. Flowey is one encounter in a game that could later gain different lore, more encounters, and a full campaign. Reuse means more than sharing code between bosses: the foundation should support another application, and the JRPG framework should be useful to a future JRPG.

The comparison with ZUN expresses the desired scale and control: a personally understandable toolset specialized for the games the user wants to author. It is not a requirement to reproduce ZUN's private implementation or match an assumed architecture. The user wants direct code/data authoring rather than a GameMaker-style authoring workflow. A visual editor is not a prerequisite; useful diagnostics and convenient tools remain welcome.

Learning is the current priority; the Pico project's presentation deadline does not apply. The user writes and understands implementations unless they explicitly ask the assistant to implement. Explain one manageable responsibility, review attempts, and relate abstractions to actual code and behavior.

## Three reusable boundaries

| Boundary | Responsibility | Reuse target |
|---|---|---|
| Engine foundation | Platform input/output, timing, audio/resources, text, menu navigation, and generic dialogue execution | Another application can supply its own data without importing this game's characters or combat. |
| JRPG framework | Combatants, abilities, statuses, inventory, battle coordination, encounter contracts, and genre definitions | Another encounter and eventually another JRPG can use these systems without copying the Flowey fight. |
| Game and encounter content | Session/story ownership, asset bindings, authored encounters, lore, presentation choices, and genuinely special behavior | Flowey can be replaced or joined by another boss without redefining the engine or shared battle loop. |

Keep the current engine/game folders while those boundaries emerge. Shared JRPG code can start under `src/game`; it is an intentional future module/library boundary, not permanently synonymous with this particular game's content. Extract a target when actual dependencies and a second use make the boundary testable. Do not move every reusable function into `engine` or create empty subsystems to satisfy a folder diagram.

## Architecture choices

- Use modules with small public APIs and acyclic dependencies. Compose higher-level mechanisms from lower-level services; for example, Speaker may borrow AudioManager and Console.
- The application owns services and their lifetimes. Pass narrow dependencies explicitly, usually through constructors for ordinary C++ objects. This is manual dependency injection, not a requirement for a DI container or an interface for every class. Interfaces are useful at actual substitution/testing boundaries.
- Keep initialization, update, and shutdown order explicit. Each timed task has one updater. Engine code receives game requests/content rather than reaching into game headers or mutable globals.
- Separate authored definitions, runtime instances, rule resolution, and presentation. A battle returns an outcome; game flow interprets it. Session state survives encounters where intended.
- Learn component-based behavior assembly within this project. Begin with one real combat variation and a narrow replaceable behavior boundary; do not require ECS, runtime component discovery, or a component registry. Data-only variation stays data; some components may use a shared interface.

## Shared combat vocabulary and subtractive bosses

The user's intended game design is a rich shared combat vocabulary whose boss encounters primarily select, restrict, configure, and sequence supported modes/actions. Treat subtraction as a deliberate encounter-design rule, not merely a code-reuse optimization. A boss need not introduce another subsystem to feel unique.

Unified combat means shared coordination of separable modes, not one object implementing every mechanic. First specify the intended vocabulary in understandable gameplay terms, then implement it incrementally as playable slices. Do not invent an enormous superset in advance or quietly replace this goal with an additive gimmick per boss. The exact combat modes are not yet selected; previous timing/dodging examples were possibilities, not approved feature requirements.

Build encounter definitions first for supported differences. Introduce localized custom behavior when a concrete mechanic needs it; if it extends the vocabulary, acknowledge that design decision and decide whether it belongs in the shared framework. No `FloweyBattle` subclass, generic behavior interface, or subclass per boss is mandated. Composition and inheritance can cooperate where a stable behavior contract warrants them.

## Learning Unity deliberately

Use this engine to learn ownership, dependencies, update-driven behavior, state machines, composition, definitions versus instances, and presentation separation. Connect those decisions to Unity's components, lifecycle, serialized references, assets, and UI/audio systems. These are conceptual mappings, not API equivalences or promises of unchanged source portability. Unreal remains a secondary comparison when useful.

Pair selected milestones with a small Unity exercise when the user is ready: supply a component dependency, advance a timed line, share an ability definition while keeping separate actor state, or connect a result to UI. Use a tiny separate exercise, not a port of the whole game or an automatic new project. Actual Unity practice is needed for scenes, GameObjects, prefabs, serialization, importing, and editor/lifecycle behavior. See DESIGN_NOTES section 10 for sources and limits.

## Roadmap and evidence

Follow REFACTOR_PLAN section 9: baseline and safe services → responsive presentation → one composable battle slice → validated content → complete encounter behavior → story isolation → reuse checks. Components and Unity comparisons fit that sequence rather than replacing it.

Completion of this slice means a full encounter and a second minimal encounter use the same flow, supported content can be extended through XML, intended session state persists, and battle-only state resets. Add a tiny second-client probe of selected engine services with no combat/Flowey dependencies; a full second game is not required. Identify the shared JRPG boundary explicitly rather than claiming cross-game reuse has already been proven. A full campaign, overworld, graphical backend, additional combat modes, and a visual editor remain separately chosen extensions.

The assistant should distinguish user requirements, project-specific design decisions, verified current code/build state, and external evidence. Credit library authors and pattern/API sources; do not call the design AAA-equivalent or infer ZUN's current internals from an older reconstruction.

For future goal changes, invoke `$update-project-direction` with the new direction. The locally installed skill reconciles project instructions, direction, roadmap, and explanations, checks current state, and identifies the next small step. It does not authorize gameplay implementation, commits, or publishing.
