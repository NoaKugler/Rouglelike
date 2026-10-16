// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/RougelikeCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Rougelike.h"
#include "Gun/Gun.h"
#include "MyCollisionChannels.h"

ARougelikeCharacter::ARougelikeCharacter()
{

	// Our bullets spawn inside the capsule. Without this, the capsule's own movement
	// sweep collides with them and the character gets nudged when firing.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset BP_PlayerCharacter (to avoid direct content references in C++)
}

void ARougelikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultGunClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		EquippedGun = GetWorld()->SpawnActor<AGun>(DefaultGunClass, Params);
		if (EquippedGun)
		{
			EquippedGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GunSocketName);
		}
	}
}

void ARougelikeCharacter::Fire()
{
	UE_LOG(LogRougelike, Warning, TEXT("Fire pressed, gun: %s"), *GetNameSafe(EquippedGun)); // TEMP debug
	if (EquippedGun)
	{
		EquippedGun->Shoot();
	}
}

void ARougelikeCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARougelikeCharacter::Move);

		// Shooting
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ARougelikeCharacter::Fire);
	}
	else
	{
		UE_LOG(LogRougelike, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARougelikeCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ARougelikeCharacter::DoMove(float Right, float Forward)
{
	const FRotator YawRotation(0.f, FollowCamera->GetComponentRotation().Yaw, 0.f);
	const FRotationMatrix YawMatrix(YawRotation);

	AddMovementInput(YawMatrix.GetUnitAxis(EAxis::X), Forward);
	AddMovementInput(YawMatrix.GetUnitAxis(EAxis::Y), Right);
}

void ARougelikeCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Before any BeginPlay, so the HUD reads the right starting value regardless of actor order
	CurrentHealth = MaxHealth;
}

float ARougelikeCharacter::TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Applied <= 0.f)
	{
		return 0.f;
	}

	// No clamp and no death on purpose (for now)
	CurrentHealth -= Applied;
	OnHealthChanged.Broadcast(CurrentHealth);

	return Applied;
}
