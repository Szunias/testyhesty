#include "BaseCharacter.h"
#include "StatsComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	StatsComponent = GetComponentByClass<UStatsComponent>();
	if (!IsValid(StatsComponent))
	{
		UE_LOG(LogTemp, Fatal, TEXT("StatsComponent in BaseCharacter not found!!!"));
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UStatsComponent* ABaseCharacter::GetStatsComponent()
{
	return StatsComponent;
}

ABaseCharacter* ABaseCharacter::GetTargettedEnemy()
{
	return TargettedEnemy;
}

const FString& ABaseCharacter::GetCharacterName() const
{
	return CharacterName;
}

void ABaseCharacter::OnBattleStart()
{
}

void ABaseCharacter::OnBattleEnd()
{
}

void ABaseCharacter::PrepareAttack(ABaseCharacter* character)
{
	
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::PerformAttack()
{
}

UTexture2D* ABaseCharacter::GetCharacterImage()
{
	return CharacterImage;
}