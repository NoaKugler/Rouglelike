# Plan: GAS Foundation (items, abilities, stats)

Status: agreed 2026-10-19. Replaces the old split between the tutorial inventory system and the tutorial GAS system with one ecosystem.
Scope: a **foundation**, not a game. It supports abilities, stats and equippable items/weapons in a modular way, whatever direction the game takes. No game loop, procs, upgrade cards, loot or run state yet (see the [Todo](#todo-later-not-part-of-the-foundation) list).
Single player only.

> **What "foundation" means here.** The game's direction is still open, so this plan builds only the general systems everything else will sit on: granting and revoking abilities and effects, equippable items and weapons that use that path, items whose whole state is plain data (definition + stats) that one factory turns into instances, item stats as GEs, input that isn't tied to specific abilities, and native tags. It deliberately does **not** decide content or game design: no specific slots, item types, stat lists, status effects, procs, upgrade cards, loot or game loop. Those are added later on top of this foundation, if and when the game needs them, without rewriting it. Concrete numbers and formulas (like the damage formula) are placeholders and are expected to change.

---

## 1. The idea in one paragraph

Every source of power (the character's starting kit, and an equipped item or weapon) is data that describes a **grant set**: abilities plus "while active" Gameplay Effects. One small function pair gives a grant set to an ASC and revokes it again. Giving it returns a **receipt** (the ability spec handles and active effect handles), and whoever granted keeps that receipt. The **source itself is the SourceObject**: for an item, that is its item instance, never the character. Because of that:

- abilities granted by an item can find their item (its rolled stats, its weapon actor)
- every hit those abilities make carries the item in its effect context
- unequipping removes exactly what equipping added, nothing more

Later sources (upgrade cards, relics, enemy modifiers) use the same grant set and receipt. Only *who decides when the source is active* differs, so they will be additions, not rewrites.

## 2. Principles

1. **One way in and out.** Persistent abilities and effects reach the ASC only by giving a grant set. They leave only by revoking its receipt. Nothing else calls `GiveAbility` or keeps active-effect handles.
2. **The source is the SourceObject.** Items use their `UItemInstance`. The character's permanent starting grants may use the character.
3. **Stats change only through GEs** (the 2026-10-18 decision still holds). Item stats are infinite GEs applied while equipped. Conditional bonuses ("with staves", "vs burning") are tag requirements on modifiers, never extra attributes.
4. **Rolled values never use GE stacking.** A new stack replaces the whole spec, so the newest roll would win. Every applied stat GE uses StackingType None.
5. **C++ for systems, Blueprint for content.** Inventory and equipment logic move to C++. Items, weapons, abilities and GEs stay Blueprint and data.
6. **Generic data, nothing hard-coded.** Equipment slots and stat types are gameplay tags defined in data. The C++ knows no slot list and no item types.
7. **Native tags for everything C++ reads.** No `RequestGameplayTag("...")` strings.
8. **Presentation subscribes, never drives.** UI listens to delegates from the ASC, the equipment component and the inventory.
9. **Each step ends playable.** One or two new concepts per step, each with in-editor checks.

## 3. The pieces

| Piece | Kind | Job | Replaces / changes |
|---|---|---|---|
| `FGrantSet` | USTRUCT (new) | Data: abilities to grant (class, level, input tag) and infinite GEs to apply while active | `FWeaponConfig.AbilitiesToGrant`, `StartupEffects`/`DefaultAbilities` |
| `FGrantedHandles` | USTRUCT (new) | The receipt: what one grant added (spec handles + active effect handles); can revoke it | `AbilitiesGrantedByWeapon`, handles that get dropped today |
| Give / Revoke | functions on the grant set and receipt (new) | Give the set to an ASC with a SourceObject; revoke the receipt | `ABaseCharacter::GrantAbilities/RemoveAbilities` |
| `URougelikeAbilitySystemComponent` | ASC subclass (adapt; empty today) | Engine hooks only: a single "abilities changed" event, input-tag activation, a per-victim damage-report delegate | `SendAbilitiesChangedEvent` + the `Event.Abilities.Changed` gameplay event |
| `UEquipmentComponent` | C++ actor component (new) | Generic slots keyed by tag, equip/unequip, spawning and attaching the weapon actor, giving and revoking the item's grant set, `OnEquipmentChanged` | BP `EquipmentManagerComponent` and BP `WeaponsManagerComponent` |
| `UEquipmentDefinition` | Blueprint class definition (keep, extend) | "What equipping does": actor class + socket, **grant set**, allowed slot tag. Several items can share one. | gains the grant set; `FWeaponConfig` is folded into it |
| `UEquipmentInstance` | UObject (adapt) | Runtime record of one equipped item: the spawned actor and the item's **receipt** | today it only spawns and attaches |
| `UItemDefinition` + fragments | Blueprint class definitions (keep) | Item identity plus composition. The Equippable fragment points to an equipment definition. | unchanged model; small fixes |
| `UItemInstance` | UObject (adapt) | The runtime form of one item while it is in an inventory: definition + one stats map holding **everything** (rolls and counters like durability). It is the SourceObject for everything the item grants. Rebuilt from data whenever the item enters an inventory. | holds nothing outside the stats map |
| `FStackInfo` | USTRUCT (keep) | The data form of an item stack: definition, count, stats. Pickups (placed and dropped) hold it; later saves store it. | unchanged; becomes the canonical item data |
| `UInventoryComponent` (+ inventory model) | C++ actor component (new) | Storage, stacking, the **single item factory**, pickup/drop/use. Dropping unequips first. | `BP_InventoryManagerComponent`, `BPFL_ItemUtils` |
| `ItemAction_Equip` | BP item action (keep) | Thin wrapper that calls `UEquipmentComponent`, so equipping stays inside the fragment/action system | today it calls the BP manager |
| Stat definitions | small data asset (new) | Maps each item stat tag to its archetype GE and its display name/format for tooltips | raw tag names in tooltips; debug `GE_FlatDamage`/`GE_DamageMultiplier` |
| `GE_Stat_*` | GE assets (new) | One archetype GE per stat type: Infinite, no stacking, one modifier whose value is SetByCaller | — |

Not in the foundation on purpose: a per-character source registry, data-asset definitions, a combat event router, a custom effect context. See the Todo list.

## 4. How items affect stats and abilities

1. **Stats.** An item's rolled stat (say +4 bonus damage) is applied while equipped as one `GE_Stat_*` with the item as SourceObject. Unequipping revokes exactly that handle.
2. **Granting abilities.** A weapon is an item whose equipment definition's grant set contains its attack ability, bound to an input tag. The ability finds its item and weapon actor through `GetCurrentSourceObject()` (item → equipment component → actor). This replaces `WeaponsManagerComponent.EquippedWeapon`.
3. **Changing other abilities** (later, mostly content). Bonuses like "+30% damage with staves" or "+1 pierce for projectile abilities" are modifiers with tag requirements, evaluated by the existing damage execution. Pierce-style behaviour stats are on the Todo list.

## 5. Damage (for starters; will be refined)

**Final = (Base + Bonus) × (1 + Increased)**

- **Base** is unchanged: set per hit by `MakeDamageEffectSpec` as `SetByCaller.Damage`.
- **Bonus** = `BonusDamage` (renamed from `FlatDamage` in F6), modified with AddBase. It is a flat amount added to every hit and every DoT tick.
- **Increased** = a new `IncreasedDamage` attribute: base 0, AddBase only, and data enters 0.2 for +20%. All increased bonuses are summed first, then applied once.
- `IncreasedDamage` replaces today's `DamageMultiplier`. Rename it in F6 together with the debug GE that uses it.
- Stats are still read when the hit lands (no snapshot). A DoT tick is still just another base damage.
- A "more" bucket (separate multipliers) can be added later as a × factor, without touching data that already exists.

This amends the 2026-10-18 decision; record it in CLAUDE.md when F6 is done.

## 6. Decisions (agreed 2026-10-19)

| Topic | Decision |
|---|---|
| Item and equipment definitions | Keep Blueprint classes with fragments. No data-asset migration. Loot tables later can reference item classes. |
| `UEquipmentDefinition` | Keep and extend with a grant set and an allowed slot tag. `FWeaponConfig` folds into it. |
| Equipment slots and item types | Generic: slot tags in data, nothing hard-coded. |
| Swapping or unequipping a weapon mid-attack | Its running ability ends immediately. |
| Dropping an equipped item | Unequip first, then drop. |
| MaxHealth / MaxMana change (changed 2026-10-20) | **Raise adds, lower clamps.** When the max rises, Health/Mana rise by the same amount (30/100 → 80/150). When it drops, they only clamp to the new max (80/150 → 80/100, 150/150 → 100/100). A dead character (Health 0) gains nothing. Side effect: unequipping and re-equipping a +Max item gives back Health each cycle. |
| `ItemAction_Equip` | Stays as a Blueprint wrapper around the C++ equipment component. |
| Damage formula | (Base + Bonus) × (1 + Increased) for now. `FlatDamage` is renamed to `BonusDamage`. |
| Item state (changed 2026-10-20) | **Data, not identity.** An item's whole state is its definition + stats map (`FStackInfo`). Dropping turns the item into data on a pickup; picking up rebuilds a new instance through the factory. Everything an item remembers must be a stat. This keeps drop/pickup on the same path saves will use. |
| Testing items | Placed in the level by hand. No loot or RNG system yet. Rolls go through one small helper function, so seeding can be added later. |

## 7. What changes in the current code

**Keep:**
- `UDamageExecution` and the `IncomingDamage` meta attribute, and the `MakeDamageEffectSpec` ordering
- `UProjectileAbility` and projectile `HitEffects`
- the item definition / fragments / instance model
- `FStackInfo` as the data form of items (pickups, drop, later saves)
- the inventory stacking algorithm (ported to C++)
- `ANState_HitScanWindow` events
- `GE_Burn`
- `UHealthComponent` (as a UI adapter)
- aim ownership
- the UI framework

**Adapt:**
- `ABaseCharacter`: init the ASC once, ready event, starting grant set, aim point fix
- `URougelikeAbilitySystemComponent`
- `UCoreAttributeSet`: follow max changes (raise adds, lower clamps), damage report
- `UOffenseAttributeSet`: `IncreasedDamage`
- `URougelikeGameplayAbility`: `InstancedPerActor` default, display data, source-item helpers
- `UItemInstance`, `UItemFragment_Equippable`
- `UEquipmentDefinition`, `UEquipmentInstance`
- `GA_MeleeAttack_Base`, `BP_Weapon_*`
- `BP_PlayerCharacter`: becomes a content shell
- the inventory and ability-bar widgets
- `ItemAction_Equip`, `BP_ItemPickup`

**Replace with C++:**
- BP `EquipmentManagerComponent`
- `BP_InventoryManagerComponent` (+ `BPFL_ItemUtils`)

**Delete:**
- `WeaponsManagerComponent`
- `FWeaponConfig`
- the legacy `Damage` attribute
- `GrantAbilities` / `RemoveAbilities` / `SendAbilitiesChangedEvent`
- `AbilityReplicationMode`
- `IA_ToggleEquip` and the 1/2 weapon keys
- debug `GE_FlatDamage` / `GE_DamageMultiplier`: replaced by test items

---

## 8. Steps

Each step ends playable. Debug tools used in the checks:
- **Gameplay Debugger:** apostrophe key, then enable the *Abilities* category (it starts disabled). Shift+1..4 show tags / abilities / effects / attributes. The abilities view shows each spec's SourceObject.
- **`AbilitySystem.Ability.ListGranted`** and **`AbilitySystem.Effect.ListActive`:** show presence and count only, not source or level.
- **`AbilitySystem.DebugAttribute <Attr...>`** (+ `AbilitySystem.DebugIncludeModifiers`)

### F0: Clean ground
**Goal:** fix what items would break, and install the engine hooks the rest needs. The game plays the same.

Sub-steps:
- **Hygiene.** Fix Up Redirectors (the `Item_Staff` redirector), resave `Lvl_Arena`, switch `LogTemp` call sites to the existing `LogRougelike`, remove debug PrintStrings.
- **Native tags.** Every tag C++ reads gets declared natively. Replace both `RequestGameplayTag` strings, and retag the few `GameplayAbility.*` tags by hand if you rename them.
- **`UItemInstance::Initialize`.** Skip null fragments, and merge passed-in stats per key instead of replacing the whole map.
- **`ABaseCharacter`.** `TObjectPtr` members, delete `AbilityReplicationMode`, initialize the ASC once (not on every possession), add an "ability system ready" event, and make `GetAimLocation` return a point ahead instead of the world origin.
- **`URougelikeGameplayAbility`.** Default to `InstancedPerActor` (5.8 defaults to a new object per activation). Add a display name and icon for the ability bar.
- **ASC "abilities changed" event.** Broadcast from `OnGiveAbility`/`OnRemoveAbility`, coalesced to the next tick, because removal fires before the spec leaves the list. The ability bar listens to it instead of `Event.Abilities.Changed`.
- **`UCoreAttributeSet`.**
  - Delete the legacy `Damage` attribute.
  - When MaxHealth/MaxMana rise, add the same amount to Health/Mana. When they drop, clamp Health/Mana to the new max.
  - Build one damage report in the `IncomingDamage` branch and send it to the victim ASC's delegate. `UHealthComponent::OnDamaged` uses it, instead of guessing damage from Health going down.

**Teaches:**
- native gameplay tags
- ASC and AttributeSet hooks: `OnGiveAbility`, `PostAttributeChange` vs `PostGameplayEffectExecute`

**Verify:**
- Weapons still work through the old path, with the same damage numbers.
- Apply a +50 MaxHealth test GE at 100/200: Health becomes 150/250, and removing it gives 150/200. At full health it goes 200/200 → 250/250 → 200/200. No damage event fires for any of it.
- The ability bar refreshes once per weapon swap.
- No `RequestGameplayTag` left in Source.

### F1: The grant door
**Goal:** abilities and effects reach the ASC through a grant set with a stored receipt. Proven on the character's starting grants while weapons still use the old path.

Sub-steps:
- `FGrantSet` (abilities: class, level, input tag; effects: infinite GEs) with a give function that takes the ASC and a SourceObject and fills an `FGrantedHandles`.
- `FGrantedHand
### F2: Input tags
**Goal:** keys activate abilities by tag, so any item can decide what a key does.

Sub-steps:
- An input config data asset maps each Input Action to an `InputTag.*`.
- Grant sets put the input tag on the ability spec.
- The ASC subclass gets pressed/released functions that activate the spec carrying that exact tag, mirroring the engine's own input-pressed handling.
- An activation policy per ability: on press, or while held (hold to fire, limited by the cooldown).
- Move Dash first. Attack moves in F4.les` with revoke.
- `ABaseCharacter`:
  - Replace `StartupEffects`/`DefaultAbilities` with an instant init-attributes GE plus one starting `FGrantSet` (Dash, mana regen), given on "ability system ready".
  - `GE_ManaRegen` becomes always on, which removes the BP regen loop.
  - Keep the old `GrantAbilities` only until F4.

**Teaches:** `FGameplayAbilitySpec`, SourceObject, spec handles vs active effect handles.

**Verify:**
- Dash works; mana regenerates and stops at max.
- Gameplay Debugger abilities view shows GA_Dash once.
- Effects view shows GE_ManaRegen active.


**Teaches:** Enhanced Input bound in C++, dynamic spec tags.

**Verify:**
- Dash works from its key.
- Holding a held-policy ability repeats at its cooldown rate.

### F3: C++ equipment component (same behaviour as today)
**Goal:** a C++ `UEquipmentComponent` replaces the BP `EquipmentManagerComponent` with the same cosmetic result, plus slots.

Sub-steps:
- Slots are gameplay tags listed on the component.
- `UEquipmentDefinition` gets an allowed slot tag. A parent tag can match child slots, so two ring slots share one tag family.
- Equip unequips the slot's occupant, spawns the actor deferred with Owner/Instigator = pawn, attaches it to the definition's socket, and broadcasts `OnEquipmentChanged`.
- `UEquipmentInstance` becomes the runtime record.
- `ItemAction_Equip` now calls the C++ component.

**Teaches:** designing a C++ component API (slots, native + dynamic delegates, deferred spawn).

**Verify:**
- Equipping the staff from the inventory attaches it.
- Equipping a second item into the same slot replaces it.
- The spawned actor has an Owner.
- Don't use the old 1/2 keys at the same time as this path.

### F4: The merge
**Goal:** equipping an item grants its weapon ability through the door with the item as source. The old weapon path is gone.

Sub-steps:
1. **Remove the old path's inputs first:** the 1/2 keys and `IA_ToggleEquip`, so two paths never grant at once.
2. `UEquipmentDefinition` gets an `FGrantSet`. `FWeaponConfig` folds into it: abilities into the grant set, handedness into the socket. AnimClass is dropped, since both weapons use `ABP_Unarmed`.
3. Equip gives the grant set with SourceObject = the item instance, and stores the receipt on the `UEquipmentInstance`. Unequip revokes the receipt first (the running ability ends), then destroys the actor.
4. Ability helpers: get the source item, a stat from it, and its equipment actor. `GA_MeleeAttack_Base` uses these instead of `WeaponsManagerComponent`.
5. Attack moves to `InputTag.Attack.Primary`. Add a sword item next to the staff item.
6. **Delete in this order:** Blueprint references first (Reference Viewer), then `WeaponsManagerComponent` and the BP equipment manager, then `FWeaponConfig` and the old `GrantAbilities`/`RemoveAbilities`. Do a full editor rebuild after the C++ deletions.

**Teaches:** provenance through SourceObject, receipts and clean revoke, safe deletion of Blueprint-referenced C++ types.

**Verify:**
- Pick up and equip the staff: Staff Bolt appears on the bar and fires.
- Swap to the sword mid-swing: the swing ends cleanly, and sword hits never hurt the wielder.
- Gameplay Debugger abilities view shows the item instance as source.
- Equip and unequip 20 times: the granted abilities and active effects return to the baseline.
- The Reference Viewer finds nothing left for the deleted types.

### F5: C++ inventory
**Goal:** one C++ inventory owns the items a character carries. Items travel as data (`FStackInfo`: definition, count, stats) and are rebuilt into instances by one factory, so nothing is lost on drop and pickup.

Sub-steps:
1. First write down what the BP inventory does for a fixed pickup sequence. That is your reference. (Recorded 2026-10-20: coin 429; crystal 10 + 6; 4 scrolls of 1 with durability 3; staff; sword. 9 stacks.)
2. Port the stacking algorithm into `UInventoryComponent`. Add the **single item factory** (`UItemInstance::CreateItem`): the only place instances are created, and the place a roll helper is called later.
3. Pickup, drop and use:
   - Pickups (placed and dropped) hold one `FStackInfo`. Picking up calls the inventory with that data; the factory builds the instances.
   - Drop turns the stack into `FStackInfo` (definition, count, the instance's stats), unequips and removes it, then spawns a pickup with that data. The old instance is discarded.
   - Use keeps the durability counters (they are stats, so they survive drop and pickup).
4. `UItemAction::Execute` becomes a BlueprintNativeEvent. The apply-effect and use-ability actions pass the item as SourceObject.
5. Widgets bind to the C++ component.
6. Delete the BP inventory and `BPFL_ItemUtils`. `FStackInfo` stays.

**Rule:** everything an item remembers is a stat in its stats map. `UItemInstance` holds nothing else, or a drop would silently lose it.

**Teaches:** UObject ownership (Outer, strong UPROPERTY owners vs the weak SourceObject), data vs runtime objects (the data form is what moves and what saves store), porting Blueprint logic against a recorded reference.

**Verify:**
- The recorded pickup sequence gives the same stacks.
- Drop the equipped staff: it unequips, and picking it up gives an item with the same definition and stats.
- A scroll used once still has 2 uses after drop and pickup.
- Consumables still work.

### F6: Item stats
**Goal:** items carry stat values, and those values become attribute modifiers only while equipped.

Sub-steps:
1. Rename `FlatDamage` to `BonusDamage`, and add `IncreasedDamage` (base 0, AddBase) replacing `DamageMultiplier`. The damage execution computes (Base + Bonus) × (1 + Increased). GE assets reference attributes by property name, so add a property redirect for the rename, or re-pick the attribute in the few GEs that use it, then resave.
2. `GE_Stat_*` archetypes: Infinite, StackingType None, one SetByCaller modifier each.
3. A stat definitions data asset (referenced from a developer-settings class, like `UUIManagerSettings`) maps each `Item.Stat.*` tag to its GE and display format.
4. A stat-range fragment rolls through the one roll helper, but only for stats the incoming data doesn't already have. A brand-new item rolls; an item rebuilt from data (pickup of a dropped item, later a loaded save) keeps its rolls. Rolls and counters share the one stats map; the stat definitions decide which stats become GEs (durability has none).
5. On equip, one stat GE per rolled stat is applied with the item as source and stored on the receipt.
6. Tooltips format stats through the stat definitions (real names, roll ranges).
7. Retire `GE_FlatDamage`/`GE_DamageMultiplier` in favour of test items.

**Teaches:** SetByCaller archetype GEs, why rolled stats never use GE stacking, the aggregator ops.

**Verify:**
- Two staff pickups show different rolls.
- Drop a staff and pick it up: its rolls are unchanged (no re-roll).
- Equipping one: `AbilitySystem.DebugAttribute BonusDamage IncreasedDamage` matches the tooltip, and a bolt deals (Base + Bonus) × (1 + Increased) (compute it by hand first).
- Swapping staffs replaces the values instead of adding them.
- No "not set by caller" errors in the log.

---

## Todo (later, not part of the foundation)

Each item plugs into the grant door rather than replacing it.

- **Combat events + procs** ("on hit: chance to ignite"): one damage report routed as gameplay events, event-triggered abilities, and chain-termination rules. F0's damage report is the starting point.
- **Hit choke point:** apply every hit through one attacker-side function, with a per-target effect context (needed for pierce/AoE correctness and procs).
- **Ability modifiers:** tag-scoped behaviour stats (pierce, extra projectiles, area) read when an ability activates, plus a stat sheet that shows conditional bonuses.
- **More damage buckets:** "more" multipliers, crit, target-side damage taken, resistances.
- **New kinds of sources:** upgrade cards, carried relics (counted, scaling by copies), enemy modifiers. All are grant sets with a different container. Add a per-character source registry when the second container appears.
- **Statuses beyond Burn**, status interactions.
- **Loot tables and rewards:** weighted lists of item classes (`TSubclassOf`/soft class references to the existing Blueprint definitions). Seeded RNG through the roll helper.
- **Run structure:** run state outside the pawn, arena travel, death and restart, saves and meta progression.
- **Data-asset definitions:** only if Asset Manager enumeration or save IDs ever require it. The Asset Manager can also scan Blueprint classes.
- **Polish:** C++ melee trace task, enemy attacks as abilities, MoveSpeed/regen attributes, pooling, MVVM.

## CLAUDE.md updates to make (when you are ready)

**Decisions to record:**
- GAS chosen
- single player only
- the grant door rule (one way in and out; the source is the SourceObject)
- the damage formula
- the max Health/Mana rule (raise adds, lower clamps)

**Drift to fix:**
- Content root is `Content/Rougelike`, not `_Game`, and the map path changed with it.
- The active plan link should point to this file.
- The source folder list is out of date.
- The `TopDownTemplate` note is stale.
