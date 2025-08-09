#include "BaseItem.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "../Characters/InventoryComponent.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BillboardComponent.h"
#include "NiagaraComponent.h"

ABaseItem::ABaseItem()
{
	ItemName = "BASEITEM";
	ItemDescription = "DEFAULT DESCRIPTION";
	HasDelayedEffect = false;
	PickupAble = true;
	CanItemBeThrown = true;
	IsBeingDestroyed = false;
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	Mesh = GetComponentByClass<UStaticMeshComponent>();
	if (!IsValid(Mesh))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Mesh in BaseItem not found!!!"));
	}

	InteractIcon = Cast<UBillboardComponent>(GetDefaultSubobjectByName(TEXT("Interact")));
	if (!IsValid(InteractIcon))
	{
		UE_LOG(LogTemp, Fatal, TEXT("InteractIcon (billboard) in BaseItem not found!!!"));
	}

	InteractParticles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("InteractNiagara")));
	if (!IsValid(InteractParticles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("InteractNiagara in BaseItem not found!!!"));
	}

	InteractParticles->ActivateSystem();

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnInteractOverlapBegin);
	Mesh->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnInteractOverlapEnd);
}

void ABaseItem::Use() { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "BaseItem got used, how even??"); }

void ABaseItem::Throw() { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "BaseItem got thrown, how even??"); }

void ABaseItem::Interact()
{
	if (!PickupAble)
	{
		return;
	}

	if (APlayerCharacter* player{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) }; IsValid(player))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Picked up Item");
		UInventoryComponent* playerInventory{ player->GetInventory() };
		if (playerInventory->CanAddItemToInventory(this))
		{
			playerInventory->AddItemToInventory(this);
			ShowItem(false);
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseItem::ClearCollisions);
			Mesh->OnComponentBeginOverlap.Clear();
			Mesh->OnComponentEndOverlap.Clear();
			InteractIcon->SetHiddenInGame(true);
			PickupAble = false;
			InteractParticles->Deactivate();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Inventory Full");
		}
	}
}

void ABaseItem::ActivatePhysics(bool activate)
{
	Mesh->SetSimulatePhysics(activate);
}

bool ABaseItem::CanBeThrown() const
{
	return CanItemBeThrown;
}

bool ABaseItem::HasDelayedItemEffect()
{
	return HasDelayedEffect;
}

void ABaseItem::SetPickupAble(bool isPickupAble)
{
	PickupAble = isPickupAble;
}

void ABaseItem::LaunchItem(const FVector& launchVelocity)
{
	ActivatePhysics(true);
	Throw();
	Mesh->AddImpulse(launchVelocity);
}

void ABaseItem::TeleportItem(const FVector& newLocation)
{
	ActivatePhysics(false);
	SetActorLocation(newLocation);
}

const FString& ABaseItem::GetItemName()
{
	return ItemName;
}

const FString& ABaseItem::GetItemDescription()
{
	return ItemDescription;
}

int ABaseItem::GetItemWeight()
{
	return ItemWeight;
}

UTexture2D* ABaseItem::GetItemImage()
{
	return ItemImage;
}

TSubclassOf<AActor> ABaseItem::GetItemMesh() const
{
	return MeshToEquip;
}

void ABaseItem::ShowItem(bool visible)
{
	Mesh->SetVisibility(visible);
	Mesh->SetGenerateOverlapEvents(visible);
}

void ABaseItem::HandleDestroy()
{
	// we check if this function has already been called before.
	// if it has, we don't want to destroy twice

	if (IsBeingDestroyed)
	{
		return;
	}

	IsBeingDestroyed = true;
	FTimerHandle unusedHandle{};
	FTimerDelegate destroyDelegate{ FTimerDelegate::CreateLambda([this]() {
		if (IsValid(this))
		{
			Destroy();
		}
	}) };
	GetWorld()->GetTimerManager().SetTimer(unusedHandle, destroyDelegate, 3.f, false);
}

void ABaseItem::ClearCollisions()
{
	Mesh->OnComponentBeginOverlap.Clear();
}



