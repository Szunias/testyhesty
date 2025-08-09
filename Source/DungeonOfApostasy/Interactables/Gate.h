#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Gate.generated.h"

/* https://docs.unrealengine.com/5.3/en-US/opening-doors-in-unreal-engine/ */

class ACameraTrigger;

UCLASS()
class DUNGEONOFAPOSTASY_API AGate : public AActor
{
	GENERATED_BODY()
	
public:	
	AGate();

	UFUNCTION(BlueprintCallable)
	void MoveGate();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* GateOpenTimelineFloatCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* GateCloseTimelineFloatCurve;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGateMovementFinish);
	UPROPERTY(BlueprintAssignable)
	FGateMovementFinish OnGateMoveFinished;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGateMovementBegin, bool, bIsOpening);
	UPROPERTY(BlueprintAssignable)
	FGateMovementBegin OnGateMoveBegin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACameraTrigger* CameraTrigger;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* GateMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTimelineComponent* GateOpenTimelineComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTimelineComponent* GateCloseTimelineComp;

private:
	FOnTimelineFloat UpdateFunctionFloat;

	FOnTimelineEventStatic OnTimelineFinished;


	UFUNCTION()
	void UpdateTimelineComp(float output);

	UFUNCTION()
	void CallMoveFinished();
};