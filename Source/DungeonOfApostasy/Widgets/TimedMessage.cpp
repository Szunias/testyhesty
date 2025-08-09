#include "TimedMessage.h"
#include "Components/TextBlock.h"

void UTimedMessage::SetMessage(const FString& message, EMessageType messageType)
{
	switch (messageType)
	{
	case EMessageType::Normal:
		Message_Tb->SetColorAndOpacity(DefaultColor);
		break;
	case EMessageType::Positive:
		Message_Tb->SetColorAndOpacity(PositiveColor);
		break;
	case EMessageType::Negative:
		Message_Tb->SetColorAndOpacity(NegativeColor);
		break;
	}

	Message_Tb->SetText(FText::FromString(message));
}

void UTimedMessage::StartCountdown(float countdownDelay)
{
	FTimerHandle unusedHandle{};
	GetWorld()->GetTimerManager().SetTimer(unusedHandle, this, &UTimedMessage::RemoveFromParent, countdownDelay, false);
}
