#include "Core/BaseCharacter.h"
#include "Combat/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter()
{
    // Movement and animation tick in their own components; the actor itself has nothing to do per frame
    PrimaryActorTick.bCanEverTick = false;

    // The movement component turns every character. Each subclass picks how (cursor, movement direction).
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Same subobject name as before the move, so Blueprint overrides on it (Max Health) carry over
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    HealthComponent->OnDeath.AddDynamic(this, &ABaseCharacter::HandleDeath);
}

void ABaseCharacter::HandleDeath()
{
    // Every character stops moving when it dies; subclasses add what death means for them
    GetCharacterMovement()->DisableMovement();
}
