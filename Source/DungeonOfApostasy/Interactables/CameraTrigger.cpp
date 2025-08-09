#include "CameraTrigger.h"
#include "Components/BoxComponent.h"
#include "CameraTriggerBox.h"
#include "../Characters/Player/PlayerCharacter.h"
ACameraTrigger::ACameraTrigger()
{
}

void ACameraTrigger::SwitchToCamera()
{
	APlayerController* pPlayerController = GetWorld()->GetFirstPlayerController();
	if (pPlayerController)
	{
		pPlayerController->SetViewTargetWithBlend(this, 0.0f, EViewTargetBlendFunction::VTBlend_Linear);
	}
}

void ACameraTrigger::BeginPlay()
{
	Super::BeginPlay();
	for (ACameraTriggerBox* trigger : CameraTriggers) 
	{	
		if (IsValid(trigger))
		{
			trigger->GetTriggerBox()->OnComponentBeginOverlap.AddDynamic(this, &ACameraTrigger::OnOverlapBegin);
		}
	}
}

void ACameraTrigger::OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, "Trigger detected");

	if (otherActor->IsA<APlayerCharacter>())
	{
		SwitchToCamera();
		Cast<APlayerCharacter>(otherActor)->UpdateControlRotation(GetActorRotation());
	}
}
