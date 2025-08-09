#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ETarotCards : uint8 {
    Devil           UMETA(DisplayName = "The Devil"),
    Hermit          UMETA(DisplayName = "The Hermit"),
    Tower           UMETA(DisplayName = "The Tower"),
    Strength        UMETA(DisplayName = "Strength"),
    Fool            UMETA(DisplayName = "The Fool"),
    Temperance      UMETA(displayName = "Temperance"),
};