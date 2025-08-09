#include "TarotScreen.h"
#include "Components/Button.h"
#include "Engine/Texture2D.h"
#include "Components/CanvasPanelSlot.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "../Interactables/TarotSpawner.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/Image.h"
#include "MenuButton.h"
#include "CardFlip.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"

void UTarotScreen::InitializeWidget()
{
	GrowthStrength = 1.5f;

	Text_Img->SetVisibility(ESlateVisibility::Hidden);

	HasCardBeenPicked = false;
	// add all buttons into an array
	CardButtons.Add(Card01_Btn);
	CardButtons.Add(Card02_Btn);
	CardButtons.Add(Card03_Btn);
	CardButtons.Add(Card04_Btn);
	CardButtons.Add(Card05_Btn);

	Card01_Btn->OnButtonFocusGained.AddDynamic(this, &UTarotScreen::SetActiveButton);
	Card02_Btn->OnButtonFocusGained.AddDynamic(this, &UTarotScreen::SetActiveButton);
	Card03_Btn->OnButtonFocusGained.AddDynamic(this, &UTarotScreen::SetActiveButton);
	Card04_Btn->OnButtonFocusGained.AddDynamic(this, &UTarotScreen::SetActiveButton);
	Card05_Btn->OnButtonFocusGained.AddDynamic(this, &UTarotScreen::SetActiveButton);

	CardFrontImages.Add(CardFront1_Img);
	CardFrontImages.Add(CardFront2_Img);
	CardFrontImages.Add(CardFront3_Img);
	CardFrontImages.Add(CardFront4_Img);
	CardFrontImages.Add(CardFront5_Img);

	for (UMenuButton* currMenuButton : CardButtons)
	{
		UButton* currButton{ currMenuButton->GetButton() };
		FButtonStyle newStyle{};

		FSlateBrush cardImageStyle{};
		cardImageStyle.SetResourceObject(CardBackTexture);
		newStyle.Normal = cardImageStyle;
		newStyle.Hovered = cardImageStyle;
		newStyle.Disabled = cardImageStyle;

		currButton->SetStyle(newStyle);
		UCanvasPanelSlot* canvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(currMenuButton) };
		canvasSlot->SetSize(FVector2D{ static_cast<double>(CardBackTexture->GetSizeX()), static_cast<double>(CardBackTexture->GetSizeY()) });
	}


	// https://benui.ca/unreal/using-same-function-for-many-ubuttons/
	for (int i{0}; i < CardButtons.Num(); ++i)
	{
		UButton* Button{ CardButtons[i]->GetButton()};

		SButton* ButtonWidget = (SButton*)&(Button->TakeWidget().Get());
		ButtonWidget->SetOnClicked(FOnClicked::CreateLambda([this, i]()
			{
				OnButtonClick(i);
				return FReply::Handled();
			}));
		ButtonWidget->SetOnHovered(FSimpleDelegate::CreateLambda([this, i]()
			{
				GrowCard(i);
			}));
		ButtonWidget->SetOnUnhovered(FSimpleDelegate::CreateLambda([this, i]()
			{
				ShrinkCard(i);
			}));
		
	}

	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	pc->GetMovementComponent()->Deactivate();
	SetEnableAllCards(false);
	ChangeAllCardsVisibility(true);
	OnEnterAnimationEnd.AddDynamic(this, &UTarotScreen::EnterAnimationFinished);
}

void UTarotScreen::EnterAnimationFinished()
{
	SetEnableAllCards(true);
	ChangeAllCardsVisibility(false);
	RestoreFocus();
}

void UTarotScreen::PlayExitAnimationWithDelay()
{
	FTimerHandle handle{};
	FTimerDelegate broadcast{ FTimerDelegate::CreateLambda([&] {
			OnExitAnimationBegin.Broadcast();
			for (UCardFlip* card : FlipButtons)
			{
				if (IsValid(card)) // make sure the card is valid
				{
					card->RemoveFromParent();
				}
			}
		}) };
	GetWorld()->GetTimerManager().SetTimer(handle, broadcast, DelayBetweenReveal, false);
}

void UTarotScreen::AllCardsRevealed()
{
	PlayExitAnimationWithDelay();
	SetEnableAllCards(false);
	ChangeAllCardsVisibility(true);

	for (int i{0} ; i < CardFrontImages.Num(); ++i)
	{
		CardFrontImages[i]->SetBrushFromTexture(*CardsImagesMap.Find(DrawnCards[CardButtons.Find(CardButtons[i])]));

		UCanvasPanelSlot* flipSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(FlipButtons[i])};
		UCanvasPanelSlot* imageSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(CardFrontImages[i])};
		imageSlot->SetZOrder(flipSlot->GetZOrder());
	}

	// we now need to spawn a shrinking card in place of the picked one.
	UCardFlip* newCard{WidgetTree->ConstructWidget<UCardFlip>(CardShrinkClass)};
	Canvas->AddChild(newCard);
	newCard->FrontCover->SetBrushFromTexture(*CardsImagesMap.Find(DrawnCards[CardButtons.Find(CardButtons[PickedIndex])]));

	UCardFlip* pickedCard{ FlipButtons[PickedIndex] };
	UCanvasPanelSlot* newCardCanvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(newCard) };
	UCanvasPanelSlot* oldCardCanvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(pickedCard) };
	newCardCanvasSlot->SetAlignment(oldCardCanvasSlot->GetAlignment());
	newCardCanvasSlot->SetPosition(oldCardCanvasSlot->GetPosition());
	newCardCanvasSlot->SetAnchors(oldCardCanvasSlot->GetAnchors());
	newCardCanvasSlot->SetZOrder(oldCardCanvasSlot->GetZOrder());
	pickedCard->RemoveFromParent();
	FlipButtons.Remove(pickedCard);
	FlipButtons.Add(newCard);
	newCard->OnStartCardFlip.Broadcast();
}

void UTarotScreen::InitializeWidget(ATarotSpawner* tarotSpawner)
{
	if (!tarotSpawner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "TarotSpawner is invalid in TarotScreen");
		return;
	}

	TarotSpawner = tarotSpawner;
	DrawnCards = tarotSpawner->GetDrawnCards();
	InitializeWidget();
}

void UTarotScreen::SetTarotTextImage(UTexture2D* newImage)
{
	Text_Img->SetBrushFromTexture(newImage, true);
	Text_Img->SetVisibility(ESlateVisibility::Visible);
	OnTextShowBegin.Broadcast();
}

void UTarotScreen::OnButtonClick(int index)
{
	PickedIndex = index;

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Card: " + FString::FromInt(index) + " was picked");
	// remove all buttons
	SetEnableAllCards(false);
	ChangeAllCardsVisibility(true);
	// spawn the animation widgets in their place
	for (int i{0}; i < CardButtons.Num(); ++i)
	{
		UCardFlip* newCard;
		// make sure to spawn the different one on the index
		if (i == index)
		{
			 newCard = WidgetTree->ConstructWidget<UCardFlip>(PickedFlipClass);
		}
		else
		{
			newCard = WidgetTree->ConstructWidget<UCardFlip>(NormalFlipClass);
		}
		Canvas->AddChild(newCard);
		// change all covers
		newCard->FrontCover->SetBrushFromTexture(*CardsImagesMap.Find(DrawnCards[CardButtons.Find(CardButtons[i])]));
		UCanvasPanelSlot* newCardCanvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(newCard) };
		UCanvasPanelSlot* oldCardCanvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(CardButtons[i])};
		newCardCanvasSlot->SetAlignment(oldCardCanvasSlot->GetAlignment());
		newCardCanvasSlot->SetPosition(oldCardCanvasSlot->GetPosition());
		newCardCanvasSlot->SetAnchors(oldCardCanvasSlot->GetAnchors());
		newCardCanvasSlot->SetSize(oldCardCanvasSlot->GetSize());
		newCardCanvasSlot->SetZOrder(oldCardCanvasSlot->GetZOrder());
		FlipButtons.Add(newCard);
	}
	
	// trigger picked animation first
	FlipButtons[index]->OnStartCardFlip.Broadcast();


	HasCardBeenPicked = true;
	
	PickCard(index);
	
	FTimerHandle timerReveal{};
	GetWorld()->GetTimerManager().SetTimer(timerReveal, this, &UTarotScreen::RevealAllCards, DelayBetweenReveal);
	FTimerHandle timerPick{};
	GetWorld()->GetTimerManager().SetTimer(timerPick, this, &UTarotScreen::DestroySystem, DurationForDestroy);
}

void UTarotScreen::SetEnableAllCards(bool isEnabled)
{
	for (UMenuButton* currButton : CardButtons)
	{
		currButton->GetButton()->SetIsEnabled(isEnabled);
		currButton->SetIsEnabled(isEnabled);
	}
}

void UTarotScreen::ChangeAllCardsVisibility(bool isHidden)
{
	ESlateVisibility newVisibility{};
	
	if (isHidden)
	{
		newVisibility = ESlateVisibility::Hidden;
	}
	else
	{
		newVisibility = ESlateVisibility::Visible;
	}

	for (UMenuButton* currButton : CardButtons)
	{
		currButton->GetButton()->SetVisibility(newVisibility);
	}
}

void UTarotScreen::PickCard(int index)
{
	TarotSpawner->PickCard(index, DurationForDestroy);
}

void UTarotScreen::DestroySystem()
{
	TarotSpawner->DestroySpawner();
}

void UTarotScreen::RevealAllCards()
{
	// make a chain so we can chain each flip to each other
	for (int i{ 0 }; i < FlipButtons.Num(); ++i)
	{
		UCardFlip* card{ FlipButtons[i] };
		if (i == PickedIndex || i + 1 == FlipButtons.Num())
		{
			continue;
		}

		int nextValidIndex{ i + 1 };
		if (nextValidIndex == PickedIndex)
		{
			nextValidIndex += 1;
		}
		if (nextValidIndex >= FlipButtons.Num())
		{
			continue;
		}
		UCardFlip* nextCard{ FlipButtons[nextValidIndex] };
		card->OnEndCardFlip.AddDynamic(nextCard, &UCardFlip::ForceStartFlip);
	}

	int indexEnd{ 4 };
	if (indexEnd == PickedIndex)
	{
		--indexEnd;
	}
	FlipButtons[indexEnd]->OnEndCardFlip.AddDynamic(this, &UTarotScreen::AllCardsRevealed);

	int indexToStart{ 0 };
	if (indexToStart == PickedIndex)
	{
		++indexToStart;
	}
	FlipButtons[indexToStart]->ForceStartFlip();
}

void UTarotScreen::GrowCard(int index)
{
	UCanvasPanelSlot* canvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(CardButtons[index]) };
	canvasSlot->SetSize(FVector2D{ static_cast<double>(CardBackTexture->GetSizeX() * GrowthStrength), static_cast<double>(CardBackTexture->GetSizeY() * GrowthStrength) });
	canvasSlot->SetZOrder(1);
}

void UTarotScreen::ShrinkCard(int index)
{
	if (HasCardBeenPicked)
	{
		return;
	}

	UCanvasPanelSlot* canvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(CardButtons[index]) };
	canvasSlot->SetSize(FVector2D{ static_cast<double>(CardBackTexture->GetSizeX()), static_cast<double>(CardBackTexture->GetSizeY()) });
	canvasSlot->SetZOrder(0);
}
