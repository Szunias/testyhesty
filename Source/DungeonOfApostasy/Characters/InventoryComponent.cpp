#include "InventoryComponent.h"
#include "../Items/BaseItem.h"
#include "../Items/BaseWeapon.h"
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


bool UInventoryComponent::CanAddItemToInventory(ABaseItem* item)
{
	if (item->GetItemWeight() + GetCurrentWeight() > MaxWeight)
		return false;

	return true;
}

int UInventoryComponent::GetCurrentWeight()
{
	int CurrentWeight{ 0 };

	for (ABaseItem* item : Items)
	{
		CurrentWeight += item->GetItemWeight();				
	}

	return CurrentWeight;
}

int UInventoryComponent::GetMaxWeight()
{
	return MaxWeight;
}

void UInventoryComponent::AddItemToInventory(ABaseItem* item)
{
	OnItemPickup.Broadcast();
	Items.Add(item);
}

void UInventoryComponent::RemoveItemFromInventory(ABaseItem* item)
{
	Items.Remove(item);
}

void UInventoryComponent::RemoveItemFromInventory(int itemIdx)
{
	Items.RemoveAt(itemIdx);
}

void UInventoryComponent::UseItem(int slot)
{
	if (slot >= Items.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Trying to use Invalid Slot");
		return;
	}

	ABaseItem* item{ Items[slot] };
	if (IsValid(item))
	{
		UseItem(item);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Trying to use Invalid Item");
	}
}

void UInventoryComponent::UseItem(ABaseItem* item)
{
	if (Items.Find(item) == INDEX_NONE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Item is not in inventory");
		return;
	}

	if (IsValid(item))
	{
		UseItem(item);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Trying to use Invalid Item");
	}
}

int UInventoryComponent::GetCurrentItemAmount()
{	
	return Items.Num();	
}

ABaseItem* UInventoryComponent::GetItemAtSlot(int slot)
{
	if (slot >= Items.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Trying to use Invalid Slot");
		return nullptr;
	}
	return Items[slot];
}



void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();


}
