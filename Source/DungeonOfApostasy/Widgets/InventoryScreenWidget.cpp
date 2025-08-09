#include "InventoryScreenWidget.h"
#include "RadialMenu.h"
#include "InventoryItemWidget.h"
#include "../Items/BaseItem.h"
#include "../Items/BaseWeapon.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"


void UInventoryScreenWidget::NativeConstruct()
{
	RadialMenu->OnItemSelected.AddDynamic(this, &UInventoryScreenWidget::SelectItem);
	InventoryItem->OnItemUsed.AddDynamic(this, &UInventoryScreenWidget::OnItemUse);
	InventoryItem->OnItemThrown.AddDynamic(this, &UInventoryScreenWidget::OnItemThrow);

	CanThrowItem = true;
}

void UInventoryScreenWidget::SelectItem(ABaseItem* item)
{
	RadialMenu->DisableAutoRegainFocus();
	InventoryItem->RestoreFocus();
	InventoryItem->SetCurrentItem(item);
	InventoryItem->EnableThrow(CanThrowItem ? item->CanBeThrown() : false);
}

void UInventoryScreenWidget::IncludeWeaponItems(bool include)
{
	RadialMenu->IncludeWeapons(include);
}

void UInventoryScreenWidget::InitializeWidget()
{
	InventoryItem->InitializeWidget();
	RadialMenu->InitializeWidget();
}

void UInventoryScreenWidget::DisableThrow()
{
	CanThrowItem = false;
	InventoryItem->EnableThrow(false);
}

bool UInventoryScreenWidget::CloseInventoryItem()
{
	if (InventoryItem->GetVisibility() == ESlateVisibility::Visible)
	{
		InventoryItem->SetVisibility(ESlateVisibility::Hidden);
		InventoryItem->DisableAutoRegainFocus();
		RadialMenu->RestoreFocus();

		return false;
	}
	OnBack.Broadcast();
	return true;
}

void UInventoryScreenWidget::NativeDestruct()
{
	Super::NativeDestruct();

	//Forcing GC to collect this widget to prevent the controller code from accessing multiple menus at the same time.
	GEngine->ForceGarbageCollection();
}

void UInventoryScreenWidget::OnItemUse(ABaseItem* usedItem)
{
	OnItemUsed.Broadcast(usedItem);
}

void UInventoryScreenWidget::OnItemThrow(ABaseItem* thrownItem)
{
	OnItemThrown.Broadcast(thrownItem);
}

