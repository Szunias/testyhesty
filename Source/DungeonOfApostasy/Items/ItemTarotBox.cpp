
#include "ItemTarotBox.h"
#include "../Interactables/TarotSpawnerComponent.h"
#include "../Interactables/TarotSpawner.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "../Characters/StatsComponent.h"
#include "Kismet/GameplayStatics.h"

AItemTarotBox::AItemTarotBox()
{
	ItemName = "Box of Tarot Cards";
	HasDelayedEffect = true;
	CanItemBeThrown = false;
}


void AItemTarotBox::BeginPlay()
{
	Super::BeginPlay();
	TarotSpawnerComponent = GetComponentByClass<UTarotSpawnerComponent>();
	if (!IsValid(TarotSpawnerComponent))
	{
		UE_LOG(LogTemp, Fatal, TEXT("TarotSpawnerComponent in TarotBox not found!!!"));
	}
}

void AItemTarotBox::Use()
{
	// draw cards using all the cards!
	ATarotSpawner* spawner{ TarotSpawnerComponent->DrawCards({	ETarotCards::Devil,
																ETarotCards::Fool,
																ETarotCards::Hermit,
																ETarotCards::Strength,
																ETarotCards::Temperance,
																ETarotCards::Tower			}) };

	spawner->ResultHandlerDelegate.AddDynamic(this, &AItemTarotBox::HandleCardsDrawn);
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	PlayerCharacter->ChangeOverlaps(false);
	PlayerCharacter->FreezeAllEnemies();
	OnItemUsed.Broadcast();
}

void AItemTarotBox::Throw()
{
	// Item cannot be thrown!!!
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Tarot Box cannot be thrown!!!");
}

void AItemTarotBox::HandleCardsDrawn(ETarotCards pickedCard, float effectDelay)
{
	TarotSpawnerComponent->SetTarotTextImage(TarotSpawnerComponent->CardTextImages[pickedCard]);

	FTimerHandle executeEffectHandle{};
	GetWorld()->GetTimerManager().SetTimer(executeEffectHandle, FTimerDelegate::CreateLambda([&, pickedCard]
		{
			if (!IsValid(PlayerCharacter))
			{
				return;
			}

			switch (pickedCard)
			{
			case ETarotCards::Devil:
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Devil got drawn");
				PlayerCharacter->GetStatsComponent()->UpdateStrength(-StrengthBuff);
				break;
			case ETarotCards::Hermit:
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Hermit got drawn");
				PlayerCharacter->GetStatsComponent()->UpdateSpeed(SpeedBuff);
				break;
			case ETarotCards::Tower:
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Tower got drawn");
				PlayerCharacter->GetStatsComponent()->UpdateLuck(-LuckBuff);
				break;
			case ETarotCards::Strength:
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Strength got drawn");
				PlayerCharacter->GetStatsComponent()->UpdateStrength(StrengthBuff);
				break;
			case ETarotCards::Fool:
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Fool got drawn");
				PlayerCharacter->GetStatsComponent()->UpdateSpeed(-SpeedBuff);
				break;
			case ETarotCards::Temperance:
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Temperance got drawn");
				PlayerCharacter->GetStatsComponent()->UpdateLuck(LuckBuff);
				break;
			}

			OnItemUsed.Broadcast();

			OnItemUsed.Clear();
			PlayerCharacter->ChangeOverlaps(true);
			PlayerCharacter->UnFreezeAllEnemies();
		}
	), effectDelay, false);
	

}
