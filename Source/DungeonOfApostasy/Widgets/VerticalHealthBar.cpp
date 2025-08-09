#include "VerticalHealthBar.h"
#include "Components/ProgressBar.h"

void UVerticalHealthBar::UpdateHealthPercentage(float percentage)
{
	HealthBar->SetPercent(percentage);
}
