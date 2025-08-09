
#include "SpawnedItemScreen.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "../Items/BaseItem.h"
#include "MenuButton.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void USpawnedItemScreen::InitializeWidget(bool HasEnoughSpaceInInventory, ABaseItem* itemToShow)
{
	InitializeWidget();
	PickUp_Btn->GetButton()->SetIsEnabled(HasEnoughSpaceInInventory);

	ItemName_Txt->SetText(FText::FromString(itemToShow->GetItemName()));
	ItemDescription_Txt->SetText(FText::FromString(itemToShow->GetItemDescription()));
	UCanvasPanelSlot* lineImageSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(Item_Img) };
	UTexture2D* itemImage{ itemToShow->GetItemImage() };
	Item_Img->SetBrushFromTexture(itemImage);
	lineImageSlot->SetSize({ static_cast<double>(itemImage->GetSizeX()), static_cast<double>(itemImage->GetSizeY()) });
}

void USpawnedItemScreen::InitializeWidget()
{
	SButton* ButtonWidget = (SButton*)&(PickUp_Btn->GetButton()->TakeWidget().Get());
	ButtonWidget->SetOnClicked(FOnClicked::CreateLambda([this]()
		{
			OnOptionPicked.Broadcast(true);
			return FReply::Handled();
		}));
	
	ButtonWidget = (SButton*)&(Drop_Btn->GetButton()->TakeWidget().Get());
	ButtonWidget->SetOnClicked(FOnClicked::CreateLambda([this]()
		{
			OnOptionPicked.Broadcast(false);
			return FReply::Handled();
		}));

	PickUp_Btn->OnButtonFocusGained.AddDynamic(this, &USpawnedItemScreen::SetActiveButton);
	Drop_Btn->OnButtonFocusGained.AddDynamic(this, &USpawnedItemScreen::SetActiveButton);
}
