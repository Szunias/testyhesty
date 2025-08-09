#include "TarotSpawner.h"
#include "../Widgets/TarotScreen.h"
#include "../Widgets/BaseWidget.h"

ATarotSpawner::ATarotSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxAmountOfCards = 5;
}

void ATarotSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	// seperate the allowed cards into positive and negative cards
	for (const ETarotCards& currCard : AllowedCards)
	{
		ECardValue currValue{ *CardsValue.Find(currCard) };
		switch (currValue)
		{
		case ECardValue::Positive:
			PositiveAllowedCards.Add(currCard);
			break;
		case ECardValue::Negative:
			NegativeAllowedCards.Add(currCard);
			break;
		}
	}


	// if either one of the arrays are empty, just add the Fool card to them
	if (PositiveAllowedCards.Num() == 0)
	{
		PositiveAllowedCards.Add(ETarotCards::Fool);
	}
	if (NegativeAllowedCards.Num() == 0)
	{
		NegativeAllowedCards.Add(ETarotCards::Fool);
	}

	// on beginplay, draw the cards
	DrawTheCards();


	TarotScreenWidget = CreateWidget<UTarotScreen>(GetWorld(), TarotScreenClass);
	TarotScreenWidget->InitializeWidget(this);
	TarotScreenWidget->AddWidget();
}

void ATarotSpawner::DestroySpawner()
{
	// remove widget from screen first
	TarotScreenWidget->RemoveWidget();
	Destroy();
}

void ATarotSpawner::SetTarotTextImage(UTexture2D* newImage)
{
	TarotScreenWidget->SetTarotTextImage(newImage);
}

void ATarotSpawner::DrawTheCards()
{
	TArray<ETarotCards> negativeCards{ NegativeAllowedCards };
	TArray<ETarotCards> positiveCards{ PositiveAllowedCards };


	int amountOfPositiveCards{};
	for (int i{ 0 }; i < MaxAmountOfCards; ++i)
	{
		int randNumber{ FMath::RandRange(0, 100) };
		if (randNumber >= Luck) // e.g. 70 luck -> 70% of time should be lucky ( 0 - 70) 
		{
			// player is unlucky
			DrawnCards.Add(DrawRandomCardFromPile(negativeCards, NegativeAllowedCards));
		}
		else
		{
			// player is lucky
			DrawnCards.Add(DrawRandomCardFromPile(positiveCards, PositiveAllowedCards));
			++amountOfPositiveCards;
		}
	}


	// pity system: -> 
	// if the player is too lucky, have a chance to be unlucky
	// if the player is very unlucky, have a chance to be lucky
	// this means that 1 card should get changed
	if (amountOfPositiveCards == MaxAmountOfCards) // all cards are positive
	{
		PitySystem(negativeCards, NegativeAllowedCards);
	}
	else if (amountOfPositiveCards == 0) // all cards are negative
	{
		PitySystem(positiveCards, PositiveAllowedCards);
	}


}

ETarotCards ATarotSpawner::DrawRandomCardFromPile(TArray<ETarotCards>& pileToDrawFrom, const TArray<ETarotCards>& refillPile)
{
	if (pileToDrawFrom.Num() == 0) // check if we can draw a card, if not refill
	{
		pileToDrawFrom = refillPile; // refilling
	}

	int randomIndex{ FMath::RandRange(0, pileToDrawFrom.Num() - 1)};
	ETarotCards drawnCard{ pileToDrawFrom[randomIndex] };
	pileToDrawFrom.RemoveAt(randomIndex);
	return drawnCard;
}

void ATarotSpawner::PitySystem(TArray<ETarotCards>& pileToDrawFrom, const TArray<ETarotCards>& refillPile)
{
	// this should be a 50/50
	if (FMath::RandRange(0, 1))
	{
		DrawnCards.RemoveAt(MaxAmountOfCards - 1);
		DrawnCards.Add(DrawRandomCardFromPile(pileToDrawFrom, refillPile));
		// to avoid having the new card always at the end, shuffle the array
		const int lastIndex{ DrawnCards.Num() - 1 };
		for (int i{ 0 }; i <= lastIndex; ++i)
		{
			const int randIndex{ FMath::RandRange(0, lastIndex) };
			if (i != randIndex)
			{
				DrawnCards.Swap(i, randIndex);
			}
		}

	}	
}

void ATarotSpawner::SetAllowedCards(const TArray<ETarotCards>& cards)
{
	AllowedCards = cards;
}

void ATarotSpawner::PickCard(int index, float effectDelay)
{
	ResultHandlerDelegate.Broadcast(DrawnCards[index], effectDelay);
}

void ATarotSpawner::DebugPrintDrawnCards()
{
	for (ETarotCards currCard : DrawnCards)
	{
		switch (currCard)
		{
		case ETarotCards::Devil:
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "The Devil");
			break;
		case ETarotCards::Hermit:
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "The Hermit");
			break;
		case ETarotCards::Tower:
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, "The Tower");
			break;
		case ETarotCards::Strength:
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Strength");
			break;
		case ETarotCards::Fool:
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, "Fool");
			break;
		case ETarotCards::Temperance:
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Temperance");
			break;
		}
	}
}

const TArray<ETarotCards>& ATarotSpawner::GetDrawnCards()
{
	return DrawnCards;
}

