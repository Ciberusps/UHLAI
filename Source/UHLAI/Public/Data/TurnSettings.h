// Pavel Penkov 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Character.h"
#include "TurnSettings.generated.h"

class UAnimMontage;

USTRUCT(Blueprintable)
struct FTurnRange
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnRange")
    FString Name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnRange")
    FFloatRange Range = FFloatRange(-135, -45);
	// nominal rotation this montage performs (magnitude, degrees). Used only by bUseBestSingleMontage
	// to pick the largest montage whose TurnAngle <= |DeltaAngle| and let motion warping finish the rest.
	// 0 = auto-derive from the Range window (min-magnitude bound) so legacy assets keep working.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnRange", meta=(Units="Degrees"))
	float TurnAngle = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnRange")
    UAnimMontage* AnimMontage = nullptr;
    // useful in big enemies cases, Dragon shouldn't cancel 180deg rotate animation
    // even if Player somehow teleported
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnRange")
    bool bOverrideStopMontageOnGoalReached = false;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnRange", meta=(EditCondition="bOverrideStopMontageOnGoalReached", EditConditionHides))
    bool bStopMontageOnGoalReached = false;
};

USTRUCT(Blueprintable)
struct FTurnRanges
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnRanges")
    TArray<FTurnRange> TurnRanges;
};


// TODO make warning that not all ranges are covered
// TODO limit TurnAnimations count e.g. max 3 times, then give choice what to do - abort or success?
// TODO limit playing similar animations count e.g. max 3 times ?? and then give choice what to do - abort or success?
// TIPS
// - ranges can overlap, in such cases animation will be fired by order, so order is important
USTRUCT(Blueprintable)
struct UHLAI_API FTurnSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurnSettings", meta=(EditCondition="true", MultiLine))
    FString Notes = FString(TEXT(
                                 "Use cases:"
                                 "\n\n1) Big enemy (dragon, etc.) - better to use with \"bTurnOnlyWithAnims\", \"bStopMontageOnGoalReached\" and \"Precision = 1°\", BlendOut settings in AnimMontage becomes critcal for smooth visual ~0.5s blendout + inertionalization recommended"
                                 "\n\n2) Medium enemy (human size) - disable bTurnOnlyWithAnims nobody will mention that medium enemy rotates in place without anim"
                                 "\n\nTips: "
                                 "\n- ranges can overlap, in such cases animation will be fired by order, so order is important"
                                ));

    // if enabled - turn only with animations, if no ranges fits - consider it as success
    // if disabled - turn also with rotating enemy in place
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnSettings")
    bool bTurnOnlyWithAnims = true;
    // stops AnimMontage when reached goal, even if 180deg turn animation on half of playing - stop it
    // BlendOut settings for this option is critical
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnSettings")
    bool bStopMontageOnGoalReached = true;

	// if enabled - pick ONE best-matching montage for the whole turn and play only it:
	// the largest montage whose nominal TurnAngle <= |DeltaAngle| is chosen, and the remaining angle
	// is completed via motion warping inside that montage (e.g. 110deg -> play 90deg montage, warp the last 20deg).
	// No montage chaining and no small residual montages.
	// if disabled - may chain several montages (e.g. 90 then 15) until the goal is reached.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnSettings")
	bool bUseBestSingleMontage = false;
	
    // TODO bChooseClosestInRaceCondition? если подходят 2 ренджа, в чью пользу принимать решение, зач если есть order TurnRange'ей

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnSettings", meta=(ForceInlineRow))
    TMap<FString, FTurnRanges> TurnRangesGroups;

    void Cleanup();
    void SetupPreset_Default_90_180();
    void SetupPreset_BigEnemy_90_180();
    void SetupPreset_45_90_180();
    void SetupPreset_15_45_90_180();
    void SetupPreset_15_30_45_90_180();
};

namespace TurnToStatics
{
	static bool IsTurnWithAnimationRequired(ACharacter* Character)
	{
		if (!Character) return false;
		if (Character->IsPlayingRootMotion()) return false;
		return true;
	}

	static FTurnRange GetTurnRange(float DeltaAngle, bool& bCurrentTurnRangeSet, FTurnSettings TurnSettings_In)
	{
		FTurnRange Result;
		bCurrentTurnRangeSet = false;
		for (TTuple<FString, FTurnRanges> TurnToRange : TurnSettings_In.TurnRangesGroups)
		{
			for (FTurnRange Range : TurnToRange.Value.TurnRanges)
			{
				if (Range.Range.Contains(DeltaAngle))
				{
					Result = Range;
					bCurrentTurnRangeSet = true;
					break;
				}
			}
			if (bCurrentTurnRangeSet)
			{
				break;
			}
		}
		return Result;
	}

	// best-single-montage selection: among ranges on the same side as DeltaAngle, pick the one whose
	// nominal TurnAngle is the largest value still <= |DeltaAngle|. Remainder is left to motion warping.
	// If a range has TurnAngle <= 0 it is auto-derived from the min-magnitude bound of its window.
	static FTurnRange GetBestSingleTurnRange(float DeltaAngle, bool& bCurrentTurnRangeSet, const FTurnSettings& TurnSettings_In)
	{
		FTurnRange Result;
		bCurrentTurnRangeSet = false;
		const float AbsDelta = FMath::Abs(DeltaAngle);
		const bool bRight = DeltaAngle >= 0.f;
		float BestNominal = -1.f;
		for (const TTuple<FString, FTurnRanges>& TurnToRange : TurnSettings_In.TurnRangesGroups)
		{
			for (const FTurnRange& Range : TurnToRange.Value.TurnRanges)
			{
				// side of this range, inferred from its selection window
				const float Mid = 0.5f * (Range.Range.GetLowerBoundValue() + Range.Range.GetUpperBoundValue());
				const bool bRangeRight = Mid >= 0.f;
				if (bRangeRight != bRight)
				{
					continue;
				}
				float Nominal = Range.TurnAngle;
				if (Nominal <= 0.f)
				{
					// auto-derive nominal from the window's min-magnitude bound (variant-1 fallback)
					Nominal = FMath::Min(FMath::Abs(Range.Range.GetLowerBoundValue()), FMath::Abs(Range.Range.GetUpperBoundValue()));
				}
				if (Nominal <= AbsDelta && Nominal > BestNominal)
				{
					BestNominal = Nominal;
					Result = Range;
					bCurrentTurnRangeSet = true;
				}
			}
		}
		return Result;
	}

	FORCEINLINE_DEBUGGABLE FVector::FReal CalculateAngleDifferenceDot(const FVector& VectorA, const FVector& VectorB)
	{
		return (VectorA.IsNearlyZero() || VectorB.IsNearlyZero())
			? 1.f
			: VectorA.CosineAngle2D(VectorB);
	}
}


UCLASS(Blueprintable)
class UHLAI_API UTurnSettingsDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnSettings")
    FTurnSettings TurnSettings;

    // Prefers to rotate 180deg if relative angle >115deg, suits for all medium mobs(human size)
    UFUNCTION(BlueprintCallable, Category="Setup", CallInEditor, meta=(DisplayPriority=1))
    void SetupPreset_Default_90_180();
    // All ranges have same proportion
    UFUNCTION(BlueprintCallable, Category="Setup", CallInEditor, meta=(DisplayPriority=1))
    void SetupPreset_BigEnemy_90_180();
    UFUNCTION(BlueprintCallable, CallInEditor, Category="Setup", meta = (DisplayPriority=2))
    void SetupPreset_45_90_180();
    UFUNCTION(BlueprintCallable, CallInEditor, Category="Setup", meta = (DisplayPriority=3))
    void SetupPreset_15_45_90_180();
    UFUNCTION(BlueprintCallable, CallInEditor, Category="Setup", meta = (DisplayPriority=4))
    void SetupPreset_15_30_45_90_180();
    UFUNCTION(BlueprintCallable, CallInEditor, Category="Setup", meta = (DisplayPriority=5))
    void Cleanup();
};
