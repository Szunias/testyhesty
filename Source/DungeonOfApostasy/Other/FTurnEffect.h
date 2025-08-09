
#pragma once

#include "CoreMinimal.h"
#include "EEffectType.h"
#include "FTurnEffect.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEffectEnded);

USTRUCT(BlueprintType)
struct FTurnEffect
{
	GENERATED_BODY()

	EEffectType EffectType;
	int EffectValue;		//increase or decrease value of the effect
	int EffectDuration;		//Turns the effect is active for	
	FString EffectDescription;

	FEffectEnded OnEffectEnded;
};
