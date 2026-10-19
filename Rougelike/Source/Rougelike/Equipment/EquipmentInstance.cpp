// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/EquipmentInstance.h"
#include "Equipment/EquipmentDefinition.h"
#include "Items/Fragments/ItemFragment_Equippable.h"
#include "Items/ItemInstance.h"
#include "GameFramework/Character.h"
#include "EquipmentInstance.h"

void UEquipmentInstance::Initialize(UItemInstance *InSourceItemInstance)
{
    if (!InSourceItemInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentInstance::Initialize called with null SourceItemInstance"));
        return;
    }

    SourceItemInstance = InSourceItemInstance;

    const UItemFragment_Equippable *EquippableFragment = Cast<UItemFragment_Equippable>(
        SourceItemInstance->FindFragmentByClass(UItemFragment_Equippable::StaticClass()));
    if (EquippableFragment)
    {
        EquipmentDefinition = EquippableFragment->EquipmentDefinition;
    }
}

void UEquipmentInstance::HandleEquipItem(ACharacter *Character)
{
    UEquipmentDefinition *DefinitionCDO = EquipmentDefinition.GetDefaultObject();
    if (!DefinitionCDO)
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentInstance::HandleEquipItem called with null EquipmentDefinition"));
        return;
    }

    EquipmentActor = GetWorld()->SpawnActor(DefinitionCDO->EquipmentActorClass);
    if (!EquipmentActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentInstance::HandleEquipItem failed to spawn EquipmentActor"));
        return;
    }

    EquipmentActor->AttachToComponent(
        Character->GetMesh(),
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        DefinitionCDO->EquipmentSocketName);

    // Apply Gameplay Effects, Stats,...
    // Keep Track of Effects etc
}

void UEquipmentInstance::HandleUnequipItem(ACharacter *Character)
{
    // Remove Effects, Stats etc

    if (EquipmentActor)
    {
        EquipmentActor->Destroy();
        EquipmentActor = nullptr;
    }
}
