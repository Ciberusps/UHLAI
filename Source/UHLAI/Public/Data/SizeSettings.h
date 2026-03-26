// Pavel Penkov 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SizeSettings.generated.h"

USTRUCT(Blueprintable)
struct FSizeSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BoundingRadius = 0.0f;
};
