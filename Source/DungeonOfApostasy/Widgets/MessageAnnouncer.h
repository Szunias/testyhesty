
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Other/EMessageType.h"
#include "MessageAnnouncer.generated.h"

class UScrollBox;
class UTimedMessage;

UCLASS()
class DUNGEONOFAPOSTASY_API UMessageAnnouncer : public UUserWidget
{
	GENERATED_BODY()

public:

	void AddMessage(const FString& message, EMessageType messageType);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UScrollBox* Message_Sb;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timed Message")
	float MessageTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timed Message")
	TSubclassOf<UTimedMessage> TimedMessageClass;
};
