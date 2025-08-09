#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"


UCLASS()
class DUNGEONOFAPOSTASY_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	DECLARE_DELEGATE(FOnMoveCompleted);
	FOnMoveCompleted OnMoveCompletedDelegate;


private:
	void ExecuteDelegate();

};
