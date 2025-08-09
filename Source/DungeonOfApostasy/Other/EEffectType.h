#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEffectType : uint8 {
    None            UMETA(DisplayName = "None"),
    Health          UMETA(DisplayName = "Health"),
    Strength        UMETA(DisplayName = "Strength"),
    Luck            UMETA(DisplayName = "Health"),
    Speed           UMETA(DisplayName = "Speed"),
    Evasiveness     UMETA(DisplayName = "Evasiveness")
};
