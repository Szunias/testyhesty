
#include "Chest.h"
#include "Components/SkeletalMeshComponent.h"
#include "TarotSpawnerComponent.h"
#include "TarotSpawner.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "../Characters/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BillboardComponent.h"
#include "../Items/BaseItem.h"
#include "../Items/ItemHolyPendant.h"
#include "../Items/ItemTarotBox.h"
#include "../Items/ItemSuspiciousPotion.h"
#include "../Items/ItemRottenMeat.h"
#include "../Items/ItemSmokeBomb.h"
#include "../Items/WeaponDagger.h"
#include "../Items/WeaponSword.h"
#include "../Widgets/SpawnedItemScreen.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BillboardComponent.h"

AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = false;
	HasBeenOpened = false;
}

void AChest::BeginPlay()
{
	Super::BeginPlay();
	
	Mesh = GetComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(Mesh))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Mesh in Chest not found!!!"));
	}

	TarotSpawnerComponent = GetComponentByClass<UTarotSpawnerComponent>();
	if (!IsValid(TarotSpawnerComponent))
	{
		UE_LOG(LogTemp, Fatal, TEXT("TarotSpawner in Chest not found!!!"));
	}

	ItemSpawnLocation = Cast<UBillboardComponent>(GetDefaultSubobjectByName(TEXT("ItemSpawn")));
	if (!IsValid(ItemSpawnLocation))
	{
		UE_LOG(LogTemp, Fatal, TEXT("ItemSpawnLocation (billboard) in Chest not found!!!"));
	}

	InteractIcon = Cast<UBillboardComponent>(GetDefaultSubobjectByName(TEXT("Interact")));
	if (!IsValid(InteractIcon))
	{
		UE_LOG(LogTemp, Fatal, TEXT("InteractIcon (billboard) in Chest not found!!!"));
	}

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AChest::OnInteractOverlapBegin);
	Mesh->OnComponentEndOverlap.AddDynamic(this, &AChest::OnInteractOverlapEnd);

	OnContinueOpening.AddDynamic(this, &AChest::ContinueOpening);
}

void AChest::Interact()
{
	if (HasBeenOpened)
		return;

	// TODO: Play chest opening sound here

	HasBeenOpened = true;

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	PlayerCharacter->ToggleInteractMode(true, CameraTrigger);

	OnStartOpening.Broadcast();

	Mesh->OnComponentBeginOverlap.Clear();
	Mesh->OnComponentEndOverlap.Clear();
	InteractIcon->SetHiddenInGame(true);
}

void AChest::ContinueOpening()
{
	ATarotSpawner* spawner{ TarotSpawnerComponent->DrawCards({ETarotCards::Devil, ETarotCards::Fool, ETarotCards::Hermit, ETarotCards::Strength, ETarotCards::Temperance, ETarotCards::Tower}) };
	spawner->ResultHandlerDelegate.AddDynamic(this, &AChest::HandleCardsDrawn);
}

void AChest::HandleCardsDrawn(ETarotCards pickedCard, float effectDelay)
{
	FTimerHandle executeEffectHandle{};
	FTimerManager& timerManager{ GetWorld()->GetTimerManager() };

	TarotSpawnerComponent->SetTarotTextImage(TarotSpawnerComponent->CardTextImages[pickedCard]);

	timerManager.SetTimer(executeEffectHandle, FTimerDelegate::CreateLambda([&, pickedCard]
		{ 
			switch (pickedCard)
			{
			case ETarotCards::Devil:
			{
				if (FMath::RandRange(0, 1))
				{
					AItemSuspiciousPotion* tempItem = GetWorld()->SpawnActorDeferred<AItemSuspiciousPotion>(SuspiciousPotionClass, ItemSpawnLocation->GetComponentTransform());
					tempItem->HealthRangeMin = -5;
					tempItem->HealthRangeMax = 5;
					tempItem->StrengthRangeMin = -5;
					tempItem->StrengthRangeMax = 5;
					tempItem->LuckRangeMin = -5;
					tempItem->LuckRangeMax = 5;
					// turn ranges into variables?
					if (2 >= FMath::RandRange(0, 100)) // 2 % chance
					{
						tempItem->IsFatalPotion = true;
					}
					ItemToSpawn = tempItem;
				}
				else
				{
					ItemToSpawn = GetWorld()->SpawnActorDeferred<AItemRottenMeat>(RottenMeatClass, ItemSpawnLocation->GetComponentTransform());
				}
			}
				break;
			case ETarotCards::Hermit:
				if (FMath::RandRange(0, 1))
				{
					ItemToSpawn = GetWorld()->SpawnActorDeferred<AItemHolyPendant>(HolyPendantClass, ItemSpawnLocation->GetComponentTransform());
				}
				else
				{
					ItemToSpawn = GetWorld()->SpawnActorDeferred<AItemTarotBox>(TarotBoxClass, ItemSpawnLocation->GetComponentTransform());
				}
				break;
			case ETarotCards::Tower:
			{
				AItemSuspiciousPotion* tempItem = GetWorld()->SpawnActorDeferred<AItemSuspiciousPotion>(SuspiciousPotionClass, ItemSpawnLocation->GetComponentTransform());
				tempItem->HealthRangeMin = -5;
				tempItem->HealthRangeMax = 0;
				tempItem->StrengthRangeMin = -5;
				tempItem->StrengthRangeMax = 0;
				tempItem->LuckRangeMin = -5;
				tempItem->LuckRangeMax = 0;
				// turn ranges into variables?
				if (5 >= FMath::RandRange(0, 100)) // 2 % chance
				{
					tempItem->IsFatalPotion = true;
				}
				ItemToSpawn = tempItem;
			}
				break;
			case ETarotCards::Strength:
				if (FMath::RandRange(0, 1))
				{
					ItemToSpawn = GetWorld()->SpawnActorDeferred<AWeaponDagger>(DaggerClass, ItemSpawnLocation->GetComponentTransform());
				}
				else
				{
					ItemToSpawn = GetWorld()->SpawnActorDeferred<AWeaponSword>(SwordClass, ItemSpawnLocation->GetComponentTransform());
				}
				break;
			case ETarotCards::Fool:
				PlayerCharacter->ToggleInteractMode(false);
				return;
				break;
			case ETarotCards::Temperance:
				if (FMath::RandRange(0, 1))
				{
					ItemToSpawn = GetWorld()->SpawnActorDeferred<AItemSmokeBomb>(SmokeBombClass, ItemSpawnLocation->GetComponentTransform());
				}
				else
				{
					AItemSuspiciousPotion* tempItem = GetWorld()->SpawnActorDeferred<AItemSuspiciousPotion>(SuspiciousPotionClass, ItemSpawnLocation->GetComponentTransform());
					tempItem->HealthRangeMin = 15;
					tempItem->HealthRangeMax = 25;
					tempItem->StrengthRangeMin = 3;
					tempItem->StrengthRangeMax = 8;
					tempItem->LuckRangeMin = 0;
					tempItem->LuckRangeMax = 10;
					ItemToSpawn = tempItem;
				}
				break;
			}

			

			ItemScreenWidget = CreateWidget<USpawnedItemScreen>(GetWorld(), SpawnedItemScreenClass);
			ItemScreenWidget->InitializeWidget(PlayerCharacter->GetInventory()->CanAddItemToInventory(ItemToSpawn), ItemToSpawn);
			ItemScreenWidget->AddWidget();
			ItemScreenWidget->OnOptionPicked.AddDynamic(this, &AChest::FinishSpawningItem);
		}
	), effectDelay, false);

	
}

void AChest::FinishSpawningItem(bool pickUp)
{
	ItemScreenWidget->RemoveWidget();
	PlayerCharacter->ToggleInteractMode(false);

	if (pickUp)
	{
		// the player can and should pick it up, spawn it inside the player
		ItemToSpawn->FinishSpawning(PlayerCharacter->GetActorTransform());
		ItemToSpawn->Interact();
	}
	else
	{
		ItemToSpawn->FinishSpawning(ItemSpawnLocation->GetComponentTransform());
	}
}
