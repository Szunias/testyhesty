
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Other/FTurnEffect.h"
#include "BattleArea.generated.h"

class UBillboardComponent;
class USceneComponent;
class ACameraTrigger;
class UBattleScreenWidget;
class ABaseEnemy;
class APlayerCharacter;
class ABaseCharacter;
class UTarotSpawnerComponent;
class UProgressBar;
class UTexture2D;
class UInventoryScreenWidget;
struct FTurnEffect;
class UCurveFloat;

UENUM(BlueprintType)
enum class EParticipantType : uint8 {
	Player       UMETA(DisplayName = "Player"),
	Enemy        UMETA(DisplayName = "Enemy")
};

USTRUCT(BlueprintType)
struct FBattleParticipant
{
	GENERATED_BODY()

	FTransform OriginalTransform;
	EParticipantType ParticipantType;
	bool HasPriorityTurn;
	int CurrentSpeed;
	FTurnEffect TurnEffect;

	void ApplyEffect(ABaseCharacter* baseCharacter);
	bool IsValidEffect() const;
};

UCLASS()
class DUNGEONOFAPOSTASY_API ABattleArea : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABattleArea();

	void StartBattle(APlayerCharacter* player, const TArray<ABaseEnemy*>& enemies);
	bool IsBattleActive();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBillboardComponent* PlayerSide;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBillboardComponent* EnemySide;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBillboardComponent* SelectEnemyArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACameraTrigger* BattleCamera;

	UFUNCTION()
	void ApplyEffect(ABaseCharacter* target, FTurnEffect turnEffect);

	UFUNCTION()
	void AddBattleMessage(FString message, EMessageType messageType);

	UFUNCTION()
	void CloseInventory();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleStart);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnBattleStart OnBattleStart;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleEnd);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnBattleEnd OnBattleEnd;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateAllParticipants();

	UFUNCTION()
	void EndTurn();

	UFUNCTION()
	void HandleItemUse();

	UPROPERTY(VisibleAnywhere)
	UTarotSpawnerComponent* TarotSpawnerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UBattleScreenWidget> BattleScreenWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInventoryScreenWidget> InventoryScreenClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBattleScreenWidget* BattleScreenWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection Arrow")
	float EnemyDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection Arrow")
	float SelectionArrowAmplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection Arrow")
	float SelectionArrowSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Battle")
	UCurveFloat* EnemyLerpCurve;

	UPROPERTY(BlueprintReadOnly)
	bool CanPlayerWin;
private:


	ABaseCharacter* FindNextCharacterTurn(TMap<ABaseCharacter*, FBattleParticipant>& participants);

	void SimulateTurns(TMap<ABaseCharacter*, FBattleParticipant>& participants, TArray<ABaseCharacter*>& characters, int order = 0);

	UFUNCTION()
	void NextTurn();

	UFUNCTION()
	void EndBattle();

	UFUNCTION()
	void AttemptFlee();

	UFUNCTION()
	void OpenInventory();


	UFUNCTION()
	void OnItemUsed(ABaseItem* usedItem);

	UFUNCTION()
	void OnItemThrown(ABaseItem* thrownItem);

	void UpdateSpeedStats();

	void HandleTurnEffects(int idx = 0);

	void UpdateHealthbar(EParticipantType participantType);

	void UpdateTurnOrder();

	void SelectEnemy(ABaseEnemy* enemy, bool lerpPosition = true);

	bool UpdateParticipant(ABaseCharacter* participant);

	UFUNCTION()
	void HandleCardsDrawn(ETarotCards pickedCard, float effectDelay);

	UFUNCTION()
	void SelectAdjacentEnemy(int direction);

	UFUNCTION()
	void CheckToEndBattle();

	UFUNCTION()
	void UpdateSelectionArrow();

	UFUNCTION()
	void UpdateEnemyXPosition(ABaseEnemy* enemy, const FVector& startPos, const FVector& endPos, float elapsedTime, bool shouldAttackOnArrival);

	void ResetToSelectedEnemy(ABaseEnemy* enemy, bool shouldAttack);

	UPROPERTY()
	TMap<ABaseCharacter*, FBattleParticipant> BattleParticipants;

	UPROPERTY()
	TArray<ABaseEnemy*> EnemyOrder;

	UPROPERTY()
	TArray<ABaseCharacter*> OrderedBattleParticipants;

	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	ABaseEnemy* SelectedEnemy;

	int SelectedEnemyIdx;

	FVector SelectionArrowLocation;

	UPROPERTY()
	UInventoryScreenWidget* InventoryScreen;

	UPROPERTY()
	TArray<UTexture2D*> PreviousCharacterImages;

	int MaxTurnSimulation;
	int PreviousTurnAmount;

	int PlayerSpeed;
	int EnemySpeed;

	bool IsPlayerDead;
};
