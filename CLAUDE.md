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
- `Rougelike/Source/Rougelike/`: main module, one folder per feature: `Characters/` (base, `Player/` with the input config, `Enemy/`), `Combat/` (health component), `Core/` (game mode), `GameplayAbilitySystem/` (ASC, attribute sets, abilities, effects, `GrantSet`), `Equipment/`, `Inventory/`, `Items/` (definitions, instances, fragments, actions, `Stats/`), `UI/`, `Weapons/` (weapon actor, projectile). The template `Variant_*` code has been removed.
- `Rougelike/Content/Rougelike/`: own content (`Core/`, `Features/`, `UI/`). The default map is `Features/Maps/Lvl_Arena`.
- Own code goes in clearly named feature folders. Decide layout together with the user as systems appear.

## Game Vision (rough, to be refined)
- Top-down 3D ARPG (Diablo-like) crossed with a roguelike: arena combat, loot, stat-heavy builds.
- Run-based with randomized upgrade/item choices, escalating enemy waves and a meta layer later.
- Builds are defined by stacking items and abilities with synergies.
- Generic roguelike systems (enemies, weapons, loot, inventory) stay modular and reusable.
- Scope intentionally small at first: one arena, one weapon, one enemy type, a handful of upgrades. Prove the systems, then widen.
- All of that can change, even the camera perspective its all just the initial idea, the only thing fixed is that its a rougelike in 3d

## Decisions
- **2026-10-16, vision:** ARPG × roguelike with a top-down camera, in this same project (no new project).
- **2026-10-16, controls:** WASD movement relative to the screen; the character aims and shoots at the mouse cursor. No crosshair and no click-to-move for now. The character always faces the cursor. Jump stays for now.
- **Aim ownership:** the controller turns input into a world aim point. The character faces it, and weapons fire at a point they are given without knowing about cameras or cursors.
- **2026-10-17, UI:** all screens go through `UUIManagerSubsystem` (`Source/Rougelike/UI/`): CommonUI stacks addressed by layer tag (`UI.Layer.Game/Menu/Modal/Loading`), registered by `WBP_UILayout`. Every pushed widget derives from `UUIActivatableWidget` and declares its input mode. Game mode never hides the cursor and only captures during mouse down, because the cursor is the aim device. Pieces placed directly inside a screen (vitals bars, slots, tooltips) derive from `UCommonUserWidget`; only widgets pushed to a layer, or hosted by a stack/switcher, are activatable.
- **2026-10-18, damage pipeline (GAS; formula amended 2026-10-20):** a hit carries its own base damage (`FDamageEffect`, written to the spec as `SetByCaller.Damage`); characters only carry stats (`UOffenseAttributeSet`: `BonusDamage`, `IncreasedDamage`), changed only by GEs. One execution, `UDamageExecution`, computes `(Base + BonusDamage) × (1 + IncreasedDamage)` and writes the victim's `IncomingDamage` meta attribute; `UCoreAttributeSet::PostGameplayEffectExecute` turns it into Health loss and sends a `FDamageReport` to the victim ASC (`OnDamageReceived`). `IncreasedDamage` is a sum (base 0, 0.2 = +20%), applied once. Stats are read when the hit lands (no snapshot). Conditional bonuses ("projectile only", "vs burning") are tag requirements on modifiers, never extra attributes. DoTs like burn are just another base damage; bonus damage applies fully per tick for now (an "effectiveness of added damage" factor can come later).
- **2026-10-19, GAS chosen, single player only.** GAS is the effect/modifier system. No replication, prediction or server-authority design; existing replication boilerplate stays harmless.
- **2026-10-19, the grant door:** abilities and lasting (Infinite) effects reach an ASC only through `FGrantSet::Give(ASC, SourceObject)`, and leave only through the `FGrantedHandles` receipt it returns. Whoever gives keeps the receipt; the source (an item instance, the character for its starting kit, later a skill node) is the SourceObject, so abilities find their item and every hit carries it. Instant effects and self-ending timed effects stay outside. Lasting effects use Stacking Type None, so each source owns its own application.
- **2026-10-19, input tags:** input actions send `InputTag.*` tags (`URougelikeInputConfig`); the tag sits on the ability spec as a dynamic tag, put there by the grant set. Ability input actions have no triggers: the ability's `ActivationPolicy` decides press vs hold.
- **2026-10-19, equipment:** `UEquipmentComponent` owns slots (gameplay tags, set per owner), spawns the actor, and gives the definition's `FGrantSet` with the item as source, plus the item's stat effects, on one receipt. Unequip revokes before destroying the actor, so a running ability ends while its weapon still exists. Definitions are Blueprint classes with fragments, not data assets.
- **2026-10-20, items are data:** an item's whole state is its definition plus one stats map (rolls and counters alike), as `FStackInfo`. `UItemInstance` exists only inside an inventory; `UItemInstance::CreateItem` is the single factory and `ToStackInfo` the reverse. Drop turns an item into data on a pickup; pickup rebuilds it. Anything an item must remember is a stat, and a fragment that writes stats must skip ones already present, so a rebuilt item never re-rolls (rolls go through `ItemRandom::RollRange`).
- **2026-10-20, item stats:** `UItemStatDefinitions` (set in Project Settings > Items) maps each `Item.Stat.*` tag to a display name and an optional equipped effect: an archetype GE (Infinite, Stacking None) whose modifier reads SetByCaller under the stat's own tag. Rolled values never use GE stacking. The Data Tag picker needs the Gameplay Tags category remap `SetByCaller` → `SetByCaller`, `Item.Stat`.
- **2026-10-20, max Health/Mana:** when the max rises, current rises by the same amount (30/100 → 80/150); when it drops, current only clamps. A dead character gains nothing.

## Deferred (not built; each plugs into the grant door)
- Combat events and procs, built on the damage report. A single hit choke point with a per-target context (needed for pierce/AoE and procs).
- Ability modifiers: tag-scoped behaviour stats (pierce, extra projectiles, area).
- More damage buckets: "more" multipliers, crit, resistances.
- New kinds of sources: upgrade cards, relics, enemy modifiers. Add a per-character source registry when the second container appears.
- Statuses beyond Burn. Loot tables and rewards (seeded RNG through the roll helper). Run structure, saves and meta progression (saves store `FStackInfo`).
- Polish: C++ melee trace task, enemy attacks as abilities, pooling, MVVM.

## Suggested Roadmap (adjust as we go)
1. ~~Strip down and own the base~~ (done).
2. ~~Core combat: health/damage model, weapons, hit detection~~ (done; weapons are equippable items).
3. ~~Attribute and modifier system~~ (GAS chosen, foundation done).
4. Upgrade/item system: items, equipment, inventory and item stats are done; upgrade cards, relics and trigger/effect events are open (see Deferred).
5. Enemies and waves: simple AI, spawner, pooling.
6. Run loop: arena start, reward choice, progression, death and restart.
7. Performance pass: Unreal Insights, stat commands, profiling, optimization with measurements.
8. Polish, UI, VFX/SFX, meta-progression.

## Working Notes
- Open design questions and decisions get recorded in the "Decisions" section as we make them.
- Keep this file short and current. Update it when goals or architecture decisions change.
- Use the Unreal MCP to get more acces into the engine / editor
