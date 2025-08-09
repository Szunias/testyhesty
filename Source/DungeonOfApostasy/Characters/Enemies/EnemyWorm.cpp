
#include "EnemyWorm.h"
#include "../EnemyController.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void AEnemyWorm::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AEnemyWorm::BindIdle()
{
	EnemyController->OnMoveCompletedDelegate.BindUObject(this, &AEnemyWorm::IdleMovement);
}

void AEnemyWorm::BindPlayerTooClose()
{
	EnemyController->OnMoveCompletedDelegate.BindUObject(this, &AEnemyWorm::PlayerTooClose);
}

void AEnemyWorm::IdleMovement()
{
	// get a random point in radius
	// move to that point
	// change movement speed with random range in mind

	FNavLocation randomPoint{};
	NavSystem->GetRandomReachablePointInRadius(GetActorLocation(), RandomPointRadius, randomPoint);
	EnemyController->MoveToLocation(randomPoint.Location);
}

void AEnemyWorm::PlayerTooClose()
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

