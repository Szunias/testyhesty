
#include "InventoryItemWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "../Items/BaseItem.h"
#include "../Items/BaseWeapon.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/InventoryComponent.h"
#include "MenuButton.h"

void UInventoryItemWidget::SetItemName(const FString& name)
{
	Item_Txt->SetText(FText::FromString(name));
}

void UInventoryItemWidget::SetItemDescription(const FString& description)
{
	Description_Txt->SetText(FText::FromString(description));
}

void UInventoryItemWidget::OnItemUse()
{
	if (!IsValid(CurrentSelectedItem))
	{
		return;
	}

	ABaseWeapon* weapon{ Cast<ABaseWeapon>(CurrentSelectedItem) };
	if (IsValid(weapon))
	{
		if (weapon->IsWeaponEquipped())
		{
			weapon->Unequip();
		}
		else
		{
			weapon->Use();
		}
		
	}
	else
	{
		CurrentSelectedItem->Use();
		Player->GetInventory()->RemoveItemFromInventory(CurrentSelectedItem);
	}	

	OnItemUsed.Broadcast(CurrentSelectedItem);
}

void UInventoryItemWidget::OnItemThrow()
{
	if (!IsValid(CurrentSelectedItem))
	{
		return;
	}
	OnItemThrown.Broadcast(CurrentSelectedItem);
}

void UInventoryItemWidget::NativeConstruct()
{
	Use_Btn->GetButton()->OnClicked.AddDynamic(this, &UInventoryItemWidget::OnItemUse);
	Throw_Btn->GetButton()->OnClicked.AddDynamic(this, &UInventoryItemWidget::OnItemThrow);

	Use_Btn->OnButtonFocusGained.AddDynamic(this, &UInventoryItemWidget::SetActiveButton);
	Throw_Btn->OnButtonFocusGained.AddDynamic(this, &UInventoryItemWidget::SetActiveButton);

	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryItemWidget::SetCurrentItem(ABaseItem* item)
{
	if (IsValid(item))
	{
		SetVisibility(ESlateVisibility::Visible);
		CurrentSelectedItem = item;
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		CurrentSelectedItem = nullptr;
		return;
	}


	SetItemName(item->GetItemName());
	SetItemDescription(item->GetItemDescription());

	FString useText{ "Use" };
	ABaseWeapon* weapon{ Cast<ABaseWeapon>(item) };
	if (IsValid(weapon))
	{
		useText = weapon->IsWeaponEquipped() ? "Unequip" : "Equip";
	}
	Use_Btn->SetButtonText(useText);
}

void UInventoryItemWidget::EnableThrow(bool enable)
{
	Throw_Btn->EnableButton(enable);
}
