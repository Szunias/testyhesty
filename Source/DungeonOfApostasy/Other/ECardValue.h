#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECardValue : uint8 {
    Positive        UMETA(DisplayName = "Positive"),
    Negative        UMETA(DisplayName = "Negative")
};
