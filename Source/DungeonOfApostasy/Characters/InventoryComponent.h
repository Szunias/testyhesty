#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class ABaseItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONOFAPOSTASY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxWeight;


	// weight functions
	bool CanAddItemToInventory(ABaseItem* item);
	int GetCurrentWeight();

	int GetMaxWeight();

	// Inventory Modification functions
	void AddItemToInventory(ABaseItem* item);

	int GetCurrentItemAmount();
	ABaseItem* GetItemAtSlot(int slot);
	void RemoveItemFromInventory(ABaseItem* item);
	void RemoveItemFromInventory(int itemIdx);

	// item functionality functions
	UFUNCTION(BlueprintCallable) // for debugging 
	void UseItem(int slot);
	void UseItem(ABaseItem* item);


protected:
	virtual void BeginPlay() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemPickup);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnItemPickup OnItemPickup;

private:
	UPROPERTY(EditAnywhere) // this way we can debug this variable
	TArray<ABaseItem*> Items;
};
