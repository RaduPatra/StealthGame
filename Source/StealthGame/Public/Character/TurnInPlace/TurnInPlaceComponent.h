// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SAnimInstance.h"
#include "SGTurnInPlaceAnimData.h"
#include "Components/ActorComponent.h"
#include "TurnInPlaceComponent.generated.h"


class USGTurnInPlaceAnimData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEALTHGAME_API UTurnInPlaceComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class USAnimInstance;

public:
	// Sets default values for this component's properties
	UTurnInPlaceComponent();

	UFUNCTION(BlueprintCallable)
	void StartTurnInPlace(FRotator TargetRotation);

	UFUNCTION(BlueprintCallable)
	void InterruptTurnInPlace();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FTurnInPlaceFinished OnTurnInPlaceFinished;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	USAnimInstance* MyAnimInstance;

	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY(BlueprintReadOnly)
	float DesiredYaw = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float StartYaw = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float EndYaw = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float StartAnimThreshold = 60.f;

	UPROPERTY(BlueprintReadOnly)	
	ERotationType CurrentRotationState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float LerpSpeed = 150.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Turn90MaxAngle = 135.f;

	bool bTurnClockwise = false;

	
	UPROPERTY(EditDefaultsOnly)
	USGTurnInPlaceAnimData* TurnAnimData;

	//called from anim instance (I think its better to do this because GetCurveValue should be called as soon as anim is updated?)
	void UpdateTurnInPlaceRotation(float DeltaSeconds);

	void FinishTurn(bool bInterrupted);

	bool IsAnimPoseRotating() const
	{
		if (!TurnAnimData) return false;
		return MyAnimInstance->GetCurveValue(TurnAnimData->RotationWeightCurveName) > 0;
	}

	FRotator GetInterpRotation(const FRotator& StartRotation, const FRotator& EndRotation, const float DeltaTime) const
	{
		return FMath::RInterpConstantTo(StartRotation, EndRotation, DeltaTime, LerpSpeed);
	}

	FRotator GetAnimInterpRotation(const FRotator& StartRotation, const FRotator& EndRotation,
								   const float TurnPercentage) const
	{
		return UKismetMathLibrary::REase(StartRotation, EndRotation, TurnPercentage, true, EEasingFunc::Linear);
	}

	UFUNCTION(BlueprintCallable)
	UAnimSequence* GetTurnAnimation() const;
};
