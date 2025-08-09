
#include "ThrowReticle.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AThrowReticle::AThrowReticle()
{
	PrimaryActorTick.bCanEverTick = false;

}

FVector AThrowReticle::PredictPath(const FVector& startPos, const TArray<AActor*>& ignoreActors)
{
	FPredictProjectilePathParams projParams{};
	projParams.StartLocation = startPos;
	projParams.LaunchVelocity = LaunchVelocity;
	projParams.bTraceWithCollision = true;
	projParams.bTraceComplex = false;
	projParams.DrawDebugType = EDrawDebugTrace::None;
	projParams.SimFrequency = 10;
	projParams.MaxSimTime = 20.f;
	projParams.ActorsToIgnore = ignoreActors;
	FPredictProjectilePathResult projResult{};

	UGameplayStatics::PredictProjectilePath(GetWorld(), projParams, projResult);

	FVector impactPoint{ projResult.HitResult.ImpactPoint };
	FVector impactNormal{ projResult.HitResult.ImpactNormal };

	SetActorLocation(impactPoint + impactNormal * 5.f);
	SetActorRotation(impactNormal.Rotation());
	
	return impactPoint;
}

void AThrowReticle::CalculateLaunchVelocity(const FVector& startPos, const FVector& endPos)
{
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVelocity, startPos, endPos, 0.f, 0.75f);
}

void AThrowReticle::SetOrientation(const FVector& direction)
{
	SetActorRotation(direction.Rotation());	
}

const FVector& AThrowReticle::GetLaunchVelocity() const
{
	return LaunchVelocity;
}




