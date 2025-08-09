#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameModeDoA.generated.h"

class UGameOverScreen;
class UWinScreen;

UCLASS()
class DUNGEONOFAPOSTASY_API AGameModeDoA : public AGameMode
{
	GENERATED_BODY()

public:

	void EndGame();
	void WinGame();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<UGameOverScreen> GameOverScreenClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<UWinScreen> WinScreenClass;

private:

	UGameOverScreen* GameOverScreen;
	UWinScreen* WinScreen;

};
