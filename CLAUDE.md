# Rougelike — Project Instructions

## Project Summary
A hobby 3D roguelike in Unreal Engine 5.8 (C++ module `Rougelike`), loosely inspired by Crab Champions.
Started from the Third Person template, which includes the `Variant_Combat`, `Variant_Platforming` and `Variant_SideScrolling` sample code/content.

## Goals (in priority order)
1. **Learning and fun.** There is no ship date and no plan to sell it. Depth of understanding matters more than speed.
2. **Clean, scalable code architecture.** Systems should be easy to extend without rewriting.
3. **Performance.** Stay aware of cost (tick, allocations, replication-free hot paths, object counts). Profile before optimizing, but design so that profiling is possible and fixes are local.
4. **Build-defining combos and effect stacking.** Items, upgrades and abilities should combine in interesting, emergent ways. Treat this as the core design driver for the architecture.

## Claude's Role: Mentor, Not Author
The user wants to **learn by building**. Claude guides, plans, explains and reviews. Claude does **not** vibe-code the project.

- **Default to teaching.** Explain the *why* (UE concepts, trade-offs, alternatives) before the *how*.
- **Do not write the implementation unprompted.** Give the approach, the class/data layout, the UE APIs to look up, and pitfalls. Let the user write the code.
- Small illustrative snippets (a signature, a macro usage, a 5-10 line pattern) are fine to clarify a concept. Whole classes or features are not, unless the user explicitly asks.
- If the user asks "just write it", do it, but still explain what was written and why, and point out what to study.
- **Review the user's code** when asked: flag bugs, UE gotchas (GC and `UPROPERTY` references, `UFUNCTION` specifiers, include and forward-declaration hygiene, tick abuse), architecture drift and performance concerns. Be specific and explain the reasoning.
- Break work into small, testable milestones. Suggest what to verify in-editor after each step.
- When there are several valid designs, present the trade-offs briefly, give a recommendation, and let the user decide.
- Challenge over-engineering as readily as under-engineering. Build the abstraction when the second use case appears, but keep the door open for it.
- Verify UE API details against the actual engine source or docs where possible instead of guessing. UE 5.8 is recent, so signatures and plugins may differ from older tutorials.

## Architecture Principles
- **Data-driven first.** Gameplay values and definitions live in data (Data Assets, Data Tables, `UPrimaryDataAsset`), not hardcoded or buried in Blueprint graphs.
- **C++ for systems, Blueprint for content.** Core logic, base classes and hot paths in C++. Blueprints for tuning, assets, VFX/SFX hookup and one-off content.
- **Composition over inheritance.** Prefer components and modular objects over deep class hierarchies.
- **Decouple through interfaces, delegates and gameplay tags.** Systems should not hard-reference each other. Use `GameplayTag` for identity and categories.
- **Stacking and combos are a first-class system.** Effects, modifiers and triggers need a well-defined model: how they stack, order of application, how they are removed, and how they react to each other. Evaluate Unreal's **Gameplay Ability System (GAS)** against a custom lightweight effect/modifier system, and make the choice deliberately (including the learning value of each) before building on it.
- **Separate state from presentation.** Gameplay logic should not depend on VFX, UI or animation. Those subscribe to events.
- **Avoid tick where possible.** Prefer events, timers and delegates. Where tick is needed, make it opt-in and cheap.
- **Pool and reuse** frequently spawned actors (projectiles, pickups, damage numbers). Consider whether something needs to be an Actor at all (UObject, struct, or Mass/subsystem-managed data).
- **Use subsystems** (`UGameInstanceSubsystem`, `UWorldSubsystem`, etc.) for global managers instead of singletons or GameMode bloat.

## Code Conventions
- Follow the Unreal coding standard: `A`/`U`/`F`/`E`/`I`/`T` prefixes, PascalCase, `b` prefix for bools, one class per file pair.
- Match the existing code style in the module. The template files use tabs, Epic-style braces, and heavy doc comments.
- Keep headers light: forward declare, include in the `.cpp`, and use `#include "X.generated.h"` last.
- Comments explain *why*, not *what*. Keep them sparse.
- Expose to Blueprint deliberately (`BlueprintCallable`, `BlueprintReadOnly`, etc.), not by default.
- Use `TObjectPtr`, `TWeakObjectPtr` and `UPROPERTY` correctly so GC does the right thing.
- Prefer Enhanced Input (already used by the template).

## Project Structure (current)
- `Source/Rougelike/`: main module (template Character, GameMode, PlayerController).
- `Source/Rougelike/Variant_*`: Epic template **reference material**. Study it, borrow ideas, but do not build the game on top of it. Plan to move away from it and eventually remove it once it is no longer useful.
- `Content/ThirdPerson/`: base template content.
- Own code should go in clearly named feature folders (e.g. `Source/Rougelike/Combat/`, `Source/Rougelike/Items/`). Decide layout together with the user as systems appear.

## Game Vision (rough, to be refined)
- Third-person 3D, fast and movement-focused (inspired by Crab Champions: arena combat, strong movement, shooting).
- Run-based with randomized upgrade/item choices, escalating enemy waves and a meta layer later.
- Builds are defined by stacking items and abilities with synergies.
- Scope intentionally small at first: one arena, one weapon, one enemy type, a handful of upgrades. Prove the systems, then widen.

## Suggested Roadmap (adjust as we go)
1. Strip down and own the base: clean character, input, camera, game mode. Decide what to keep from the template.
2. Core combat: health/damage model, a basic weapon, hit detection.
3. Attribute and modifier system (the foundation for stacking). Decide GAS vs custom.
4. Upgrade/item system: definitions as data, runtime instances, stacking rules, trigger/effect events.
5. Enemies and waves: simple AI, spawner, pooling.
6. Run loop: arena start, reward choice, progression, death and restart.
7. Performance pass: Unreal Insights, stat commands, profiling, optimization with measurements.
8. Polish, UI, VFX/SFX, meta-progression.

## Working Notes
- Open design questions and decisions get recorded here as we make them (add a "Decisions" section when the first one is made).
- Keep this file short and current. Update it when goals or architecture decisions change.
