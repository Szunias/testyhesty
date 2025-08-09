#include "BattleScreenWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "TurnOrderItem.h"
#include "../Characters/BaseCharacter.h"
#include "Components/ProgressBar.h"
#include "Engine/Texture2D.h"
#include "ControllerButton.h"
#include "MenuButton.h"
#include "Components/Image.h"
#include "MessageAnnouncer.h"
#include "Components/TextBlock.h"
#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/Enemies/BaseEnemy.h"

void UBattleScreenWidget::AnnounceMessage(const FString& message, EMessageType messageType)
{
	MessageAnnouncer->AddMessage(message, messageType);
}

ABaseEnemy* UBattleScreenWidget::GetActiveEnemy() const
{
	return ActiveEnemy;
}

void UBattleScreenWidget::SetTurnOrder(const TArray<UTexture2D*>& previousTurns, const TArray<UTexture2D*>& turnOrder)
{
	if (IsFirstTurn)
	{
		IsFirstTurn = false;
		return;
	}
	
	if (TurnOrderIconsPrevious.Num() == 0)
	{
		HiddenIconIdx = previousTurns.Num() - 1;
		TurnOrderIconsPrevious.Append(previousTurns);
		TurnOrderIconsPrevious.Append(turnOrder);
		for (int i{ 0 }; i < TurnOrderIconsPrevious.Num(); ++i)
		{
			AddCharacterIcon(TurnOrderIconsPrevious[i], TurnOrderCurve->GetFloatValue(static_cast<float>(i)));
		}

		for (int i{ 0 }; i < previousTurns.Num(); ++i)
		{
			UpdateCharacterIconColor(i, FLinearColor{0.f, 0.f, 0.f, 0.f});
		}
		
		IsFirstTurn = true;
	}
	else
	{
		TurnOrderIcons.Empty();
		TurnOrderIcons.Append(previousTurns);
		TurnOrderIcons.Append(turnOrder);
		Turn_Sb->SetScrollOffset(0.f);
		LerpTurnOrder();
	}	
}

void UBattleScreenWidget::UnlockButtons()
{
	SetButtonsAvailable(true);
}

void UBattleScreenWidget::DisableFlee()
{
	CanFlee = false;
	Flee_Btn->EnableButton(false);
}

void UBattleScreenWidget::SetButtonsAvailable(bool available)
{
	Attack_Btn->EnableButton(available);

	if (CanFlee)
	{
		Flee_Btn->EnableButton(available);
	}
	
	Inventory_Btn->EnableButton(available);

	if (CanChangeEnemySelectionLeft)
	{
		EnemyLeft_Btn->EnableButton(available);
	}

	if (CanChangeEnemySelectionRight)
	{
		EnemyRight_Btn->EnableButton(available);
	}
}

void UBattleScreenWidget::NativeDestruct()
{
	GEngine->ForceGarbageCollection();
}

void UBattleScreenWidget::LockButtons()
{
	SetButtonsAvailable(false);
	
}

void UBattleScreenWidget::AddCharacterIcon(UTexture2D* icon, float size)
{
	UTurnOrderItem* item{ CreateWidget<UTurnOrderItem>(GetWorld(), TurnOrderItemClass) };
	item->SetImage(icon);
	item->SetImageSize(size);
	item->SetBoxSize(size + BoxSizeOffset);
	TurnOrderItems.Add(item);
	Turn_Sb->AddChild(item);
}

void UBattleScreenWidget::UpdateCharacterIconColor(int idx, const FLinearColor& color)
{
	UTurnOrderItem* item{ TurnOrderItems[idx] };
	item->SetImageColor(color);
}

void UBattleScreenWidget::UpdateCharacterIconImage(int idx, UTexture2D* icon)
{
	UTurnOrderItem* item{ TurnOrderItems[idx] };
	item->SetImage(icon);
}

void UBattleScreenWidget::UpdateCharacterIconSize(int idx, float size)
{
	UTurnOrderItem* item{ TurnOrderItems[idx] };
	item->SetImageSize(size);
	item->SetBoxSize(size + BoxSizeOffset);
}

void UBattleScreenWidget::UpdateCharacterIcon(int idx, UTexture2D* icon, float size, const FLinearColor& color)
{
	UTurnOrderItem* item{ TurnOrderItems[idx] };
	item->SetImage(icon);
	item->SetImageSize(size);
	item->SetBoxSize(size + BoxSizeOffset);
	item->SetImageColor(color);
}

float UBattleScreenWidget::GetLastBoxSize() const
{
	return TurnOrderCurve->GetFloatValue(static_cast<float>(TurnOrderItems.Num())) + BoxSizeOffset;
}

void UBattleScreenWidget::LerpTurnOrder()
{
	IndexOffset += static_cast<float>(UGameplayStatics::GetWorldDeltaSeconds(GetWorld())) * TransitionSpeed;

	if (IndexOffset < 1.f)
	{
		for (int i{ 0 }; i < TurnOrderIconsPrevious.Num(); ++i)
		{
			float decimalIndex{ static_cast<float>(i) - IndexOffset };
			UpdateCharacterIconSize(i, TurnOrderCurve->GetFloatValue(decimalIndex));

			FLinearColor color{ 1.f, 1.f, 1.f, (decimalIndex >= HiddenIconIdx ? TurnOrderOpacityCurve->GetFloatValue(decimalIndex) : 0.f) };
			UpdateCharacterIconColor(i, color);
		}		

		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UBattleScreenWidget::LerpTurnOrder);
		Turn_Sb->SetScrollOffset(GetLastBoxSize() * IndexOffset);
	}
	else
	{
		for (int i{ 0 }; i < TurnOrderIcons.Num(); ++i)
		{
			float decimalIndex{ static_cast<float>(i)};
			FLinearColor color{ 1.f, 1.f, 1.f, (decimalIndex >= HiddenIconIdx ? TurnOrderOpacityCurve->GetFloatValue(decimalIndex) : 0.f) };
			UpdateCharacterIcon(i, TurnOrderIcons[i], TurnOrderCurve->GetFloatValue(decimalIndex), color);
		}

		if (HiddenIconIdx >= 0)
		{
			--HiddenIconIdx;
		}

		TurnOrderIconsPrevious = TurnOrderIcons;
		IndexOffset = 0.f;
		Turn_Sb->SetScrollOffset(0.f);
	}	
}

void UBattleScreenWidget::SelectEnemyLeft()
{
	OnEnemySelection.Broadcast(-1);
}

void UBattleScreenWidget::SelectEnemyRight()
{
	OnEnemySelection.Broadcast(1);
}

void UBattleScreenWidget::InitializeWidget()
{
	Attack_Btn->GetButton()->OnClicked.AddDynamic(this, &UBattleScreenWidget::LockButtons);
	Flee_Btn->GetButton()->OnClicked.AddDynamic(this, &UBattleScreenWidget::LockButtons);
	EnemyLeft_Btn->GetButton()->OnClicked.AddDynamic(this, &UBattleScreenWidget::SelectEnemyLeft);
	EnemyRight_Btn->GetButton()->OnClicked.AddDynamic(this, &UBattleScreenWidget::SelectEnemyRight);	

	Attack_Btn->OnButtonFocusGained.AddDynamic(this, &UBattleScreenWidget::SetActiveButton);
	Flee_Btn->OnButtonFocusGained.AddDynamic(this, &UBattleScreenWidget::SetActiveButton);
	Inventory_Btn->OnButtonFocusGained.AddDynamic(this, &UBattleScreenWidget::SetActiveButton);
	EnemyLeft_Btn->OnButtonFocusGained.AddDynamic(this, &UBattleScreenWidget::SetActiveButton);
	EnemyRight_Btn->OnButtonFocusGained.AddDynamic(this, &UBattleScreenWidget::SetActiveButton);

	IndexOffset = 0.f;
	IsFirstTurn = false;
	CanFlee = true;
}

UButton* UBattleScreenWidget::GetAttackButton()
{
	return Attack_Btn->GetButton();
}

UButton* UBattleScreenWidget::GetFleeButton()
{
	return Flee_Btn->GetButton();
}

UButton* UBattleScreenWidget::GetInventoryButton()
{
	return Inventory_Btn->GetButton();
}

void UBattleScreenWidget::OnPlayerDamageTaken()
{
	OnPlayerDamage.Broadcast();
}

void UBattleScreenWidget::OnEnemyDamageTaken()
{
	OnEnemyDamage.Broadcast();
}

void UBattleScreenWidget::UpdatePlayerHealthbar(float healthPercentage, int currentHealth)
{
	PlayerHealth_Pb->SetPercent(healthPercentage);
	PlayerHealth_Tb->SetText(FText::FromString(FString::FromInt(currentHealth)));
}

void UBattleScreenWidget::UpdateEnemyHealthbar(float healthPercentage, int currentHealth)
{
	EnemyHealth_Pb->SetPercent(healthPercentage);
	EnemyHealth_Tb->SetText(FText::FromString(FString::FromInt(currentHealth)));
}

void UBattleScreenWidget::SetEnemyImage(ABaseEnemy* enemy)
{
	Enemy_Img->SetBrushFromTexture(enemy->GetCharacterImage());
	ActiveEnemy = enemy;
}


void UBattleScreenWidget::EnableEnemySelectionButtons(bool enableLeft, bool enableRight)
{
	CanChangeEnemySelectionLeft = enableLeft;
	EnemyLeft_Btn->EnableButton(enableLeft);

	CanChangeEnemySelectionRight = enableRight;
	EnemyRight_Btn->EnableButton(enableRight);

	if (EnemyLeft_Btn == ActiveButton && !enableLeft)
	{
		if (enableRight)
		{
			SetButtonActiveAndFocus(EnemyRight_Btn);
		}
		else
		{
			SetButtonActiveAndFocus(Attack_Btn);
		}
	}
	else if (EnemyRight_Btn == ActiveButton && !enableRight)
	{
		if (enableLeft)
		{
			SetButtonActiveAndFocus(EnemyLeft_Btn);
		}
		else
		{
			SetButtonActiveAndFocus(Attack_Btn);
		}
	}
}
