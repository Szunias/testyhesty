#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "../Other/EMessageType.h"
#include "BattleScreenWidget.generated.h"

class UButton;
class UMenuButton;
class ABaseCharacter;
class UScrollBox;
class UTurnOrderItem;
class UProgressBar;
class UTexture2D;
class UImage;
class UMessageAnnouncer;
class UTextBlock;
class UCurveFloat;
class ABaseEnemy;

UCLASS()
class DUNGEONOFAPOSTASY_API UBattleScreenWidget : public UBaseWidget
{
	GENERATED_BODY()

public:

	ABaseEnemy* GetActiveEnemy() const;

	void AnnounceMessage(const FString& message, EMessageType messageType);

	void SetTurnOrder(const TArray<UTexture2D*>& previousTurns, const TArray<UTexture2D*>& turnOrder);

	UFUNCTION()
	void UnlockButtons();

	void DisableFlee();

	virtual void InitializeWidget() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerFlee);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnPlayerFlee OnPlayerFlee;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAttack);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnPlayerAttack OnPlayerAttack;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemySelection, int, direction);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnEnemySelection OnEnemySelection;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartAnimationFinished);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnStartAnimationFinished OnStartAnimationFinished;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDamage);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnPlayerDamage OnPlayerDamage;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDamage);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnEnemyDamage OnEnemyDamage;

	UButton* GetAttackButton();
	UButton* GetFleeButton();
	UButton* GetInventoryButton();

	UFUNCTION()
	void OnPlayerDamageTaken();

	UFUNCTION()
	void OnEnemyDamageTaken();

	void UpdatePlayerHealthbar(float healthPercentage, int currentHealth);
	void UpdateEnemyHealthbar(float healthPercentage, int currentHealth);

	void SetEnemyImage(ABaseEnemy* enemy);

	void EnableEnemySelectionButtons(bool enableLeft, bool enableRight);

	UFUNCTION()
	void LockButtons();

protected:
	void SetButtonsAvailable(bool available);

	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
	UMenuButton* Attack_Btn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UMenuButton* Flee_Btn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UMenuButton* Inventory_Btn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UMenuButton* EnemyLeft_Btn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UMenuButton* EnemyRight_Btn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UScrollBox* Turn_Sb;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UProgressBar* PlayerHealth_Pb;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* PlayerHealth_Tb;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UProgressBar* EnemyHealth_Pb;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* EnemyHealth_Tb;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* Enemy_Img;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UMessageAnnouncer* MessageAnnouncer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Container Widgets")
	TSubclassOf<UTurnOrderItem> TurnOrderItemClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Container Widgets")
	UCurveFloat* TurnOrderCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Container Widgets")
	UCurveFloat* TurnOrderOpacityCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Container Widgets")
	float TransitionSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Container Widgets")
	float BoxSizeOffset;


private:

	void AddCharacterIcon(UTexture2D* icon, float size);

	void UpdateCharacterIconColor(int idx, const FLinearColor& color);
	void UpdateCharacterIconImage(int idx, UTexture2D* icon);
	void UpdateCharacterIconSize(int idx, float size);
	void UpdateCharacterIcon(int idx, UTexture2D* icon, float size, const FLinearColor& color);

	float GetLastBoxSize() const;

	void LerpTurnOrder();

	UFUNCTION()
	void SelectEnemyLeft();

	UFUNCTION()
	void SelectEnemyRight();

	float IndexOffset;

	bool CanFlee;
	bool CanChangeEnemySelectionLeft;
	bool CanChangeEnemySelectionRight;
	bool IsFirstTurn;

	int HiddenIconIdx;

	TArray<UTexture2D*> TurnOrderIcons;
	TArray<UTexture2D*> TurnOrderIconsPrevious;
	TArray<UTurnOrderItem*> TurnOrderItems;

	UPROPERTY()
	ABaseEnemy* ActiveEnemy;
};
