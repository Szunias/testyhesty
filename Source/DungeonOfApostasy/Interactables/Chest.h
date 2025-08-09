#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Other/InteractableInterface.h"
#include "../Other/ETarotCards.h"
#include "Chest.generated.h"

class USkeletalMeshComponent;
class UTarotSpawnerComponent;
class ACameraTrigger;
class APlayerCharacter;
class ABaseItem;
class AItemHolyPendant;
class AItemTarotBox;
class AItemSuspiciousPotion;
class AItemRottenMeat;
class AItemSmokeBomb;
class AWeaponDagger;
class AWeaponSword;
class UBillboardComponent;
class USpawnedItemScreen;

UCLASS()
class DUNGEONOFAPOSTASY_API AChest : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AChest();

	virtual void Interact() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartOpening);
	UPROPERTY(BlueprintAssignable)
	FStartOpening OnStartOpening;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FContinueOpening);
	UPROPERTY(BlueprintCallable)
	FContinueOpening OnContinueOpening;

	UPROPERTY(VisibleAnywhere)
	UTarotSpawnerComponent* TarotSpawnerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACameraTrigger* CameraTrigger;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBillboardComponent* ItemSpawnLocation;

	// items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TSubclassOf<USpawnedItemScreen> SpawnedItemScreenClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TSubclassOf<AItemHolyPendant> HolyPendantClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TSubclassOf<AItemTarotBox> TarotBoxClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TSubclassOf<AItemSuspiciousPotion> SuspiciousPotionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TSubclassOf<AItemRottenMeat> RottenMeatClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TSubclassOf<AItemSmokeBomb> SmokeBombClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TSubclassOf<AWeaponDagger> DaggerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TSubclassOf<AWeaponSword> SwordClass;





protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* Mesh;

private:
	UFUNCTION()
	void ContinueOpening();

	UFUNCTION()
	void HandleCardsDrawn(ETarotCards pickedCard, float effectDelay);

	UFUNCTION()
	void FinishSpawningItem(bool pickUp);

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	USpawnedItemScreen* ItemScreenWidget;

	UPROPERTY()
	ABaseItem* ItemToSpawn;

	bool HasBeenOpened;
};
