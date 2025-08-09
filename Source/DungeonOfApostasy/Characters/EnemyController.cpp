#include "EnemyController.h"

void AEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	FTimerDelegate timerDelegate{};
	timerDelegate.BindUObject(this, &AEnemyController::ExecuteDelegate);
	GetWorld()->GetTimerManager().SetTimerForNextTick(timerDelegate);
	// we set a timer to call the Delegate 
	// otherwise it might get called every frame and get stuck in an infinite loop
}

void AEnemyController::ExecuteDelegate()
{
	OnMoveCompletedDelegate.ExecuteIfBound();
}
