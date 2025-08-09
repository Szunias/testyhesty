// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMessageType : uint8 {
    Normal           UMETA(DisplayName = "Normal"),
    Positive           UMETA(DisplayName = "Postive"),
    Negative           UMETA(DisplayName = "Negative"),
};
