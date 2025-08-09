
#include "InteractableInterface.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"

void IInteractableInterface::Interact()
{
}

void IInteractableInterface::OnInteractOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (otherActor->IsA<APlayerCharacter>() && otherComp->IsA<UBoxComponent>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Display Icon");
		InteractIcon->SetHiddenInGame(false);
	}
}

void IInteractableInterface::OnInteractOverlapEnd(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (otherActor->IsA<APlayerCharacter>() && otherComp->IsA<UBoxComponent>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Remove Icon");
		InteractIcon->SetHiddenInGame(true);
	}
}
