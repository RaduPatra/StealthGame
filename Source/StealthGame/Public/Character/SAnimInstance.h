// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SAnimInstance.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTurnInPlaceFinished, bool, bInterrupted);

UENUM(BlueprintType)
enum class ERotationType : uint8
{
	NotRotating,
	AnimationCurve,
	Lerp
};

class AStealthGameCharacter;
class UTurnInPlaceComponent;
UCLASS()
class STEALTHGAME_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UTurnInPlaceComponent* TurnInPlaceComponent = nullptr;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite)
	ACharacter* Character;
	
	UPROPERTY(BlueprintReadWrite)
	UCharacterMovementComponent* MovementComponent;
	
};
