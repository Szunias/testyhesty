#include "PlayerHudWidget.h"
#include "VerticalHealthBar.h"

void UPlayerHudWidget::SetCurrentHealthPercentage(float percentage)
{
	VerticalHealthBar->UpdateHealthPercentage(percentage);
}
