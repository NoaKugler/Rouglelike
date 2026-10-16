# Rougelike — Project Instructions

## Project Summary
A hobby top-down ARPG × roguelike in Unreal Engine 5.8 (C++ module `Rougelike`): Diablo-style combat and loot inside run-based roguelike structure.
Started from the Third Person template (originally as a Crab Champions-style learning project) and is being moved to top-down controls.

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
- `Rougelike/Source/Rougelike/`: main module, organized in feature folders (`Player/`, `Gun/`, `Enemy/`). The template `Variant_*` code has been removed.
- `Rougelike/Content/_Game/`: own content. `Content/ThirdPerson/` is template leftovers, to be removed.
- `TopDownTemplate/` (gitignored): throwaway UE 5.8 Top Down C++ project. **Reference only**: read its code and migrate pure content (input actions, FX, meshes) from it, never its Blueprints or C++.
- Own code goes in clearly named feature folders (e.g. `Combat/`, `Items/`). Decide layout together with the user as systems appear.

## Game Vision (rough, to be refined)
- Top-down 3D ARPG (Diablo-like) crossed with a roguelike: arena combat, loot, stat-heavy builds.
- Run-based with randomized upgrade/item choices, escalating enemy waves and a meta layer later.
- Builds are defined by stacking items and abilities with synergies.
- Generic roguelike systems (enemies, weapons, loot, inventory) stay modular and reusable.
- Scope intentionally small at first: one arena, one weapon, one enemy type, a handful of upgrades. Prove the systems, then widen.

## Decisions
- **2026-10-16, vision:** ARPG × roguelike with a top-down camera, in this same project (no new project).
- **2026-10-16, controls:** WASD movement relative to the screen; the character aims and shoots at the mouse cursor. No crosshair and no click-to-move for now. The character always faces the cursor. Jump stays for now.
- **Aim ownership:** the controller turns input into a world aim point. The character faces it, and weapons fire at a point they are given without knowing about cameras or cursors.

## Suggested Roadmap (adjust as we go)
1. Strip down and own the base: clean character, input, camera, game mode. **Active plan: [Docs/Plan-TopDownFoundation.md](Docs/Plan-TopDownFoundation.md).**
2. Core combat: health/damage model, a basic weapon, hit detection.
3. Attribute and modifier system (the foundation for stacking). Decide GAS vs custom.
4. Upgrade/item system: definitions as data, runtime instances, stacking rules, trigger/effect events.
5. Enemies and waves: simple AI, spawner, pooling.
6. Run loop: arena start, reward choice, progression, death and restart.
7. Performance pass: Unreal Insights, stat commands, profiling, optimization with measurements.
8. Polish, UI, VFX/SFX, meta-progression.

## Working Notes
- Open design questions and decisions get recorded in the "Decisions" section as we make them.
- Keep this file short and current. Update it when goals or architecture decisions change.
- Use the Unreal MCP to get more acces into the engine / editor
