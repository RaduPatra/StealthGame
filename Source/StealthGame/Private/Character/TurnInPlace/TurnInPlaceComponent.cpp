// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/TurnInPlace/TurnInPlaceComponent.h"
#include "Character/SAnimInstance.h"
#include "Character/TurnInPlace/SGTurnInPlaceAnimData.h"
#include "GameFramework/Character.h"

UTurnInPlaceComponent::UTurnInPlaceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTurnInPlaceComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	ensureMsgf(TurnAnimData, TEXT("TurnAnimData is not set in TurnInPlaceComponent"));

	if (!ensure(OwnerCharacter)) return;

	UAnimInstance* OwnerAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	MyAnimInstance = Cast<USAnimInstance>(OwnerAnimInstance);
	ensure(MyAnimInstance);
	// MyAnimInstance->OnTurnInPlaceFinished.AddDynamic(this, &UTurnInPlaceComponent::TurnInPlaceFinished);
}

void UTurnInPlaceComponent::StartTurnInPlace(FRotator TargetRotation)
{
	if (!MyAnimInstance) return;
	if (CurrentRotationState != ERotationType::NotRotating) return;

	const FRotator CharacterRotator = OwnerCharacter->GetActorRotation();

	StartYaw = CharacterRotator.Yaw;
	EndYaw = TargetRotation.Yaw;
	float DeltaYaw = EndYaw - StartYaw;
	DeltaYaw = FMath::UnwindDegrees(DeltaYaw);

	//if the desired angle is 180, add 1 to the end yaw so that the character turns in the correct direction
	if (FMath::IsNearlyEqual(FMath::Abs(DeltaYaw), 180.0f))
	{
		EndYaw = FMath::UnwindDegrees(TargetRotation.Yaw + 1);
		DeltaYaw = EndYaw - StartYaw;
		DeltaYaw = FMath::UnwindDegrees(DeltaYaw);
	}

	DesiredYaw = DeltaYaw;

	//set rotation type based on desired angle
	if (FMath::Abs(DesiredYaw) < StartAnimThreshold)
	{
		CurrentRotationState = ERotationType::Lerp;
	}
	else
	{
		CurrentRotationState = ERotationType::AnimationCurve;
	}
}

void UTurnInPlaceComponent::InterruptTurnInPlace()
{
	FinishTurn(true);
}

void UTurnInPlaceComponent::UpdateTurnInPlaceRotation(float DeltaSeconds)
{
	const FRotator TargetRotation = FRotator(0, EndYaw, 0);

	switch (CurrentRotationState)
	{
	case ERotationType::Lerp:
		{
			const FRotator InterpRotation = GetInterpRotation(OwnerCharacter->GetActorRotation(), TargetRotation,
			                                                  DeltaSeconds);
			OwnerCharacter->SetActorRotation(InterpRotation);
			const float CharacterYaw = OwnerCharacter->GetActorRotation().Yaw;
			const bool bNearlyEqual = FMath::IsNearlyEqual(CharacterYaw, EndYaw, .1f);
			if (bNearlyEqual)
			{
				FinishTurn(false);
			}
			break;
		}
	case ERotationType::AnimationCurve:
		{
			/*Rotate with the curve from the selected turn animation so that the character rotates at the same speed
			 *as the animation and there is not foot sliding*/
			if (!IsAnimPoseRotating()) return;
			const float TurnPercentage = MyAnimInstance->GetCurveValue(TurnAnimData->TurnPercentageCurveName);

			const FRotator StartRotation = FRotator(0, StartYaw, 0);
			const FRotator InterpRotation = GetAnimInterpRotation(StartRotation, TargetRotation, TurnPercentage);
			OwnerCharacter->SetActorRotation(InterpRotation);
			if (FMath::IsNearlyEqual(TurnPercentage, 1, .03f))
			{
				FinishTurn(false);
			}
			break;
		}

	default:
		break;;
	}
}

void UTurnInPlaceComponent::FinishTurn(bool bInterrupted)
{
	CurrentRotationState = ERotationType::NotRotating;
	OnTurnInPlaceFinished.Broadcast(bInterrupted);
}

UAnimSequence* UTurnInPlaceComponent::GetTurnAnimation() const
{
	if (!TurnAnimData) return nullptr;

	//turn right
	if (DesiredYaw > StartAnimThreshold)
	{
		if (DesiredYaw < Turn90MaxAngle)
		{
			return TurnAnimData->TurnInPlaceAnimations.TurnRight90Animation;
		}
		return TurnAnimData->TurnInPlaceAnimations.TurnRight180Animation;
	}

	//turn left
	if (DesiredYaw < StartAnimThreshold * -1)
	{
		if (DesiredYaw > Turn90MaxAngle * -1)
		{
			return TurnAnimData->TurnInPlaceAnimations.TurnLeft90Animation;
		}
		return TurnAnimData->TurnInPlaceAnimations.TurnLeft180Animation;
	}

	return nullptr;
}
