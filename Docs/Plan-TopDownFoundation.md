# Plan: Top-Down Foundation

**Goal:** turn the third-person shooter prototype into a clean top-down base for an ARPG × roguelike.
WASD moves relative to the screen, the character faces the mouse cursor, and the gun fires at the cursor.
Everything that already works (enemies, projectiles, damage, HUD) carries over.

- **Branch:** `Top-Down` for Parts A and B, then merge. A separate branch for Part C.
- **Rhythm:** one milestone = one commit = one thing to check in PIE.
- **Reference project:** `TopDownTemplate/` (gitignored). Read, don't copy:
  - `Variant_TwinStick/TwinStickCharacter.cpp`: camera setup and mouse aim.
  - `TopDownTemplateCharacter.cpp`: the minimal top-down camera.

---

## Where we start

| Area | Status | Action |
|---|---|---|
| `AProjectile`, collision channels | Works and doesn't depend on the camera | Keep |
| `ABaseEnemy` chase and attack | Works. `MoveToActor` already uses the NavMesh | Keep |
| `UPlayerHUD`, `UEnemyHealthBar` | Works, good delegate pattern | Keep, rebind in Part C |
| `ARougelikeCharacter` camera and movement | Third-person orbit, movement based on control rotation | **Change (B1, B2, B4)** |
| `ARougelikePlayerController` | Input mapping contexts, HUD, touch controls | **Change (A1, B3)** |
| `AGun::GetAimDirection` | Traces from the camera (`GetPlayerViewPoint`) | **Change (B5)**. This is what breaks with a top-down camera |
| Look and mouse-look input, touch controls, crosshair | Third-person or mobile only | **Remove (A1, B5)** |
| Health | Duplicated in the character and in `ABaseEnemy` | **Extract (C1)** |
| `BP_ThirdPerson*`, `Lvl_ThirdPerson` | Template naming, third-person level | **Rename or replace (A2, A3)** |

---

## The core idea: who owns aiming?

Today the gun asks the camera where to shoot. That is why changing the camera breaks the gun. After this plan, each class has one job:

- **Controller = intent.** It turns the input device (mouse) into a world-space aim point. It is the *only* class that knows about the cursor, the screen or the camera.
- **Character = body.** It moves, turns toward the aim, and holds the weapon.
- **Gun = tool.** It fires at a point it's given, and knows nothing about cameras, cursors or controllers.

The payoff: gamepad stick aiming, click-to-move later, or an AI using the same gun only means writing a new "intent" source. The gun and character don't change.

**Control rotation:** in UE, the controller's `ControlRotation` means "where this player wants to look". In top-down we set it to face the aim point, and the movement component turns the body toward it (`bUseControllerDesiredRotation`). This needs no Tick on the character. The catch is that movement can no longer use control rotation as its "forward" (B2).

---

## Part A: Clean slate (mostly editor work, no new features)

### A1. Remove third-person look and touch input
- **Character:** remove `LookAction`, `MouseLookAction`, `Look()` and `DoLook()`. **Jump stays** (decided; a dash may replace it later).
- **Controller:** remove the touch path (`MobileControlsWidgetClass`, `MobileExcludedMappingContexts`, `bForceTouchControls`, `ShouldUseTouchControls()` and the `SVirtualJoystick` include).
- **Content:** delete `IA_Look`, `IA_MouseLook`, `IMC_MouseLook` and `Input/Touch/`. Delete from the Content Browser so the editor checks references first.
- **While you're in these files:**
  - Switch raw pointer `UPROPERTY`s to `TObjectPtr`: `UInputAction*`, `TArray<UInputMappingContext*>`, `CameraBoom` and `FollowCamera`.
  - Remove the leftover `LogTemplateCharacter` declaration.
- **Check in PIE:** it compiles and runs, WASD and fire work, and the camera no longer rotates (expected).

### A2. Rename and tidy content and config
- **Rename in the editor:**
  - `BP_ThirdPersonCharacter` → e.g. `BP_PlayerCharacter`
  - `BP_ThirdPersonPlayerController` → `BP_PlayerController`
  - `BP_ThirdPersonGameMode` → `BP_GameMode`

  Then right-click the folders and choose **Fix Up Redirectors**.
- **Stale config:** `DefaultEngine.ini` → `GlobalDefaultGameMode` still points to `/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode`, which no longer exists because the asset lives in `_Game/Core`. Set it again under *Project Settings → Maps & Modes*, and check whether the level's *World Settings* override it.
- **Comments:** update the doc comments that still say "third person" or "crosshair" (character, controller and game mode headers, `Gun.h`).
- **Lesson:**
  - **Moving** a C++ file to another folder is free for Blueprints, because the class path is `/Script/Rougelike.ClassName` with no folder in it.
  - **Renaming** a C++ class is not free: it needs a `CoreRedirects` entry, like the `TP_ThirdPerson*` redirects already in `DefaultEngine.ini`.
  - Don't rename the `CameraBoom` and `FollowCamera` subobjects either. Blueprint component overrides are stored by subobject name.
- **Check in PIE:** restart the editor. The Message Log shows no missing-class or missing-asset warnings and PIE still works.

### A3. Top-down arena level
- **Create the level:** a new **Basic** level (not Open World, because an arena doesn't need World Partition) at `_Game/Maps/Lvl_Arena`.
- **Fill it:** a floor and a few obstacles from `LevelPrototyping`, a `PlayerStart`, lighting, a few `BP_DefaultEnemy`, and a **NavMeshBoundsVolume**. Press `P` in the viewport to see the NavMesh.
- **Make it the default:** set it as both the Editor Startup Map and the Game Default Map.
- **Remove the old level:** once the Reference Viewer shows nothing pointing into `Content/ThirdPerson/`, delete that folder.
- **Check in PIE:** enemies path around obstacles to reach you.

---

## Part B: Top-down controls

### B1. Camera
- **Boom settings:**

  | Setting | Value |
  |---|---|
  | `SetUsingAbsoluteRotation(true)` | so the boom ignores the pawn's rotation |
  | Pitch | about −50° to −60° |
  | `TargetArmLength` | about 1200–2000 |
  | `bDoCollisionTest` | `false` |
  | `bUsePawnControlRotation` | `false` on the boom and the camera |
  | Camera lag | optional |

- **Tune first, then commit to C++:** find the values in the Blueprint (PIE, eject with F8, tweak), then write them into the C++ constructor.
- **Pitfall:** values overridden in the Blueprint take priority over C++ defaults. After moving values into C++, reset the Blueprint's overrides (the yellow reset arrow), or your C++ changes will seem to do nothing.
- **Check in PIE:** fixed angle, the camera follows, and the mouse moves the cursor, not the camera.

### B2. Screen-relative movement
- `DoMove`: build forward and right from the **camera's (or boom's) yaw**, not from `GetControlRotation()`.
- **Why:** in B4, control rotation becomes "where I aim". If movement still used it, W would walk toward the cursor. That's a valid scheme, but not the one you chose.
- **Check in PIE:** temporarily set the boom yaw to 45°. W must still move "up the screen".

### B3. Cursor → aim point (controller)
- **Steps:**
  1. Set `bShowMouseCursor = true`.
  2. Override `PlayerTick` (it runs only for local player controllers, once per frame).
  3. Call `DeprojectMousePositionToWorld` to get a ray.
  4. Intersect the ray with a horizontal plane at muzzle height using `FMath::RayPlaneIntersection` and `FPlane(Origin, FVector::UpVector)`.
  5. Cache the result and expose it as a const getter, e.g. `GetAimLocation()`.
- **Why a plane and not `GetHitResultUnderCursorByChannel`** (which TwinStick uses): a trace hits whatever is under the cursor, such as enemy heads, walls or props. The aim height then jumps around, and bullets dive into the floor or fly over targets. A plane gives a stable, flat aim point and costs no trace.
  - Later you will *also* want a trace for **hover** (highlighting the enemy or loot under the cursor). That answers a different question, "what's under the cursor?" versus "where am I aiming?". Keep the two separate.
- **Guard:** if the ray doesn't point down toward the plane (dot product with up ≥ 0), keep the last aim point.
- **Cost:** one deproject and a little vector math per frame. That's negligible, and it lives in one place if profiling ever says otherwise.
- **Pitfall:** if the cursor vanishes while a button is held, or the first click in PIE is swallowed, look at `SetInputMode` and `FInputModeGameAndUI` (`SetHideCursorDuringCapture`).
- **Check in PIE:** temporarily draw a `DrawDebugSphere` at the aim point. It should sit under the cursor at waist height.

### B4. Face the aim
- **Controller:** in `PlayerTick`, `SetControlRotation` to the yaw from the pawn to the aim point (pitch 0).
- **Character:**
  - `bOrientRotationToMovement = false`
  - `bUseControllerDesiredRotation = true`
  - a high yaw `RotationRate` (720–1440), or snap with `bUseControllerRotationYaw = true`
- **Pitfall:** if both rotation modes are on, orient-to-movement wins (`UCharacterMovementComponent::PhysicsRotation`), so turn it off.
- **Decided:** always face the cursor (the least effort). Turning toward the cursor only while firing can come later by toggling those two flags.
- **Known visual issue, acceptable for now:** the current anim Blueprint assumes the character faces where it walks. Walking sideways or backwards while facing the cursor will slide or moonwalk. The fix is a direction-aware (strafe) blendspace in a later animation pass.
- **Check in PIE:** the body turns with the cursor and WASD stays screen-relative.

### B5. The gun fires at the aim point
- **New signature:** something like `void Shoot(const FVector& TargetLocation);`.
  - `Fire()` on the character gets the aim point from its controller and passes it in.
  - Delete the camera trace and `AimTraceDistance`. `ECC_Weapon` becomes unused. Keep the channel, because hover or hitscan may need it.
- **Flatten the direction** (zero its Z) so bullets fly level at muzzle height.
- **Pitfall:** when the cursor is very close to or behind the muzzle, the muzzle-to-target direction flips. Take the direction from the pawn's center, or enforce a minimum aim distance, and still spawn at the muzzle.
- **Decision, push or pull:**
  - **Push (now):** the character passes the point in. It's the simplest option.
  - **Pull (later):** the gun asks its owner through an interface on every shot. That's worth it once the gun times its own auto-fire, or once enemies get guns.
  - The switch only touches `AGun` and whoever implements the interface.
- **HUD:** remove the crosshair from `WBP_Hud`.
- **Check in PIE:** bullets fly level to the cursor and hit an enemy under it. With the cursor next to the character, it never fires backwards.

### B6. Hold to fire
- Bind fire on `ETriggerEvent::Triggered` instead of `Started`. The gun's existing `FireRate` check does the rest.
- **Trade-off:** shots snap to frame boundaries. That's fine at 5 shots per second and 60 fps, but very high fire rates or low frame rates lose shots. The proper version is `StartFire`/`StopFire` plus a timer owned by the gun, and that's also when the pull model from B5 makes sense.
- **Check in PIE:** holding LMB fires steadily until the magazine is empty. There's no reload yet.

**→ Merge `Top-Down` into `main`.**

---

## Part C: Shared combat base (new branch)

### C1. `UHealthComponent`
- **Why now:** health is copied in `ARougelikeCharacter` and `ABaseEnemy`. Every future thing that can die (destructibles, bosses, summons) would copy it again.
- **Where:** `Source/Rougelike/Combat/`. That's a feature folder, not the type folder `Components/`, so delete the empty `Components/`.
- **Shape:**
  - A `UActorComponent` with no tick that owns max and current health.
  - In `BeginPlay` it subscribes to the owner's `OnTakeAnyDamage`. `AActor::TakeDamage` broadcasts that, so owners no longer override `TakeDamage`.
  - It broadcasts `OnHealthChanged(Current, Max)` and `OnDeath`, the latter exactly once.
- **Who listens:**
  - The enemy binds `OnDeath` to `Die()`.
  - The player binds `OnDeath` to a minimal death state: disable input, stop movement, show "you died" text.
  - `UPlayerHUD` and `UEnemyHealthBar` bind to the component. The enemy health bar becomes event-driven instead of being updated by the enemy.
- **Pitfalls:**
  - `OnTakeAnyDamage` also fires for **negative** damage. Decide whether that means healing or should be ignored.
  - Ignore damage after death.
  - Bind in `BeginPlay`, not in the constructor.
  - The handlers must be `UFUNCTION()` because the delegate is dynamic.
- **GAS note:** if you choose GAS later, health becomes an attribute and damage a Gameplay Effect. The component's *public face* (`OnHealthChanged`, `OnDeath`, `GetHealth`) survives as a thin wrapper around the ability system; Lyra's `ULyraHealthComponent` does exactly this. So this work isn't thrown away.
- **Check in PIE:** enemies die, the player dies, and both health displays update. No health logic is left in either owner.

### C2. Folder layout
Target layout (moving files is safe for Blueprints, see A2; update the `#include` paths):
```
Source/Rougelike/
  Core/        GameMode (later: subsystems)
  Player/      Character, PlayerController, UI/
  Combat/      HealthComponent (later: damage types, damage events)
  Weapons/     Gun, Projectile        (rename of Gun/)
  Enemy/       BaseEnemy, UI/
Content/_Game/
  Core/  Maps/  Input/  Features/{Player, Enemy, Weapons}
```

---

## Deliberately *not* in this plan
- **Click-to-move and hover-highlighting** of enemies and loot under the cursor.
- **A weapon base class or abilities.** Wait for a second weapon type. This is tied to the GAS decision.
- **GAS vs custom attributes and modifiers.** This is the **next big decision**, to make before items and loot. An ARPG has many stats and stacking modifiers, which leans toward GAS, but it costs more to learn.
- **Projectile pooling.** Profile first (roadmap step 7).
- **Polish:** strafe animations, cursor widget, camera feel, reload and ammo UI.
