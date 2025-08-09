#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Other/InteractableInterface.h"
#include "Lever.generated.h"

class APlayerCharacter;
class ACameraTrigger;
class AGate;

UCLASS()
class DUNGEONOFAPOSTASY_API ALever : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ALever();

	virtual void Interact() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACameraTrigger* CameraTrigger;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPullingLever);
	UPROPERTY(BlueprintAssignable)
	FPullingLever OnLeverPull;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCutsceneDone);
	UPROPERTY(BlueprintCallable)
	FCutsceneDone OnCutsceneDone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AGate*> GatesToMove;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	USkeletalMeshComponent* Mesh;


private:
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UFUNCTION()
	void FinishInteracting();
	
	int AmountOfFinishedGates;

};
