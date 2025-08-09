#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../InventoryComponent.h"
#include "../../Interactables/TarotSpawnerComponent.h"
#include "../../Widgets/PlayerHudWidget.h"
#include "../StatsComponent.h"
#include "../../Widgets/InventoryScreenWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../../Battle/BattleArea.h"
#include "../Enemies/BaseEnemy.h"
#include "../../Other/InteractableInterface.h"
#include "DrawDebugHelpers.h"
#include "../../Interactables/CameraTrigger.h"
#include "../../Items/BaseWeapon.h"
#include "../../Interactables/ThrowReticle.h"
#include "Components/CapsuleComponent.h"
#include "../../Other/EMessageType.h"
#include "../../Other/GameModeDoA.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../EnemyManager.h"

APlayerCharacter::APlayerCharacter()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	IsInventoryBlocked = false;
	IsObscured = false;
	CanAim = false;
	ReticleSpeed = 10.f;
	SocketName = "WeaponSocket";
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* pPlayerController = GetWorld()->GetFirstPlayerController();

	CanEnterBattle = true;

	OnAttackConnects.AddDynamic(this, &APlayerCharacter::PerformAttack);

	Inventory = GetComponentByClass<UInventoryComponent>();
	if (!IsValid(Inventory))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Inventory in PlayerCharacter not found!!!"));
	}

	//Add Input Mapping Context
	if (pPlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (IsValid(PlayerHudClass))
	{
		PlayerHud = CreateWidget<UPlayerHudWidget>(GetWorld(), PlayerHudClass);
		PlayerHud->SetCurrentHealthPercentage(StatsComponent->GetHealthPercentage());
		StatsComponent->OnUpdateHealth.AddDynamic(PlayerHud, &UPlayerHudWidget::SetCurrentHealthPercentage);
		PlayerHud->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Cannot find player hud");
	}

	BattleArena = Cast<ABattleArea>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleArea::StaticClass()));
	if (!IsValid(BattleArena))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Cannot find battle arena");
	}

	EnemyManager = Cast<AEnemyManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyManager::StaticClass()));
	if (!IsValid(EnemyManager))
	{
		UE_LOG(LogTemp, Fatal, TEXT("EnemyManager in PlayerCharacter not found!!!"));
	}

	InteractionBox = GetComponentByClass<UBoxComponent>();
	if (!IsValid(InteractionBox))
	{
		UE_LOG(LogTemp, Fatal, TEXT("InteractionBox in PlayerCharacter not found!!!"));
	}

	DisableMovement();
	BlockInventory(true);

	StatsComponent->OnDeath.AddDynamic(this, &APlayerCharacter::PlayerDied);
	StatsComponent->OnDeathFinished.AddDynamic(this, &APlayerCharacter::EndGame);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &APlayerCharacter::MoveStarted);
		
		// Open Inventory
		EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OpenInventory);
	
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);
		EnhancedInputComponent->BindAction(StopInteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StopInteraction);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Aim);
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Problem with SetupPlayerInputComponent");
	}
}

void APlayerCharacter::UpdateControlRotation(const FRotator& newRotation)
{
	CurrentRotation = newRotation;
}

void APlayerCharacter::CloseInventory()
{
	if (!IsValid(InventoryScreen))
	{
		return;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "closing Inventory");
	InventoryScreen->RemoveWidget();
	InventoryScreen = nullptr;
}

UInventoryComponent* APlayerCharacter::GetInventory()
{
	return Inventory;
}

bool APlayerCharacter::UpdateUICounter(int amount)
{
	OpenUIAmount += amount;
	if (OpenUIAmount > 0)
	{
		GetCharacterMovement()->Deactivate();
		BlockInventory(true);
		return true;
	}
	else
	{
		if (!ForceStopMovement)
		{
			GetCharacterMovement()->Activate();
		}
		UWidgetBlueprintLibrary::SetFocusToGameViewport();	
		BlockInventory(false);
	}
	return false;
}

void APlayerCharacter::ToggleInteractMode(bool active, ACameraTrigger* switchCamera)
{
	if (active)
	{
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly{});
		if(switchCamera)
			StorePreviousCameraAndSwitch(switchCamera);
		GetCharacterMovement()->Deactivate();
		FreezeAllEnemies();
	}
	else
	{
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly{});
		SwitchToPreviousCamera();
		GetCharacterMovement()->Activate();
		UnFreezeAllEnemies();
	}
	ChangeOverlaps(!active);
	SetIsInteracting(active);
}

void APlayerCharacter::InitiateBattle(TArray<ABaseEnemy*> enemies)
{
	if (!IsValid(BattleArena) || !CanEnterBattle)
	{
		return;
	}
	DeactivateAiming();
	CloseInventory();
	if (enemies.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "No enemies to battle? How did this get triggered?");
	}
	BattleArena->StartBattle(this, enemies);
}

void APlayerCharacter::OnBattleStart()
{
	if (IsValid(PlayerHud))
	{
		PlayerHud->SetVisibility(ESlateVisibility::Hidden);
	}
	GetCharacterMovement()->Deactivate();
	CanEnterBattle = false;
	OnBattleStarted.Broadcast();
	FreezeAllEnemies();
}

void APlayerCharacter::OnBattleEnd()
{
	if (IsValid(PlayerHud))
	{
		PlayerHud->SetVisibility(ESlateVisibility::Visible);
	}
	if (StatsComponent->GetHealthPercentage() > FLT_EPSILON)
	{
		CanAim = false;
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &APlayerCharacter::EnableMovement);

		FTimerHandle timerReset{};
		GetWorld()->GetTimerManager().SetTimer(timerReset, this, &APlayerCharacter::ResetCanEnterBattle, BattleTimer);
		OnBattleEnded.Broadcast();
	}
	UnFreezeAllEnemies();
}

void APlayerCharacter::PlayerDied()
{
	DisableMovement();
	ChangeOverlaps(false);
	CloseInventory();
}

void APlayerCharacter::EndGame()
{
	AGameModeDoA* gameMode = Cast<AGameModeDoA>(GetWorld()->GetAuthGameMode());
	gameMode->EndGame();
}

void APlayerCharacter::ResetCanEnterBattle()
{
	CanEnterBattle = true;
}

void APlayerCharacter::AddItemMeshToPlayer(ABaseItem* item)
{
	for (AActor* prevMesh : PrevMeshes)
	{
		prevMesh->Destroy();
	}
	PrevMeshes.Empty();

	ActiveMesh = GetWorld()->SpawnActor<AActor>(item->GetItemMesh(), FTransform{});
	if (IsValid(ActiveMesh))
	{
		ActiveMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules{ EAttachmentRule::SnapToTarget, false }, SocketName);
	}
}

void APlayerCharacter::Move(const FInputActionValue& value)
{	
	if (CanAim) return;
	// input is a Vector2D
	FVector2D movementVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{

		// find out which way is forward
		const FRotationMatrix rotation{ Controller->GetControlRotation() };

		// get forward vector
		FVector forwardDirection{ rotation.GetUnitAxis(EAxis::X) };
		forwardDirection.Z = 0.0f;

		// get right vector 
		FVector rightDirection{ rotation.GetUnitAxis(EAxis::Y) };
		rightDirection.Z = 0.0f;

		// add movement 
		AddMovementInput(forwardDirection, movementVector.Y);
		AddMovementInput(rightDirection, movementVector.X);
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, forwardDirection.ToString());
	}
}

void APlayerCharacter::Aim(const FInputActionValue& value)
{
	if (CanAim && IsValid(ThrowReticle))
	{
		ScreenAimLocation += value.Get<FVector2D>() * ReticleSpeed;

		FVector worldLocation;
		FVector worldDirection;

		int viewportX;
		int viewportY;
		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		playerController->GetViewportSize(viewportX, viewportY);

		ScreenAimLocation.X = FMath::Clamp(ScreenAimLocation.X, 0.f, viewportX);
		ScreenAimLocation.Y = FMath::Clamp(ScreenAimLocation.Y, 0.f, viewportY);
		//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Purple, ScreenAimLocation.ToString());

		if (!playerController->DeprojectScreenPositionToWorld(ScreenAimLocation.X, ScreenAimLocation.Y, worldLocation, worldDirection))
		{
			UE_LOG(LogTemp, Fatal, TEXT("Cannot determine aim world location"));		
		}		

		FHitResult hitResult{};
		if (GetWorld()->LineTraceSingleByChannel(hitResult, worldLocation, worldLocation + worldDirection * 15000.f, ECollisionChannel::ECC_Visibility))
		{
			FVector actorLocation{ GetMesh()->GetSocketLocation(SocketName) };
			ThrowReticle->CalculateLaunchVelocity(actorLocation, hitResult.Location);
			ItemToThrow->SetActorLocation(actorLocation);
			TArray<AActor*> ignoreActors{};
			ignoreActors.Add(this);
			ignoreActors.Add(ItemToThrow);
			if (IsValid(ActiveMesh))
			{
				ignoreActors.Add(ActiveMesh);
			}
			ThrowReticle->PredictPath(actorLocation, ignoreActors);
		}
		
		FVector lookRotation{ ThrowReticle->GetActorLocation() - GetActorLocation() };
		lookRotation.Z = 0.f;
		SetActorRotation(lookRotation.Rotation());
		
	}
}

void APlayerCharacter::StopInteraction(const FInputActionValue& value)
{
	DeactivateAiming();
}

void APlayerCharacter::MoveStarted(const FInputActionValue& value)
{
	if (Controller != nullptr)
	{
		Controller->SetControlRotation(CurrentRotation);
	}
}

void APlayerCharacter::OpenInventory(const FInputActionValue& value)
{
	if (!IsValid(InventoryScreen) && !IsInventoryBlocked && !CanAim && !BattleArena->IsBattleActive())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Opening Inventory");
		InventoryScreen = CreateWidget<UInventoryScreenWidget>(GetWorld(), InventoryScreenClass);
		InventoryScreen->OnItemUsed.AddDynamic(this, &APlayerCharacter::HandleItemUsed);
		InventoryScreen->OnItemThrown.AddDynamic(this, &APlayerCharacter::HandleItemThrown);
		InventoryScreen->OnBack.AddDynamic(this, &APlayerCharacter::CloseInventory);
		InventoryScreen->IncludeWeaponItems(true);
		InventoryScreen->InitializeWidget();
		InventoryScreen->AddWidget();
	}
	else if (IsValid(BattleArena) && BattleArena->IsBattleActive())
	{
		BattleArena->CloseInventory();
	}
	else 
	{
		CloseInventory();
	}
}

void APlayerCharacter::Interact(const FInputActionValue& value)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "interact button pressed");

	if (OpenUIAmount > 0)
		return;

	if (IsValid(ItemToThrow))
	{
		OnLaunchItem.Broadcast();
		Inventory->RemoveItemFromInventory(ItemToThrow);
		return;
	}

	FVector pos{ InteractionBox->GetComponentLocation() };
	pos += GetActorForwardVector() * 5.0f;
	//DrawDebugBox(GetWorld(), pos, InteractionBox->GetScaledBoxExtent(), InteractionBox->GetComponentQuat(), FColor::Blue, false, 5.0f, (uint8)0U, 4.0f);

	TArray<FHitResult> outHits{};
	
	GetWorld()->SweepMultiByChannel(outHits, pos, pos, InteractionBox->GetComponentQuat(), ECollisionChannel::ECC_Visibility, InteractionBox->GetCollisionShape());
	
	// next one is to debug draw -> also will work
	//UKismetSystemLibrary::BoxTraceMulti(GetWorld(), pos, pos, InteractionBox->GetScaledBoxExtent(), InteractionBox->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery_MAX, true, {}, EDrawDebugTrace::ForDuration, outHits, true);
	if (outHits.IsEmpty())
		return;

	// get all actors that can be interacted with
	TArray<AActor*> interactables{};
	for (const FHitResult& hit : outHits)
	{
		if (AActor* hitActor{ hit.GetActor() }; IsValid(hitActor) && hitActor->Implements<UInteractableInterface>())
		{
			interactables.AddUnique(hitActor);
		}
	}

	// do interactable thing
	for (AActor* actor : interactables)
	{
		IInteractableInterface* interface{Cast<IInteractableInterface>(actor)};
		if (IsValid(interface->_getUObject())) // safety check
		{
			interface->Interact();
		}
	}
}

void APlayerCharacter::HandleItemUsed(ABaseItem* usedItem)
{
	CloseInventory();
}

void APlayerCharacter::HandleItemThrown(ABaseItem* thrownItem)
{
	ItemToThrow = thrownItem;
	CanAim = true;
	CloseInventory();
	OnActivateThrow.Broadcast();
}

void APlayerCharacter::LaunchItem()
{
	if (IsValid(ItemToThrow))
	{
		RemoveWeaponModelFromPlayer();
		ItemToThrow->LaunchItem(ThrowReticle->GetLaunchVelocity());
		ThrowReticle->Destroy();
		ThrowReticle = nullptr;
	}
}

void APlayerCharacter::ActivateAiming()
{
	if (IsPlayerInBattle())
	{
		return;
	}
	ThrowReticle = GetWorld()->SpawnActor<AThrowReticle>(ThrowReticleClass, GetActorTransform());
	if (IsValid(ThrowReticle))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Activating aim");
		ItemToThrow->TeleportItem(GetMesh()->GetSocketLocation(SocketName));
		AddItemMeshToPlayer(ItemToThrow);

		FVector actorLocation{ GetActorLocation() };
		ThrowReticle->CalculateLaunchVelocity(actorLocation, actorLocation + GetActorForwardVector() * 20.f);

		TArray<AActor*> ignoreActors{};
		ignoreActors.Add(this);
		ignoreActors.Add(ItemToThrow);
		if (IsValid(ActiveMesh))
		{
			ignoreActors.Add(ActiveMesh);
		}

		FVector reticleLocation{ ThrowReticle->PredictPath(actorLocation, ignoreActors) };
		ThrowReticle->SetActorLocation(reticleLocation);

		FVector2D screenCoordinates{};
		APlayerController * playerController = GetWorld()->GetFirstPlayerController();
		playerController->ProjectWorldLocationToScreen(reticleLocation, screenCoordinates);

		ScreenAimLocation = screenCoordinates;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		CanAim = true;
	}
}

void APlayerCharacter::DeactivateAiming()
{
	if (CanAim)
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		CanAim = false;
		if (IsValid(ItemToThrow))
		{
			if (!(IsValid(ActiveMesh) && IsValid(CurrentEquippedWeapon) && ActiveMesh->GetClass()->IsChildOf(CurrentEquippedWeapon->GetItemMesh())))
			{
				RemoveWeaponModelFromPlayer();
			}
			
			ItemToThrow = nullptr;
			OnDeactivateThrow.Broadcast();
		}
		if (IsValid(ThrowReticle))
		{
			ThrowReticle->Destroy();
			ThrowReticle = nullptr;
		}
	}
}

void APlayerCharacter::StorePreviousCameraAndSwitch(ACameraTrigger* switchCamera)
{
	if (!IsValid(switchCamera))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "new camera not found");
		return;
	}

	//Saving previous camera view
	APlayerController* playerController{ GetWorld()->GetFirstPlayerController() };
	PreviousCamera = Cast<ACameraTrigger>(playerController->GetViewTarget());

	if (!IsValid(PreviousCamera) || PreviousCamera == switchCamera)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Previous camera not found");
	}

	switchCamera->SwitchToCamera();
}

void APlayerCharacter::SwitchToPreviousCamera()
{
	if (!IsValid(PreviousCamera))
	{
		return;
	}
	PreviousCamera->SwitchToCamera();
}

void APlayerCharacter::PrepareAttack(ABaseCharacter* target)
{
	TargettedEnemy = target;
}

void APlayerCharacter::PerformAttack()
{
	if (!IsValid(TargettedEnemy))
	{
		return;
	}
	// make sure we can hit them

	UStatsComponent* targetStats{ TargettedEnemy->GetStatsComponent() };
	if (FMath::RandRange(0, 99) >= targetStats->GetCurrentEvasiveness())
	{
		int damageGiven{ StatsComponent->GetDamage() };
		targetStats->UpdateHealth(-damageGiven);
	}
	else
	{
		FString battleMessage{ CharacterName + " missed"};
		OnBattleMessage.Broadcast(battleMessage, EMessageType::Negative);
	}
	TargettedEnemy = nullptr;
}

void APlayerCharacter::Attack()
{
	OnAttack.Broadcast();
}

void APlayerCharacter::SetCurrentEquippedWeapon(ABaseWeapon* weaponToEquip, TSubclassOf<AActor> weaponToSpawn)
{
	DisableMovement();

	if (!IsValid(weaponToEquip))
	{
		OnWeaponUnequip.Broadcast();
		CurrentEquippedWeapon = nullptr;
		return;
	}

	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->Unequip();
	}

	CurrentEquippedWeapon = weaponToEquip;
	MeshToEquip = weaponToSpawn;

	OnWeaponEquip.Broadcast();
}

bool APlayerCharacter::IsPlayerInBattle()
{
	if (!IsValid(BattleArena))
	{
		return false;
	}
	else
	{
		return BattleArena->IsBattleActive();
	}
}

bool APlayerCharacter::IsPlayerObscured()
{
	return IsObscured;
}

void APlayerCharacter::SetPlayerObscured(bool isObscured)
{
	IsObscured = isObscured;
}

void APlayerCharacter::ChangeOverlaps(bool newValue)
{
	GetCapsuleComponent()->SetGenerateOverlapEvents(newValue);
	InteractionBox->SetGenerateOverlapEvents(newValue);
}

void APlayerCharacter::FreezeAllEnemies()
{
	EnemyManager->StopMovementForAllEnemies();
}

void APlayerCharacter::UnFreezeAllEnemies()
{
	EnemyManager->ResumeMovementForAllEnemies();
}

void APlayerCharacter::BlockInventory(bool block)
{
	IsInventoryBlocked = block;
}

void APlayerCharacter::AddWeaponModelToPlayer()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, "added weapon model to hand");
	ActiveMesh = GetWorld()->SpawnActor<AActor>(MeshToEquip, FTransform{});
	ActiveMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules{ EAttachmentRule::SnapToTarget, false }, SocketName);
	PrevMeshes.Add(ActiveMesh);
}

void APlayerCharacter::RemoveWeaponModelFromPlayer()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, "Removed model from hand");
	for (AActor* prevMesh : PrevMeshes)
	{
		prevMesh->Destroy();
	}
	PrevMeshes.Empty();
	if (IsValid(ActiveMesh))
	{
		ActiveMesh->Destroy();
		ActiveMesh = nullptr;
	}
}

void APlayerCharacter::DisableMovement()
{
	ForceStopMovement = true;
	GetCharacterMovement()->Deactivate();
}

void APlayerCharacter::SetIsInteracting(bool isInteracting)
{
	IsInteracting = isInteracting;
}

bool APlayerCharacter::GetIsInteracting()
{
	return IsInteracting;
}

void APlayerCharacter::EnableMovement()
{
	if (!IsPlayerInBattle())
	{
		GetCharacterMovement()->Activate();
	}
	ForceStopMovement = false;
}

bool APlayerCharacter::HasWeaponEquipped() const
{
	return IsValid(CurrentEquippedWeapon);
}
