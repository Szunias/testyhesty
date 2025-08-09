#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceDoA.generated.h"

class ULoadingScreen;

UCLASS()
class DUNGEONOFAPOSTASY_API UGameInstanceDoA : public UGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ShowLoadingScreen();

	UFUNCTION(BlueprintCallable)
	void RemoveLoadingScreen();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ULoadingScreen> LoadingScreenClass;

private:
	ULoadingScreen* CurrentLoadingScreen;
};
