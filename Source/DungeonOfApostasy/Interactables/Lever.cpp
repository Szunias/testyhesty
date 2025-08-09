
#include "Lever.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BillboardComponent.h"

ALever::ALever()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALever::BeginPlay()
{
	Super::BeginPlay();

	Mesh = GetComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(Mesh))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Mesh in Lever not found!!!"));
	}

	InteractIcon = Cast<UBillboardComponent>(GetDefaultSubobjectByName(TEXT("Interact")));
	if (!IsValid(InteractIcon))
	{
		UE_LOG(LogTemp, Fatal, TEXT("InteractIcon (billboard) in Lever not found!!!"));
	}

	OnCutsceneDone.AddDynamic(this, &ALever::FinishInteracting);

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ALever::OnInteractOverlapBegin);
	Mesh->OnComponentEndOverlap.AddDynamic(this, &ALever::OnInteractOverlapEnd);

}

void ALever::Interact()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Lever has been interacted with");

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	PlayerCharacter->StorePreviousCameraAndSwitch(CameraTrigger);
	OnLeverPull.Broadcast();

	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly{});
	PlayerCharacter->ChangeOverlaps(false);
	PlayerCharacter->GetCharacterMovement()->Deactivate();
	PlayerCharacter->FreezeAllEnemies();
	PlayerCharacter->SetIsInteracting(true);
}

void ALever::FinishInteracting()
{
	++AmountOfFinishedGates;

	if (AmountOfFinishedGates < GatesToMove.Num())
	{
		return;
	}

	FTimerHandle handle{};
	GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Lever has been pulled");
			PlayerCharacter->SwitchToPreviousCamera();
			PlayerCharacter->GetCharacterMovement()->Activate();
			PlayerCharacter->ChangeOverlaps(true);
			GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly{});
			AmountOfFinishedGates = 0;
			PlayerCharacter->UnFreezeAllEnemies();
			PlayerCharacter->SetIsInteracting(false);
		}
	), 1.5f, false);
}
