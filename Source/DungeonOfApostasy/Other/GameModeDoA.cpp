
#include "GameModeDoA.h"
#include "../Widgets/GameOverScreen.h"
#include "../Widgets/WinScreen.h"

void AGameModeDoA::EndGame()
{
	GameOverScreen = CreateWidget<UGameOverScreen>(GetWorld(), GameOverScreenClass);
	GameOverScreen->InitializeWidget();
}

void AGameModeDoA::WinGame()
{

	WinScreen = CreateWidget<UWinScreen>(GetWorld(), WinScreenClass);
	WinScreen->InitializeWidget();
}
