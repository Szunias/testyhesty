#include "TarotSpawnerComponent.h"
#include "TarotSpawner.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/StatsComponent.h"
#include "../Widgets/BaseWidget.h"

UTarotSpawnerComponent::UTarotSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTarotSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(TarotSpawnerClass))
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "Need to link the Tarot Spawner in the Component!!!!!");
	}
}

ATarotSpawner* UTarotSpawnerComponent::DrawCards(const TArray<ETarotCards>& AllowedCards, int luckToUse)
{
	ATarotSpawner* tarotSpawner{ GetWorld()->SpawnActorDeferred<ATarotSpawner>(TarotSpawnerClass, FTransform{}) };
	tarotSpawner->SetAllowedCards(AllowedCards);
	if (luckToUse == -1) // if no luck has been specified -> use the player luck
	{
		tarotSpawner->Luck = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetStatsComponent()->GetCurrentLuck();
	}
	tarotSpawner->FinishSpawning(FTransform{});

	CachedSpawner = tarotSpawner;

	return tarotSpawner;
}

void UTarotSpawnerComponent::PickCard(int index)
{
	if (IsValid(CachedSpawner))
	{
		CachedSpawner->PickCard(index, 4.0f);
	}
}

void UTarotSpawnerComponent::SetTarotTextImage(UTexture2D* newImage)
{
	CachedSpawner->SetTarotTextImage(newImage);
}
