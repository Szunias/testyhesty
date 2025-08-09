
#include "BattleArea.h"
#include "Components/BillboardComponent.h"
#include "../Interactables/CameraTrigger.h"
#include "../Widgets/BattleScreenWidget.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "../Characters/Enemies/BaseEnemy.h"
#include "../Characters/StatsComponent.h"
#include "../Interactables/CameraTrigger.h"
#include "../Characters/BaseCharacter.h"
#include "Components/Button.h"
#include "../Interactables/TarotSpawnerComponent.h"
#include "../Interactables/TarotSpawner.h"
#include "Components/ProgressBar.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Widgets/InventoryScreenWidget.h"
#include "../Items/BaseItem.h"
#include "../Items/BaseWeapon.h"
#include "Curves/CurveFloat.h"

ABattleArea::ABattleArea()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxTurnSimulation = 6;
	PreviousTurnAmount = 4;
}

void ABattleArea::BeginPlay()
{
	Super::BeginPlay();

	TarotSpawnerComponent = GetComponentByClass<UTarotSpawnerComponent>();

	PlayerSide = Cast<UBillboardComponent>(GetDefaultSubobjectByName(TEXT("PlayerSideComponent")));
	if (!IsValid(PlayerSide))
	{
		UE_LOG(LogTemp, Fatal, TEXT("No Player Side found on BattleArea"));
	}

	EnemySide = Cast<UBillboardComponent>(GetDefaultSubobjectByName(TEXT("EnemySideComponent")));
	if (!IsValid(EnemySide))
	{
		UE_LOG(LogTemp, Fatal, TEXT("No Enemy Side found on BattleArea"));
	}

	SelectEnemyArrow = Cast<UBillboardComponent>(GetDefaultSubobjectByName(TEXT("SelectionArrow")));
	if (!IsValid(SelectEnemyArrow))
	{
		UE_LOG(LogTemp, Fatal, TEXT("No selection arrow found on BattleArea"));
	}

	CanPlayerWin = false;
	IsPlayerDead = false;
}

void ABattleArea::StartBattle(APlayerCharacter* player, const TArray<ABaseEnemy*>& enemies)
{

	BattleScreenWidget = CreateWidget<UBattleScreenWidget>(GetWorld(), BattleScreenWidgetClass);
	BattleScreenWidget->InitializeWidget();
	BattleScreenWidget->AddWidget();
	BattleScreenWidget->GetFleeButton()->OnClicked.AddDynamic(this, &ABattleArea::AttemptFlee);
	BattleScreenWidget->GetAttackButton()->OnClicked.AddDynamic(player, &APlayerCharacter::Attack);
	BattleScreenWidget->GetInventoryButton()->OnClicked.AddDynamic(this, &ABattleArea::OpenInventory);
	BattleScreenWidget->OnEnemySelection.AddDynamic(this, &ABattleArea::SelectAdjacentEnemy);
	
	//Adding player to battle
	FBattleParticipant playerParticipant{};
	UStatsComponent* playerStats{ player->GetStatsComponent() };
	playerParticipant.CurrentSpeed = playerStats->GetCurrentSpeed();
	playerParticipant.OriginalTransform = player->GetActorTransform();
	playerParticipant.HasPriorityTurn = true;
	playerParticipant.ParticipantType = EParticipantType::Player;
	playerParticipant.TurnEffect.EffectType = EEffectType::None;
	BattleParticipants.Add(player, playerParticipant);
	OrderedBattleParticipants.Add(player);
	Player = player;
	
	FTransform transformOffset{ PlayerSide->GetComponentTransform() };
	float capsuleHalfHeight{ player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	transformOffset.SetLocation(transformOffset.GetLocation() + FVector{ 0.f, 0.f, capsuleHalfHeight });

	player->OnBattleStart();
	player->SetActorTransform(transformOffset);
	player->OnAttackConnects.AddDynamic(this, &ABattleArea::UpdateAllParticipants);
	player->OnAttackFinished.AddDynamic(this, &ABattleArea::EndTurn);
	player->OnApplyEffect.AddDynamic(this, &ABattleArea::ApplyEffect);
	player->OnBattleMessage.AddDynamic(this, &ABattleArea::AddBattleMessage);

	BattleScreenWidget->UpdatePlayerHealthbar(playerStats->GetHealthPercentage(), playerStats->GetCurrentHealth());
	playerStats->OnTakenDamage.AddDynamic(BattleScreenWidget, &UBattleScreenWidget::OnPlayerDamageTaken);
	playerStats->OnDeathFinished.AddDynamic(this, &ABattleArea::CheckToEndBattle);

	for (int i{ 0 }; i < PreviousTurnAmount; ++i)
	{
		PreviousCharacterImages.Add(Player->GetCharacterImage());
	}

	//Adding enemies to battle
	EnemyOrder = enemies;
	float totalOffset{ 0.f };
	float smallestRadius{ FLT_MAX };

	for (ABaseEnemy* enemy : EnemyOrder)
	{
		FBattleParticipant enemyParticipant{};
		UStatsComponent* enemyStats{ enemy->GetStatsComponent() };
		enemyParticipant.CurrentSpeed = enemyStats->GetCurrentSpeed();
		enemyParticipant.OriginalTransform = enemy->GetActorTransform();
		enemyParticipant.HasPriorityTurn = false;
		enemyParticipant.ParticipantType = EParticipantType::Enemy;
		BattleParticipants.Add(enemy, enemyParticipant);
		OrderedBattleParticipants.Add(enemy);
		enemyStats->OnTakenDamage.AddDynamic(BattleScreenWidget, &UBattleScreenWidget::OnEnemyDamageTaken);
		enemyStats->OnDeathFinished.AddDynamic(this, &ABattleArea::CheckToEndBattle);

		float currentBattleRadius{ enemy->GetBattleRadius() };
		totalOffset += currentBattleRadius;

		if (currentBattleRadius < smallestRadius)
		{
			smallestRadius = currentBattleRadius;
		}

		CanPlayerWin |= enemy->IsBossEnemy();
		enemy->OnBattleStart();

		
		enemy->OnAttackConnects.AddDynamic(this, &ABattleArea::UpdateAllParticipants);
		enemy->OnAttackFinished.AddDynamic(this, &ABattleArea::EndTurn);
		
		
		enemy->OnApplyEffect.AddDynamic(this, &ABattleArea::ApplyEffect);
		enemy->OnBattleMessage.AddDynamic(this, &ABattleArea::AddBattleMessage);
	}

	if (CanPlayerWin)
	{
		BattleScreenWidget->DisableFlee();
	}

	float currentYOffset{ EnemyOrder.Num() > 1 ? - (totalOffset / 2.f + smallestRadius) : 0.f};
	float currentCapsuleRadius{ 0.f };

	Algo::Sort(EnemyOrder, [](const ABaseEnemy* enemyA, const ABaseEnemy* enemyB)
		{
			return enemyA->GetBattleRadius() < enemyB->GetBattleRadius();
		});

	for (ABaseEnemy* sortedEnemy : EnemyOrder)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Radius: " + FString::FromInt(static_cast<int>(sortedEnemy->GetBattleRadius())) 
			+ " Name: " + sortedEnemy->GetName());

		currentCapsuleRadius = sortedEnemy->GetBattleRadius();
		currentYOffset += currentCapsuleRadius;

		transformOffset = EnemySide->GetComponentTransform();
		capsuleHalfHeight = sortedEnemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		transformOffset.SetLocation(transformOffset.GetLocation() + FVector{ EnemyDepth, currentYOffset, capsuleHalfHeight });

		currentYOffset += currentCapsuleRadius;

		sortedEnemy->SetActorTransform(transformOffset);
	}

	UpdateSpeedStats();
	UpdateTurnOrder();

	BattleScreenWidget->OnStartAnimationFinished.AddDynamic(this, &ABattleArea::NextTurn);

	if (EnemyOrder.Num() > 0)
	{
		SelectEnemy(EnemyOrder[0], false);
		SelectAdjacentEnemy(-1);
		UpdateSelectionArrow();
	}
	BattleScreenWidget->LockButtons();

	if (!IsValid(BattleCamera))
	{
		BattleCamera->SwitchToCamera();
	}

	OnBattleStart.Broadcast();

	FTimerDelegate cameraSwitchDelegate{ FTimerDelegate::CreateUObject(Player, &APlayerCharacter::StorePreviousCameraAndSwitch, BattleCamera) };
	GetWorld()->GetTimerManager().SetTimerForNextTick(cameraSwitchDelegate);
}

void ABattleArea::SelectEnemy(ABaseEnemy* enemy, bool lerpPosition)
{
	float enemySideX{ static_cast<float>(EnemySide->GetComponentLocation().X) };
	if (IsValid(enemy))
	{		
		if (IsValid(SelectedEnemy))
		{		
			FVector enemyLocation{ SelectedEnemy->GetActorLocation() };
			enemyLocation.X = enemySideX + EnemyDepth;
			if (lerpPosition)
			{
				UpdateEnemyXPosition(SelectedEnemy, SelectedEnemy->GetActorLocation(), enemyLocation, 0.f, false);
			}
			else
			{
				SelectedEnemy->SetActorLocation(enemyLocation);
			}
		}
		SelectedEnemy = enemy;
	}

	if (IsValid(SelectedEnemy))
	{
		FVector enemyLocation{ SelectedEnemy->GetActorLocation() };
		enemyLocation.X = enemySideX;
		if (lerpPosition)
		{
			UpdateEnemyXPosition(SelectedEnemy, SelectedEnemy->GetActorLocation(), enemyLocation, 0.f, false);
		}
		else
		{
			SelectedEnemy->SetActorLocation(enemyLocation);
		}
	}
	else
	{
		return;
	}

	SelectionArrowLocation = SelectedEnemy->GetActorLocation() + FVector{ 0.f, 0.f, 2 * SelectedEnemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	Player->PrepareAttack(SelectedEnemy);

	UStatsComponent* enemyStats{ SelectedEnemy->GetStatsComponent() };
	BattleScreenWidget->UpdateEnemyHealthbar(enemyStats->GetHealthPercentage(), enemyStats->GetCurrentHealth());
	BattleScreenWidget->SetEnemyImage(SelectedEnemy);

	SelectedEnemyIdx = EnemyOrder.Find(SelectedEnemy);
}

void ABattleArea::ApplyEffect(ABaseCharacter* target, FTurnEffect turnEffect)
{
	if (IsValid(target))
	{
		//add turn effect
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, "Apply effect");
		FBattleParticipant* battleParticipant{ BattleParticipants.Find(target) };
		if (battleParticipant && battleParticipant->TurnEffect.EffectType == EEffectType::None)
		{
			battleParticipant->TurnEffect = turnEffect;
		}
	}
}

void ABattleArea::AddBattleMessage(FString message, EMessageType messageType)
{
	BattleScreenWidget->AnnounceMessage(message, messageType);
}

bool ABattleArea::UpdateParticipant(ABaseCharacter* participant)
{
	if (!IsValid(participant))
	{
		return false;
	}

	bool isPlayer{ participant->IsA<APlayerCharacter>() };
	const float healthPercentage{ participant->GetStatsComponent()->GetHealthPercentage() };
	if (isPlayer)
	{
		UpdateHealthbar(EParticipantType::Player);
		if (healthPercentage < FLT_EPSILON)
		{
			IsPlayerDead = true;
			return false;
		}		
	}
	else
	{
		if (BattleScreenWidget->GetActiveEnemy() == SelectedEnemy)
		{
			UpdateHealthbar(EParticipantType::Enemy);
		}
		if (healthPercentage < FLT_EPSILON)
		{
			FBattleParticipant battler{};
			BattleParticipants.RemoveAndCopyValue(participant, battler);
			OrderedBattleParticipants.Remove(participant);

			EnemyOrder.Remove(Cast<ABaseEnemy>(participant));

			if (participant == SelectedEnemy)
			{
				for (TPair<ABaseCharacter*, FBattleParticipant>& battleParticipant : BattleParticipants)
				{
					ABaseEnemy* newEnemy{ Cast<ABaseEnemy>(battleParticipant.Key) };
					if (IsValid(newEnemy))
					{
						SelectEnemy(newEnemy);
						SelectAdjacentEnemy(0);
					}
				}
			}

			battler.TurnEffect.OnEffectEnded.Broadcast();
			return false;
		}
	}
	return true;
}

bool ABattleArea::IsBattleActive()
{
	return BattleParticipants.Num() > 0;
}

ABaseCharacter* ABattleArea::FindNextCharacterTurn(TMap<ABaseCharacter*, FBattleParticipant>& participants)
{
	ABaseCharacter* nextCharacter = nullptr;
	int lowestSpeed = INT32_MAX;

	for (TPair<ABaseCharacter*, FBattleParticipant>& participant : participants)
	{
		if (participant.Value.HasPriorityTurn)
		{
			participant.Value.HasPriorityTurn = false;
			nextCharacter = participant.Key;
			break;
		}
		if (participant.Value.CurrentSpeed < lowestSpeed)
		{
			nextCharacter = participant.Key;
			lowestSpeed = participant.Value.CurrentSpeed;
		}
	}

	FBattleParticipant* participantData{ participants.Find(nextCharacter) };
	if (nextCharacter->IsA<ABaseEnemy>())
	{
		participantData->CurrentSpeed += PlayerSpeed;
	}
	else
	{
		participantData->CurrentSpeed += EnemySpeed;
	}
	return nextCharacter;
}

void ABattleArea::SimulateTurns(TMap<ABaseCharacter*, FBattleParticipant>& participants, TArray<ABaseCharacter*>& characters, int order)
{
	if (order >= MaxTurnSimulation)
	{
		return;
	}
	ABaseCharacter* participant{ FindNextCharacterTurn(participants) };
	characters.Add(participant);
	SimulateTurns(participants, characters, order + 1);
}

void ABattleArea::NextTurn()
{
	UpdateSpeedStats();
	UpdateTurnOrder();

	ABaseCharacter* nextCharacter{ FindNextCharacterTurn(BattleParticipants) };

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, nextCharacter->GetName());
	if (nextCharacter->IsA<APlayerCharacter>())
	{
		SelectEnemyArrow->SetHiddenInGame(false);
		BattleScreenWidget->UnlockButtons();
		BattleScreenWidget->RestoreFocus();
		ResetToSelectedEnemy(SelectedEnemy, false);
		nextCharacter->PrepareAttack(SelectedEnemy);
	}
	else
	{
		SelectEnemyArrow->SetHiddenInGame(true);
		ResetToSelectedEnemy(Cast<ABaseEnemy>(nextCharacter), true);
	}

	PreviousCharacterImages.RemoveAt(0);
	PreviousCharacterImages.Add(nextCharacter->GetCharacterImage());
}

void ABattleArea::UpdateAllParticipants()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, "Updating all participants");
	for (auto target : BattleParticipants)
	{
		if (!UpdateParticipant(target.Key) && !IsPlayerDead) // if it returns false, something has died
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABattleArea::UpdateAllParticipants);
			break;
		}
	}
}

void ABattleArea::EndTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, "Ending Turn");
	FTimerHandle unusedHandle{};
	if (BattleParticipants.Num() >= 2 && !IsPlayerDead) // check for if the battle should end
	{
		FTimerDelegate turnEffectDelegate{ FTimerDelegate::CreateUObject(this, &ABattleArea::HandleTurnEffects, 0) };
		GetWorld()->GetTimerManager().SetTimer(unusedHandle, turnEffectDelegate, 0.5f, false);
	}

	// intentially freeze the battle when we need to end it
	// this will result in when the death animations are done playing, it will automatically end the battle 
	
	//else
	//{
	//	GetWorld()->GetTimerManager().SetTimer(unusedHandle, this, &ABattleArea::EndBattle, 1.f, false);
	//}
}

void ABattleArea::HandleItemUse()
{
	CloseInventory();
	HandleTurnEffects(0);
	GetWorld()->GetTimerManager().SetTimerForNextTick(BattleScreenWidget, &UBattleScreenWidget::LockButtons);
	UpdateHealthbar(EParticipantType::Player);
}

void ABattleArea::HandleTurnEffects(int idx)
{	
	if (idx >= OrderedBattleParticipants.Num())
	{
		FTimerHandle unusedHandle;
		if (BattleParticipants.Num() >= 2)
		{
			GetWorld()->GetTimerManager().SetTimer(unusedHandle, this, &ABattleArea::NextTurn, 1.f, false);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(unusedHandle, this, &ABattleArea::EndBattle, 1.f, false);
		}

		return;
	}

	ABaseCharacter* effectCharacter{ OrderedBattleParticipants[idx] };

	FBattleParticipant* battleParticipant{ BattleParticipants.Find(effectCharacter) };
	if (battleParticipant && battleParticipant->IsValidEffect())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, "execute turn effect " + effectCharacter->GetName());
		battleParticipant->ApplyEffect(effectCharacter);
		bool isAlive{ UpdateParticipant(effectCharacter) };

		FTimerHandle unusedHandle;
		if (!IsPlayerDead)
		{
			int participantIdx{ (isAlive ? ++idx : idx) };
			FTimerDelegate turnEffectDelegate{ FTimerDelegate::CreateUObject(this, &ABattleArea::HandleTurnEffects, participantIdx) };
			GetWorld()->GetTimerManager().SetTimer(unusedHandle, turnEffectDelegate, 0.5f, false);
		}
	}
	else
	{
		HandleTurnEffects(idx + 1);
	}
}

void ABattleArea::EndBattle()
{
	BattleScreenWidget->RemoveWidget();

	for (TPair<ABaseCharacter*, FBattleParticipant>& participant : BattleParticipants)
	{
		participant.Value.TurnEffect.OnEffectEnded.Broadcast();

		participant.Key->SetActorTransform(participant.Value.OriginalTransform);
		participant.Key->OnBattleEnd();
		participant.Key->OnAttackConnects.Remove(this, FName{"UpdateAllParticipants"});
		UStatsComponent* stats{ participant.Key->GetStatsComponent() };
		stats->OnTakenDamage.Remove(BattleScreenWidget, FName{ "OnPlayerDamageTaken" });
		stats->OnTakenDamage.Remove(BattleScreenWidget, FName{ "OnEnemyDamageTaken" });
		stats->OnDeathFinished.Remove(this, FName{"CheckToEndBattle"});
		participant.Key->OnAttackFinished.Clear();
		participant.Key->OnApplyEffect.Clear();
		participant.Key->OnBattleMessage.Clear();
	}

	Player->SwitchToPreviousCamera();


	//Reset variables
	BattleParticipants.Empty();
	OrderedBattleParticipants.Empty();
	EnemyOrder.Empty();
	PreviousCharacterImages.Empty();
	Player = nullptr;
	SelectedEnemy = nullptr;
	CanPlayerWin = false;

	OnBattleEnd.Broadcast();
}

void ABattleArea::UpdateTurnOrder()
{
	TArray<ABaseCharacter*> characters{ TArray<ABaseCharacter*>() };
	TMap<ABaseCharacter*, FBattleParticipant> currentTurnOrder = BattleParticipants;
	SimulateTurns(currentTurnOrder, characters);

	TArray<UTexture2D*> characterImages;
	for (ABaseCharacter* character : characters)
	{
		characterImages.Add(character->GetCharacterImage());
	}
	BattleScreenWidget->SetTurnOrder(PreviousCharacterImages, characterImages);
}

void ABattleArea::AttemptFlee()
{
	BattleScreenWidget->DisableAutoRegainFocus();
	ATarotSpawner* spawner{ TarotSpawnerComponent->DrawCards({	ETarotCards::Devil, ETarotCards::Hermit, ETarotCards::Fool})};
	spawner->ResultHandlerDelegate.AddDynamic(this, &ABattleArea::HandleCardsDrawn);
}

void ABattleArea::OpenInventory()
{
	if (!IsValid(InventoryScreen))
	{
		BattleScreenWidget->DisableAutoRegainFocus();
		BattleScreenWidget->LockButtons();
		InventoryScreen = CreateWidget<UInventoryScreenWidget>(GetWorld(), InventoryScreenClass);
		InventoryScreen->OnItemUsed.AddDynamic(this, &ABattleArea::OnItemUsed);
		InventoryScreen->OnItemThrown.AddDynamic(this, &ABattleArea::OnItemThrown);
		InventoryScreen->OnBack.AddDynamic(this, &ABattleArea::CloseInventory);
		InventoryScreen->IncludeWeaponItems(true);
		InventoryScreen->InitializeWidget();
		InventoryScreen->SetParentWidget(BattleScreenWidget);
		InventoryScreen->DisableThrow();
	}
}

void ABattleArea::CloseInventory()
{
	if (IsValid(InventoryScreen))
	{
		InventoryScreen->RemoveWidget();
		InventoryScreen = nullptr;
		BattleScreenWidget->UnlockButtons();
		BattleScreenWidget->RestoreFocus();
	}
}

void ABattleArea::OnItemUsed(ABaseItem* usedItem)
{
	if (usedItem->IsA<ABaseWeapon>())
	{
		CloseInventory();
		return;
	}
	if (!usedItem->HasDelayedItemEffect())
	{
		FTimerDelegate closeInventoryDelegate{ FTimerDelegate::CreateLambda([this] {
			CloseInventory();
			BattleScreenWidget->LockButtons();
			UpdateHealthbar(EParticipantType::Player);
		}) };
		GetWorld()->GetTimerManager().SetTimerForNextTick(closeInventoryDelegate);

		FTimerHandle unusedHandle{};
		FTimerDelegate turnEffectDelegate{ FTimerDelegate::CreateUObject(this, &ABattleArea::HandleTurnEffects, 0) };
		GetWorld()->GetTimerManager().SetTimer(unusedHandle, turnEffectDelegate, 0.5f, false);
	}
	else
	{
		BattleScreenWidget->DisableAutoRegainFocus();
		usedItem->OnItemUsed.AddDynamic(this, &ABattleArea::HandleItemUse);
	}
}

void ABattleArea::OnItemThrown(ABaseItem* thrownItem)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABattleArea::CloseInventory);
	BattleScreenWidget->LockButtons();

	FTimerHandle unusedHandle{};
	FTimerDelegate turnEffectDelegate{ FTimerDelegate::CreateUObject(this, &ABattleArea::HandleTurnEffects, 0) };
	GetWorld()->GetTimerManager().SetTimer(unusedHandle, turnEffectDelegate, 0.5f, false);
}

void ABattleArea::HandleCardsDrawn(ETarotCards pickedCard, float effectDelay)
{
	FTimerHandle unusedHandle{};
	FTimerHandle executeEffectHandle{};
	FTimerManager& timerManager{GetWorld()->GetTimerManager()};

	TarotSpawnerComponent->SetTarotTextImage(TarotSpawnerComponent->CardTextImages[pickedCard]);

	timerManager.SetTimer(executeEffectHandle, FTimerDelegate::CreateLambda([&, pickedCard]
		{ 
			switch (pickedCard)
			{
			case ETarotCards::Fool:
				timerManager.SetTimer(unusedHandle, this, &ABattleArea::NextTurn, 1.f, false);
				break;
			case ETarotCards::Hermit:
				EndBattle();
				break;
			case ETarotCards::Devil:
				Player->GetStatsComponent()->UpdateHealth(-FMath::RandRange(5, 10));
				UpdateParticipant(Player);
				EndBattle();
				break;
			}
		}
	), effectDelay, false);

	
}

void ABattleArea::SelectAdjacentEnemy(int direction)
{
	SelectedEnemyIdx += direction;
	bool enableSelectionLeft{ true };
	bool enableSelectionRight{ true };

	if (SelectedEnemyIdx <= 0 || EnemyOrder.Num() == 1)
	{
		enableSelectionLeft = false;
		SelectedEnemyIdx = 0;
	}

	if (SelectedEnemyIdx >= EnemyOrder.Num() - 1 || EnemyOrder.Num() == 1)
	{
		enableSelectionRight = false;
		SelectedEnemyIdx = EnemyOrder.Num() - 1;
	}

	ABaseEnemy* newEnemy{ EnemyOrder[SelectedEnemyIdx] };
	if (newEnemy != SelectedEnemy)
	{
		SelectEnemy(EnemyOrder[SelectedEnemyIdx]);
	}

	BattleScreenWidget->EnableEnemySelectionButtons(enableSelectionLeft, enableSelectionRight);
}

void ABattleArea::CheckToEndBattle()
{
	if (BattleParticipants.Num() <= 1 || IsPlayerDead)
	{ 
		EndBattle();
	}
}

void ABattleArea::UpdateSelectionArrow()
{
	float arrowMovement{ static_cast<float>(FMath::Sin(SelectionArrowSpeed * UGameplayStatics::GetTimeSeconds(GetWorld())) * SelectionArrowAmplitude + SelectionArrowAmplitude)};
	
	SelectionArrowLocation = SelectedEnemy->GetActorLocation() + FVector{ 0.f, 0.f, 2 * SelectedEnemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + arrowMovement};
	SelectEnemyArrow->SetWorldLocation(SelectionArrowLocation);

	FTimerDelegate arrowDelegate{ FTimerDelegate::CreateLambda([this]() {
		if (BattleParticipants.Num() > 0)
		{
			UpdateSelectionArrow();
		}
	}) };
	GetWorld()->GetTimerManager().SetTimerForNextTick(arrowDelegate);
}

void ABattleArea::ResetToSelectedEnemy(ABaseEnemy* enemy, bool shouldAttack)
{
	float enemySideX{ static_cast<float>(EnemySide->GetComponentLocation().X) };

	for (ABaseEnemy* resetEnemy : EnemyOrder)
	{
		FVector enemyLocation{ resetEnemy->GetActorLocation() };

		if (resetEnemy != enemy)
		{
			enemyLocation.X = enemySideX + EnemyDepth;
			UpdateEnemyXPosition(resetEnemy, resetEnemy->GetActorLocation(), enemyLocation, 0.f, false);
		}
		else
		{
			enemyLocation.X = enemySideX;
			UpdateEnemyXPosition(resetEnemy, resetEnemy->GetActorLocation(), enemyLocation, 0.f, shouldAttack);

			
			UStatsComponent* stats{ resetEnemy->GetStatsComponent() };
			BattleScreenWidget->UpdateEnemyHealthbar(stats->GetHealthPercentage(), stats->GetCurrentHealth());
			BattleScreenWidget->SetEnemyImage(resetEnemy);
		}
	}
}

void ABattleArea::UpdateEnemyXPosition(ABaseEnemy* enemy, const FVector& startPos, const FVector& endPos, float elapsedTime, bool shouldAttackOnArrival)
{
	float deltaTime{static_cast<float>(UGameplayStatics::GetWorldDeltaSeconds(GetWorld()))};
	elapsedTime += deltaTime * 4.f;
	FVector moveVector{ endPos - startPos };	

	if ((endPos - startPos).IsNearlyZero(0.5f))
	{
		elapsedTime = 1.1f;
	}
	else
	{
		enemy->SetActorLocation(startPos + moveVector * EnemyLerpCurve->GetFloatValue(elapsedTime));
	}

	FTimerDelegate positionDelegate{ FTimerDelegate::CreateLambda([=, this]() {
		if (elapsedTime <= 1.f)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Magenta, "updating position of " + enemy->GetName());
			UpdateEnemyXPosition(enemy, startPos, endPos, elapsedTime, shouldAttackOnArrival);
		}
		else 
		{
			enemy->SetActorLocation(endPos);
			if (shouldAttackOnArrival)
			{
				enemy->PrepareAttack(Player);
			}
		}
	}) };
	GetWorld()->GetTimerManager().SetTimerForNextTick(positionDelegate);
}

void ABattleArea::UpdateSpeedStats()
{
	EnemySpeed = 0;
	for (TPair<ABaseCharacter*, FBattleParticipant>& participant : BattleParticipants)
	{
		if (participant.Key->IsA<ABaseEnemy>())
		{
			int enemySp{ participant.Key->GetStatsComponent()->GetCurrentSpeed() };
			if (enemySp > EnemySpeed)
			{
				EnemySpeed = enemySp;
			}
		}
		else
		{
			PlayerSpeed = participant.Key->GetStatsComponent()->GetCurrentSpeed();
		}
	}
}

void ABattleArea::UpdateHealthbar(EParticipantType participantType)
{
	switch (participantType)
	{
	case EParticipantType::Enemy:
	{
		UStatsComponent* stats{ SelectedEnemy->GetStatsComponent() };
		BattleScreenWidget->UpdateEnemyHealthbar(stats->GetHealthPercentage(), stats->GetCurrentHealth());
		break;
	}
	case EParticipantType::Player:
	{
		UStatsComponent* stats{Player->GetStatsComponent()};
		BattleScreenWidget->UpdatePlayerHealthbar(stats->GetHealthPercentage(), stats->GetCurrentHealth());
		break;
	}
	}
}

void FBattleParticipant::ApplyEffect(ABaseCharacter* baseCharacter)
{
	FString battleMessage{};
	bool isPositive{ TurnEffect.EffectValue >= 0 };

	UStatsComponent* statsComponent{ baseCharacter->GetStatsComponent() };
	switch (TurnEffect.EffectType)
	{
	case EEffectType::Health:
		statsComponent->UpdateHealth(TurnEffect.EffectValue);

		break;
	case EEffectType::Speed:
		statsComponent->UpdateSpeed(TurnEffect.EffectValue);

		break;
	case EEffectType::Luck:

		statsComponent->UpdateLuck(TurnEffect.EffectValue);

		break;
	case EEffectType::Strength:
		statsComponent->UpdateStrength(TurnEffect.EffectValue);

		break;
	case EEffectType::None:
	default:
		break;
	}

	if (!battleMessage.IsEmpty())
	{
		baseCharacter->OnBattleMessage.Broadcast(battleMessage, (isPositive ? EMessageType::Positive : EMessageType::Negative));
	}

	if (TurnEffect.EffectDuration > 0)
	{
		--TurnEffect.EffectDuration;
	}
	else
	{
		if (TurnEffect.OnEffectEnded.IsBound())
		{
			TurnEffect.OnEffectEnded.Broadcast();
		}
		TurnEffect.EffectType = EEffectType::None;
	}
}

bool FBattleParticipant::IsValidEffect() const
{
	return TurnEffect.EffectType != EEffectType::None;
}
