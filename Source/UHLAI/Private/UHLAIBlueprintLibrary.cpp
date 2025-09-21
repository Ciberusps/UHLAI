// Pavel Penkov 2025 All Rights Reserved.

#include "UHLAIBlueprintLibrary.h"
#include "KismetAnimationLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UHLAIBlueprintLibrary)

float UUHLAIBlueprintLibrary::RelativeAngleToActor(
    AActor* ActorRelativeToWhomAngleCalculated, AActor* TargetActor, bool bRelativeToActorBack, const bool bDebug, const float DebugLifetime, const FLinearColor DebugColor)
{
    if (!IsValid(ActorRelativeToWhomAngleCalculated) || !IsValid(TargetActor))
        return FLOAT_ERROR;

    float Multiplier = bRelativeToActorBack ? 1 : -1;
    float Result = UKismetAnimationLibrary::CalculateDirection(
        ActorRelativeToWhomAngleCalculated->GetActorLocation() - TargetActor->GetActorLocation(), (ActorRelativeToWhomAngleCalculated->GetActorForwardVector() * Multiplier).ToOrientationRotator());

    if (bDebug)
    {
        UWorld* DebugWorld = ActorRelativeToWhomAngleCalculated->GetWorld();
        FColor DebugCol = DebugColor.ToFColor(true);
        FVector LineStart = ActorRelativeToWhomAngleCalculated->GetActorLocation();
        FVector LineEnd = TargetActor->GetActorLocation();
        FVector TextLocation = GetCenterPointBetweenVectors(ActorRelativeToWhomAngleCalculated->GetWorld(), LineStart, LineEnd);

        DrawDebugString(DebugWorld, TextLocation, FString::Printf(TEXT("RelativeAngle: %.2f°"), Result), 0, DebugCol, DebugLifetime, true, 1.0f);
        DrawDebugDirectionalArrow(DebugWorld, LineStart, LineEnd, RELATIVE_POINT_ARROW_SIZE, DebugCol, true, DebugLifetime, DEPTH_PRIORITY, 2);
    }

    return Result;
}

float UUHLAIBlueprintLibrary::RelativeAngleToVector(AActor* ActorRelativeToWhomAngleCalculated, FVector TargetVector)
{
    if (!IsValid(ActorRelativeToWhomAngleCalculated))
        return FLOAT_ERROR;
    return UKismetAnimationLibrary::CalculateDirection(
        ActorRelativeToWhomAngleCalculated->GetActorLocation() - TargetVector, (ActorRelativeToWhomAngleCalculated->GetActorForwardVector() * -1).ToOrientationRotator());
}

FVector UUHLAIBlueprintLibrary::GetCenterPointBetweenVectors(
    const UObject* WorldContextObject, const FVector& PointA, const FVector& PointB,
    const bool bDebug, const float DebugLifetime, const FLinearColor DebugColor)
{
    FVector Result = (PointB - PointA) / 2 + PointA;

    if (bDebug)
    {
        UWorld* DebugWorld = WorldContextObject->GetWorld();
        DrawDebugSphere(DebugWorld, Result, 10.0f, 12, DebugColor.ToFColor(true), true, DebugLifetime, DEPTH_PRIORITY, 1);
    }

    return Result;
}
