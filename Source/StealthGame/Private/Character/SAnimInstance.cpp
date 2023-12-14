// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SAnimInstance.h"

#include "Player/StealthGameCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		Character = Cast<ACharacter>(OwningActor);
		TurnInPlaceComponent = OwningActor->FindComponentByClass<UTurnInPlaceComponent>();
	}
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (TurnInPlaceComponent)
	{
		TurnInPlaceComponent->UpdateTurnInPlaceRotation(DeltaSeconds);
	}
}
