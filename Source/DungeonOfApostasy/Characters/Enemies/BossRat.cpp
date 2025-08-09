
#include "BossRat.h"
#include "../EnemyController.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../Interactables/TarotSpawnerComponent.h"
#include "../../Interactables/TarotSpawner.h"
#include "../StatsComponent.h"
#include "../../Other/FTurnEffect.h"
#include "../Player/PlayerCharacter.h"
#include "Engine/TargetPoint.h"
#include "Navigation/PathFollowingComponent.h"
#include "../../Other/EMessageType.h"
#include "../../Other/GameModeDoA.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

void ABossRat::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;

	TarotSpawnerComponent = GetComponentByClass<UTarotSpawnerComponent>();
	if (!IsValid(TarotSpawnerComponent))
	{
		UE_LOG(LogTemp, Fatal, TEXT("TarotSpawnerComponent in BossRat not found!!!"));
	}
	OnAttackConnects.Clear();

	Charge01Particles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("Charge01Niagara")));
	if (!IsValid(Charge01Particles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Charge01Niagara in BossRat not found!!!"));
	}

	Charge02Particles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("Charge02Niagara")));
	if (!IsValid(Charge02Particles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Charge02Niagara in BossRat not found!!!"));
	}

	Charge03Particles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("Charge03Niagara")));
	if (!IsValid(Charge03Particles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Charge03Niagara in BossRat not found!!!"));
	}

	Charge04Particles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("Charge04Niagara")));
	if (!IsValid(Charge04Particles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Charge04Niagara in BossRat not found!!!"));
	}

	Charge05Particles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("Charge05Niagara")));
	if (!IsValid(Charge05Particles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Charge05Niagara in BossRat not found!!!"));
	}

	ChargeParticles.Add(Charge01Particles);
	ChargeParticles.Add(Charge02Particles);
	ChargeParticles.Add(Charge03Particles);
	ChargeParticles.Add(Charge04Particles);
	ChargeParticles.Add(Charge05Particles);

	CanRoar = true;
}

void ABossRat::PrepareAttack(ABaseCharacter* target)
{
	TargettedEnemy = target;
	
	FTimerHandle timerHandle{};
	const float attackDelay{ 1.0f };

	if (CurrentCharges > 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "charge attack");
		OnChargeAttack.Broadcast();
		return;
	}


	const int randNumber{ FMath::RandRange(0, (NormalAttackChance + TarotAttackChance + ChargeAttackChance)) };
	if (randNumber <= NormalAttackChance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "normal attack");
		OnAttack.Broadcast();
	}
	else if (randNumber > NormalAttackChance && randNumber <= (NormalAttackChance + TarotAttackChance))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "tarot attack");
		OnTarotAttack.Broadcast();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "charge attack");
		OnChargeAttack.Broadcast();
	}

}

void ABossRat::PerformAttack()
{
	OnAttackFinished.Broadcast();
}

bool ABossRat::IsBossEnemy() const
{
	return true;
}

void ABossRat::OnBattleStart()
{
	Super::OnBattleStart();
	CanRoar = false;
}

void ABossRat::OnBattleEnd()
{
	Super::OnBattleEnd();
	CanRoar = true;
}

void ABossRat::BindIdle()
{
	EnemyController->OnMoveCompletedDelegate.BindUObject(this, &ABossRat::IdleMovement);
}

void ABossRat::BindPlayerTooClose()
{
	EnemyController->OnMoveCompletedDelegate.BindUObject(this, &ABossRat::PlayerTooClose);
	PlayerTooClose();
}

void ABossRat::IdleMovement()
{
	EPathFollowingResult::Type moveResult{ EnemyController->MoveToActor(PatrolPoint) };
	if (moveResult == EPathFollowingResult::Blocked)
	{
		EnemyController->OnMoveCompletedDelegate.BindUObject(this, &ABossRat::RotateCorrectly);
	}
}

void ABossRat::PlayerTooClose()
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

void ABossRat::RotateCorrectly()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "Boss Should Rotate");
	// rotate the boss so it is facing the entrance again
}

void ABossRat::NormalAttack()
{
	UStatsComponent* targetStats{ TargettedEnemy->GetStatsComponent() };
	if (FMath::RandRange(0, 99) >= targetStats->GetCurrentEvasiveness())
	{
		int damageGiven{ StatsComponent->GetDamage() };
		targetStats->UpdateHealth(-damageGiven);
	}
	else
	{
		FString battleMessage{ CharacterName + " missed" };
		OnBattleMessage.Broadcast(battleMessage, EMessageType::Positive);
	}
}

void ABossRat::TarotDrawAttack()
{
	UStatsComponent* targetStats{ TargettedEnemy->GetStatsComponent() };
	if (FMath::RandRange(0, 100) <= targetStats->GetCurrentEvasiveness())
	{
		PerformAttack();
		return;
	}

	int playerLuck{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetStatsComponent()->GetCurrentLuck() };
	ATarotSpawner* spawner{ TarotSpawnerComponent->DrawCards({	ETarotCards::Devil, 
																ETarotCards::Hermit, 
																ETarotCards::Fool, 
																ETarotCards::Strength, 
																ETarotCards::Temperance, 
																ETarotCards::Tower }, 100 - playerLuck) };

	spawner->ResultHandlerDelegate.AddDynamic(this, &ABossRat::HandleCardsDrawn);
}

void ABossRat::HandleCardsDrawn(ETarotCards pickedCard, float effectDelay)
{
	TarotSpawnerComponent->SetTarotTextImage(TarotSpawnerComponent->CardTextImages[pickedCard]);

	FTimerHandle executeEffectHandle{};
	GetWorld()->GetTimerManager().SetTimer(executeEffectHandle, FTimerDelegate::CreateLambda([&, pickedCard]
		{
			switch (pickedCard)
			{
			case ETarotCards::Devil:
			{
				TargettedEnemy->GetStatsComponent()->UpdateStrength(-StrengthBuff);
				break;
			}
			case ETarotCards::Hermit:
			{
				TargettedEnemy->GetStatsComponent()->UpdateSpeed(-SpeedBuff);
				break;
			}
			case ETarotCards::Tower:
			{
				TargettedEnemy->GetStatsComponent()->UpdateLuck(-LuckBuff);
				break;
			}
			case ETarotCards::Strength:
			{
				GetStatsComponent()->UpdateStrength(StrengthBuff);
				break;
			}
			case ETarotCards::Fool:
			{
				//FTurnEffect turnEffect{};
				//turnEffect.EffectDuration = 1;
				//turnEffect.EffectType = EEffectType::Health;
				//turnEffect.EffectValue = -GetStatsComponent()->GetCurrentStrength();
				//turnEffect.EffectDescription = "itself";

				StatsComponent->UpdateHealth(-StatsComponent->GetCurrentStrength());

				//FString battleMessage{ CharacterName + " hurt itself!" };
				//OnBattleMessage.Broadcast(battleMessage, EMessageType::Normal);
				//OnApplyEffect.Broadcast(this, turnEffect);
				break;
			}
			case ETarotCards::Temperance:
			{
				TargettedEnemy->GetStatsComponent()->UpdateHealth(-GetStatsComponent()->GetCurrentStrength());
				break;
			}
			}

			OnAttackConnects.Broadcast();
			// set timer for 2 seconds to handle all and then call the finished
			FTimerHandle handle{};
			GetWorld()->GetTimerManager().SetTimer(handle, this, &ABossRat::PerformAttack, 2.f);

		}
	), effectDelay, false);
	
}

void ABossRat::Charging()
{
	if (IsValid(ChargeParticles[CurrentCharges]))
	{
		ChargeParticles[CurrentCharges]->ActivateSystem();
	}

	++CurrentCharges;

	int randomNumber{ FMath::RandRange(1, MaxAmountOfCharges) };
	// we generate a random number between 1 and the max amount of charges
	// if this number is smaller or equal to the current charges, we release the charge and hit the player
	float waitDelay{ 2.0f };

	if (randomNumber <= CurrentCharges)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "releasing charges");
		FTimerHandle handle{};
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
			{
				OnReleaseCharges.Broadcast();
			}
		), waitDelay, false);
	}
	else
	{
		FString battleMessage{ CharacterName + " prepares the attack" };
		OnBattleMessage.Broadcast(battleMessage, EMessageType::Negative);

		FTimerHandle handle{};
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
			{
				OnAttackFinished.Broadcast();
			}
		), waitDelay, false);
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "current charges: " + FString::FromInt(CurrentCharges));

}

void ABossRat::FinishCharging()
{
	UStatsComponent* targetStats{ TargettedEnemy->GetStatsComponent() };
	if (FMath::RandRange(0, 99) >= targetStats->GetCurrentEvasiveness())
	{
		int damageGiven{ StatsComponent->GetDamage() * CurrentCharges };
		targetStats->UpdateHealth(-damageGiven);
	}
	else
	{
		FString battleMessage{ CharacterName + " missed" };
		OnBattleMessage.Broadcast(battleMessage, EMessageType::Positive);
	}
	CurrentCharges = 0;
}

void ABossRat::WhenEnemyDeathFinished()
{
	Super::WhenEnemyDies();
	AGameModeDoA* gameMode = Cast<AGameModeDoA>(GetWorld()->GetAuthGameMode());
	gameMode->WinGame();
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	pc->ChangeOverlaps(false);
	pc->DisableMovement();
}