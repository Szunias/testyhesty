
#include "EnemyRat.h"
#include "../EnemyController.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void AEnemyRat::BindIdle()
{
	EnemyController->OnMoveCompletedDelegate.BindUObject(this, &AEnemyRat::IdleMovement);
}

void AEnemyRat::BindPlayerTooClose()
{
	EnemyController->OnMoveCompletedDelegate.BindUObject(this, &AEnemyRat::PlayerTooClose);
	GetCharacterMovement()->MaxWalkSpeed = RunningMovementSpeed;
}

void AEnemyRat::IdleMovement()
{
	// get a random point in radius
	// move to that point
	// change movement speed with random range in mind

	FNavLocation randomPoint{};
	NavSystem->GetRandomReachablePointInRadius(GetActorLocation(), RandomPointRadius, randomPoint);
	EnemyController->MoveToLocation(randomPoint.Location);
	GetCharacterMovement()->MaxWalkSpeed = IdleMovementSpeed + FMath::FRandRange(0, SpeedRange);
}


void AEnemyRat::PlayerTooClose()
{
	if (IsValid(PointOfInterest))
	{
		EnemyController->MoveToActor(PointOfInterest);
	}
	else
	{
		EnemyController->MoveToActor(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
}
