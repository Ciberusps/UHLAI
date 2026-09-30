#pragma once
#include "BehaviorTree/ValueOrBBKey.h"

#include "UHLAIDebugSettings.generated.h"

// struct for easier creating BBValue + BBValue math operations
USTRUCT(BlueprintType)
struct FUHLAIDebugSettings
{
	GENERATED_BODY()

	UPROPERTY(Category="Decorator", EditAnywhere)
	bool bEnableDebug = false;
	
	UPROPERTY(Category="Decorator", EditAnywhere)
	FValueOrBBKey_Float DrawDebugTime = 2.0f;

	UPROPERTY(Category="Decorator", EditAnywhere)
	FColor Color = FLinearColor(0.f, 0.66f, 1.f).ToFColor(true);
};
