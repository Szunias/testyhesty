#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UInventoryComponent;
class UTarotSpawnerComponent;
struct FInputActionValue;
class UPlayerHudWidget;
class UInventoryScreenWidget;
class ABaseEnemy;
class ABattleArea;
class ACameraTrigger;
class ABaseWeapon;
class AThrowReticle;
class UBoxComponent;
class AEnemyManager;

UCLASS()
class DUNGEONOFAPOSTASY_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:

	APlayerCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UpdateControlRotation(const FRotator& newRotation);

	UFUNCTION()
	void CloseInventory();

	UInventoryComponent* GetInventory();

	bool UpdateUICounter(int amount);

	void ToggleInteractMode(bool active, ACameraTrigger* switchCamera = nullptr);

	void InitiateBattle(TArray<ABaseEnemy*> enemies);
	virtual void OnBattleStart() override;
	virtual void OnBattleEnd() override;
	
	UFUNCTION(BlueprintCallable)
	void BlockInventory(bool block);

	void StorePreviousCameraAndSwitch(ACameraTrigger* switchCamera);
	void SwitchToPreviousCamera();

	virtual void PrepareAttack(ABaseCharacter* target) override;
	virtual void PerformAttack() override;

	UFUNCTION()
	virtual void Attack() override;

	void SetCurrentEquippedWeapon(ABaseWeapon* weaponToEquip, TSubclassOf<AActor> weaponToSpawn);

	UFUNCTION(BlueprintCallable)
	bool IsPlayerInBattle();

	bool IsPlayerObscured();
	void SetPlayerObscured(bool isObscured);

	void ChangeOverlaps(bool newValue);

	UFUNCTION(BlueprintCallable)
	void FreezeAllEnemies();

	UFUNCTION(BlueprintCallable)
	void UnFreezeAllEnemies();

	UFUNCTION(BlueprintCallable)
	void DisableMovement();

	void SetIsInteracting(bool isInteracting);
	bool GetIsInteracting();

protected:
	virtual void BeginPlay() override;

	
	void Move(const FInputActionValue& value);

	void Aim(const FInputActionValue& value);

	void StopInteraction(const FInputActionValue& value);

	void MoveStarted(const FInputActionValue& value);

	void OpenInventory(const FInputActionValue& value);

	void Interact(const FInputActionValue& value);

	UFUNCTION()
	void HandleItemUsed(ABaseItem* usedItem);

	UFUNCTION()
	void HandleItemThrown(ABaseItem* thrownItem);

	UFUNCTION(BlueprintCallable)
	void LaunchItem();

	UFUNCTION(BlueprintCallable)
	void ActivateAiming();

	UFUNCTION(BlueprintCallable)
	void DeactivateAiming();

	UPROPERTY()
	UInventoryComponent* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerHudWidget> PlayerHudClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventoryScreenWidget> InventoryScreenClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<AThrowReticle> ThrowReticleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float BattleTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanAim;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLaunchItem);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnLaunchItem OnLaunchItem;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivateThrow);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnActivateThrow OnActivateThrow;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeactivateThrow);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnDeactivateThrow OnDeactivateThrow;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponEquip);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnWeaponEquip OnWeaponEquip;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequip);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnWeaponUnequip OnWeaponUnequip;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleStarted);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnBattleStarted OnBattleStarted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleEnded);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnBattleEnded OnBattleEnded;

	UPROPERTY(BlueprintReadOnly)
	ABaseWeapon* CurrentEquippedWeapon;

	UFUNCTION(BlueprintCallable)
	void AddWeaponModelToPlayer();

	UFUNCTION(BlueprintCallable)
	void RemoveWeaponModelFromPlayer();

	UFUNCTION(BlueprintCallable)
	void EnableMovement();

	UFUNCTION(BlueprintCallable)
	bool HasWeaponEquipped() const;

	UPROPERTY(BlueprintReadWrite)
	ABaseItem* ItemToThrow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float ReticleSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* ActiveMesh;

private:
	UFUNCTION()
	void PlayerDied();

	UFUNCTION()
	void EndGame();

	UFUNCTION()
	void ResetCanEnterBattle();

	void AddItemMeshToPlayer(ABaseItem* item);

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StopInteractAction;

	/** OpenInventory Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* OpenInventoryAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere)
	FRotator CurrentRotation;

	UPROPERTY()
	UPlayerHudWidget* PlayerHud;

	UPROPERTY()
	UInventoryScreenWidget* InventoryScreen;

	UPROPERTY()
	ABattleArea* BattleArena;

	UPROPERTY()
	AThrowReticle* ThrowReticle;

	int OpenUIAmount;
	bool IsInventoryBlocked;

	bool CanEnterBattle;

	UPROPERTY()
	ACameraTrigger* PreviousCamera;

	FVector2D ScreenAimLocation;


	bool IsObscured;

	FName SocketName;

	UPROPERTY()
	TArray<AActor*> PrevMeshes;

	TSubclassOf<AActor> MeshToEquip;

	bool ForceStopMovement;

	UPROPERTY()
	UBoxComponent* InteractionBox;

	UPROPERTY()
	AEnemyManager* EnemyManager;

	bool IsInteracting;
};
