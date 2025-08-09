#include "StatsComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "../Characters/BaseCharacter.h"
#include "../Other/EMessageType.h"
#include "../Characters/Enemies/BaseEnemy.h"

UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	EffectHealthParticles = Cast<UNiagaraComponent>(GetOwner()->GetDefaultSubobjectByName(TEXT("HealthEffect")));
	if (!IsValid(EffectHealthParticles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("EffectHealthParticles in StatsComponent not found!!!"));
	}

	EffectSpeedParticles = Cast<UNiagaraComponent>(GetOwner()->GetDefaultSubobjectByName(TEXT("SpeedEffect")));
	if (!IsValid(EffectSpeedParticles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("EffectSpeedParticles in StatsComponent not found!!!"));
	}

	EffectStrengthParticles = Cast<UNiagaraComponent>(GetOwner()->GetDefaultSubobjectByName(TEXT("StrengthEffect")));
	if (!IsValid(EffectStrengthParticles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("EffectStrengthParticles in StatsComponent not found!!!"));
	}

	EffectLuckParticles = Cast<UNiagaraComponent>(GetOwner()->GetDefaultSubobjectByName(TEXT("LuckEffect")));
	if (!IsValid(EffectLuckParticles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("EffectLuckParticles in StatsComponent not found!!!"));
	}
}

void UStatsComponent::AddEffect(UNiagaraSystem* system, UNiagaraComponent* component)
{
	if (IsValid(system))
	{
		component->SetAsset(system);
		component->Activate(true);
	}
}

ABaseCharacter* UStatsComponent::GetBaseCharacter()
{
	return Cast<ABaseCharacter>(GetOwner());
}


int UStatsComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

void UStatsComponent::UpdateHealth(int value)
{
	ABaseCharacter* character{ GetBaseCharacter() };
	bool isEnemy{ character->IsA<ABaseEnemy>() };
	FString battleMessage{ character->GetCharacterName() };
	if (value < 0)
	{
		OnTakenDamage.Broadcast();
		AddEffect(HealthDown, EffectHealthParticles);
		
		battleMessage += " lost " + FString::FromInt(FMath::Abs(value)) + " health";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Positive : EMessageType::Negative);
	}
	else if (value > 0)
	{	
		AddEffect(HealthUp, EffectHealthParticles);
		battleMessage += " gained " + FString::FromInt(FMath::Abs(value)) + " health";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Negative : EMessageType::Positive);
	}
	CurrentHealth = FMath::Clamp((CurrentHealth + value), 0, MaxHealth);

	const float healthPercentage{ GetHealthPercentage() };
	OnUpdateHealth.Broadcast(healthPercentage);
	if (healthPercentage < FLT_EPSILON)
	{
		OnDeath.Broadcast();
	}
}

float UStatsComponent::GetHealthPercentage()
{
	return (FMath::Clamp(static_cast<float>(CurrentHealth) / MaxHealth, 0.f, 1.f)); 
}

int UStatsComponent::GetCurrentSpeed()
{
	return CurrentSpeed;
}

void UStatsComponent::UpdateSpeed(int value)
{
	ABaseCharacter* character{ GetBaseCharacter() };
	bool isEnemy{ character->IsA<ABaseEnemy>() };
	FString battleMessage{ character->GetCharacterName() };
	if (value < 0)
	{
		battleMessage += " lost " + FString::FromInt(FMath::Abs(value)) + " speed";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Positive : EMessageType::Negative);
		AddEffect(SpeedDown, EffectSpeedParticles);
	}
	else if (value > 0)
	{
		battleMessage += " gained " + FString::FromInt(value) + " speed";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Negative : EMessageType::Positive);
		AddEffect(SpeedUp, EffectSpeedParticles);
	}
	CurrentSpeed = FMath::Clamp((CurrentSpeed + value), 1, 99);
}

int UStatsComponent::GetCurrentLuck()
{
	return CurrentLuck;
}

void UStatsComponent::UpdateLuck(int value)
{
	ABaseCharacter* character{ GetBaseCharacter() };
	bool isEnemy{ character->IsA<ABaseEnemy>() };
	FString battleMessage{ character->GetCharacterName() };
	if (value < 0)
	{
		battleMessage += " lost " + FString::FromInt(FMath::Abs(value)) + " luck";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Positive : EMessageType::Negative);
		AddEffect(LuckDown, EffectLuckParticles);
	}
	else if (value > 0)
	{
		battleMessage += " gained " + FString::FromInt(value) + " luck";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Negative : EMessageType::Positive);
		AddEffect(LuckUp, EffectLuckParticles);
	}
	CurrentLuck = FMath::Clamp((CurrentLuck + value), 0, 100);
}

int UStatsComponent::GetCurrentStrength()
{
	return CurrentStrength;
}

int UStatsComponent::GetCurrentStrengthRange()
{
	return StrengthRange;
}

int UStatsComponent::GetDamage()
{
	return CurrentStrength + FMath::RandRange(0, StrengthRange);
}

void UStatsComponent::UpdateStrength(int value)
{
	ABaseCharacter* character{ GetBaseCharacter() };
	bool isEnemy{ character->IsA<ABaseEnemy>() };
	FString battleMessage{ character->GetCharacterName() };
	if (value < 0)
	{
		battleMessage += " lost " + FString::FromInt(FMath::Abs(value)) + " strength";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Positive : EMessageType::Negative);
		AddEffect(StrengthDown, EffectStrengthParticles);
	}
	else if (value > 0)
	{
		battleMessage += " gained " + FString::FromInt(value) + " strength";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Negative : EMessageType::Positive);
		AddEffect(StrengthUp, EffectStrengthParticles);
	}
	CurrentStrength = FMath::Clamp((CurrentStrength + value), 1, 1000);
}

int UStatsComponent::GetCurrentEvasiveness()
{
	return CurrentEvasiveness;
}

void UStatsComponent::UpdateEvasiveness(int value)
{
	ABaseCharacter* character{ GetBaseCharacter() };
	bool isEnemy{ character->IsA<ABaseEnemy>() };
	FString battleMessage{ character->GetCharacterName() };
	if (value < 0)
	{
		battleMessage += " lost evasion";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Positive : EMessageType::Negative);
	}
	else if (value > 0)
	{
		battleMessage += " gained evasion";
		GetBaseCharacter()->OnBattleMessage.Broadcast(battleMessage, isEnemy ? EMessageType::Negative : EMessageType::Positive);
	}
	CurrentEvasiveness = FMath::Clamp((CurrentEvasiveness + value), 0, 100);
}

