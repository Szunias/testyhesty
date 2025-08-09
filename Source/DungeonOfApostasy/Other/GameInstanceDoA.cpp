
#include "GameInstanceDoA.h"
#include "../Widgets/LoadingScreen.h"

void UGameInstanceDoA::ShowLoadingScreen()
{
	CurrentLoadingScreen = CreateWidget<ULoadingScreen>(GetWorld(), LoadingScreenClass);
	CurrentLoadingScreen->AddToViewport();
}

void UGameInstanceDoA::RemoveLoadingScreen()
{
	if (IsValid(CurrentLoadingScreen))
	{
		CurrentLoadingScreen->RemoveFromParent();
		CurrentLoadingScreen = nullptr;
	}
}
