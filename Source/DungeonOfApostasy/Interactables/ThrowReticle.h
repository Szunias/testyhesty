#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowReticle.generated.h"

UCLASS()
class DUNGEONOFAPOSTASY_API AThrowReticle : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AThrowReticle();

	FVector PredictPath(const FVector& startPos, const TArray<AActor*>& ignoreActors);
	void CalculateLaunchVelocity(const FVector& startPos, const FVector& endPos);
	void SetOrientation(const FVector& direction);

	const FVector& GetLaunchVelocity() const;

private:

	FVector LaunchVelocity;
};
