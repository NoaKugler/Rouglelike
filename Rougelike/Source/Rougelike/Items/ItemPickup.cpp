#include "Items/ItemPickup.h"
#include "Items/ItemDefinition.h"
#include "Items/Fragments/ItemFragment_WorldRepresentation.h"
#include "Inventory/InventoryComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"

AItemPickup::AItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(32.f);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SetRootComponent(Collision);

	// Purely visual: the sphere decides pickup, so the mesh can be any size
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Mesh->SetGenerateOverlapEvents(false);
}

void AItemPickup::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	ApplyWorldRepresentation();

#if WITH_EDITOR
	// Only pickups placed in the level listen; runtime spawns read fresh data anyway
	const UWorld *World = GetWorld();
	if (World && World->WorldType == EWorldType::Editor && !PropertyChangedHandle.IsValid())
	{
		PropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddUObject(this, &AItemPickup::HandleObjectPropertyChanged);
	}
#endif
}

void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	// PIE copies placed pickups without re-running construction, so refresh from the current data
	ApplyWorldRepresentation();
}

void AItemPickup::NotifyActorBeginOverlap(AActor *OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Two of our components can start overlapping in the same frame
	if (IsActorBeingDestroyed())
	{
		return;
	}

	UInventoryComponent *Inventory = OtherActor ? OtherActor->FindComponentByClass<UInventoryComponent>() : nullptr;
	if (!Inventory || !ItemStack.ItemDefinition)
	{
		return;
	}

	Inventory->AddItem(ItemStack.ItemDefinition, ItemStack.Count, ItemStack.ItemStats);
	Destroy();
}

void AItemPickup::ApplyWorldRepresentation()
{
	const UItemFragment_WorldRepresentation *WorldFragment = Cast<UItemFragment_WorldRepresentation>(UItemDefinition::FindFragmentByClass(ItemStack.ItemDefinition, UItemFragment_WorldRepresentation::StaticClass()));
	Mesh->SetStaticMesh(WorldFragment ? WorldFragment->ItemMesh : nullptr);
	Mesh->SetRelativeScale3D(WorldFragment ? WorldFragment->GetItemScale3D() : FVector::OneVector);
}

#if WITH_EDITOR
void AItemPickup::BeginDestroy()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(PropertyChangedHandle);
	PropertyChangedHandle.Reset();

	Super::BeginDestroy();
}

void AItemPickup::HandleObjectPropertyChanged(UObject *Object, FPropertyChangedEvent &PropertyChangedEvent)
{
	// Fragments are instanced subobjects of the definition's CDO, so IsIn catches edits to them too
	const UObject *DefinitionCDO = ItemStack.ItemDefinition ? ItemStack.ItemDefinition->GetDefaultObject() : nullptr;
	if (DefinitionCDO && Object && (Object == DefinitionCDO || Object->IsIn(DefinitionCDO)))
	{
		ApplyWorldRepresentation();
	}
}
#endif
