// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ControllerButton.generated.h"


UCLASS()
class DUNGEONOFAPOSTASY_API UControllerButton : public UButton
{
	GENERATED_BODY()
	
public:

	void HoverButton();
	void UnhoverButton();
	void ClickButton();

private:
	void ButtonDown();
	void ButtonUp();
};
