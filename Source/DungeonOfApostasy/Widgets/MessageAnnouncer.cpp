#include "MessageAnnouncer.h"
#include "TimedMessage.h"
#include "Components/Scrollbox.h"


void UMessageAnnouncer::AddMessage(const FString& message, EMessageType messageType)
{
	UTimedMessage* timedMessage{ CreateWidget<UTimedMessage>(GetWorld(), TimedMessageClass) };
	timedMessage->SetMessage(message, messageType);
	timedMessage->StartCountdown(MessageTime);
	Message_Sb->AddChild(timedMessage);
}
