// Pavel Penkov 2025 All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UHLAIBlueprintLibrary.generated.h"

static const float FLOAT_ERROR = -99999.0f;
static const float RELATIVE_POINT_ARROW_SIZE = 200.0f;
static const int32 DEPTH_PRIORITY = -1;

UCLASS()
class UHLAI_API UUHLAIBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    /**
     * Same function as in UHL->UnrealHelperLibraryBPL
     * Calculates relative angle to other actor [-180, 180]
     * @param ActorRelativeToWhomAngleCalculated
     * @param TargetActor
     * @param bRelativeToActorBack sometimes you want to know actors that nearest to character backside
     * @param bDebug
     * @param DebugLifetime
     * @param DebugColor
     */
    UFUNCTION(BlueprintPure, Category = "UHLBTBlueprintLibrary|Angles", meta = (DefaultToSelf = "ActorRelativeToWhomAngleCalculated", Keywords = "UnrealHelperLibrary debug Development angle relative GetAngle RelativeTo", AdvancedDisplay = "bDebug,DebugLifetime,DebugColor,bRelativeToActorBack"))
    static float RelativeAngleToActor(AActor* ActorRelativeToWhomAngleCalculated,
        AActor* TargetActor, bool bRelativeToActorBack = false, const bool bDebug = false,
        const float DebugLifetime = -1, const FLinearColor DebugColor = FLinearColor::White);

    // Same function as in UHL->UnrealHelperLibraryBPL
    UFUNCTION(BlueprintPure, Category = "UHLBTBlueprintLibrary|Angles", meta = (DefaultToSelf = "ActorRelativeToWhomAngleCalculated", Keywords = "UnrealHelperLibrary debug Development angle relative GetAngle RelativeTo"))
    static float RelativeAngleToVector(AActor* ActorRelativeToWhomAngleCalculated, FVector TargetVector);

    UFUNCTION(BlueprintPure, Category = "UnrealHelperLibrary|Utils", meta = (WorldContext = "WorldContextObject", Keywords = "UnrealHelperLibrary vector center"))
    static FVector GetCenterPointBetweenVectors(const UObject* WorldContextObject,
        const FVector& PointA, const FVector& PointB,
        const bool bDebug = false, const float DebugLifetime = -1, const FLinearColor DebugColor = FLinearColor::White);
};
