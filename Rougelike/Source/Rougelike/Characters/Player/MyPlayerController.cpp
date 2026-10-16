// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/Player/MyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToPlayerScreen();
		}
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext *CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AMyPlayerController::PlayerTick(float DeltaTime)
{
	// Before Super: Super processes input, so a Fire() this frame already sees this frame's cursor
	UpdateAimLocation();
	UpdateFacing();

	Super::PlayerTick(DeltaTime);
}

void AMyPlayerController::UpdateFacing()
{
	const APawn *ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// Yaw only: the character movement component turns the body toward the control rotation
	const FVector ToAim = AimLocation - ControlledPawn->GetActorLocation();
	SetControlRotation(FRotator(0.f, ToAim.Rotation().Yaw, 0.f));
}

void AMyPlayerController::UpdateAimLocation()
{
	const APawn *ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	FVector RayOrigin;
	FVector RayDirection;
	// Fails while the cursor is outside the viewport. A ray that isn't pointing down never reaches the plane.
	if (!DeprojectMousePositionToWorld(RayOrigin, RayDirection) || RayDirection.Z > -UE_KINDA_SMALL_NUMBER)
	{
		return;
	}

	// On flat ground the pawn's center height is also an enemy's center height,
	// so the cursor over an enemy's body lands near that enemy's center
	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FPlane AimPlane(PawnLocation, FVector::UpVector);
	FVector NewAimLocation = FMath::RayPlaneIntersection(RayOrigin, RayDirection, AimPlane);

	const FVector FlatOffset = FVector(NewAimLocation.X - PawnLocation.X, NewAimLocation.Y - PawnLocation.Y, 0.f);
	if (FlatOffset.SizeSquared() < FMath::Square(MinAimDistance))
	{
		const FVector Direction = FlatOffset.GetSafeNormal2D(UE_SMALL_NUMBER, ControlledPawn->GetActorForwardVector());
		NewAimLocation = PawnLocation + Direction * MinAimDistance;
	}

	AimLocation = NewAimLocation;

	DrawDebugSphere(GetWorld(), AimLocation, 20.f, 12, FColor::Green); // TEMP
}
