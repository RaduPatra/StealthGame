// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SThrowAction.h"

#include "Player/StealthGameCharacter.h"

bool USThrowAction::CanStart(FGameplayActionData& MyActionData, USGameplayActionComponent* OwnerComp) const
{
	return Super::CanStart(MyActionData, OwnerComp);
}

void USThrowAction::CancelAction()
{
	Super::CancelAction();
}

void USThrowAction::ThrowProjectile()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwnerActor());
	if (!ensureAlways(OwnerCharacter)) return;

	if (!ensureAlways(GetOwnerActor()->Implements<USThrower>())) return;
	// FinalThrowLocation = ISThrower::Execute_GetThrowLocation(GetOwnerActor());

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwnerActor();
	SpawnParams.Instigator = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (const ADistractionProjectile* Projectile = GetWorld()->SpawnActor<ADistractionProjectile>(
		ProjectileClass, FinalThrowLocation, FRotator::ZeroRotator, SpawnParams))
	{
		// const FVector ThrowVelocity = GetThrowVelocity();
		const FVector ThrowVelocity = FinalThrowVelocity;
		Projectile->Throw(ThrowVelocity);
	}
}

void USThrowAction::SetThrowVelocity()
{
	FinalThrowLocation = ISThrower::Execute_GetThrowLocation(GetOwnerActor());
	FinalThrowVelocity = ISThrower::Execute_GetThrowVelocity(GetOwnerActor());
}

void USThrowAction::Initialize(FGameplayActionData& NewActionData, USGameplayActionComponent* NewOwnerComponent)
{
	Super::Initialize(NewActionData, NewOwnerComponent);

	bShouldTick = true;
	const ACharacter* Character = Cast<ACharacter>(GetOwnerActor());
	
	if (!ensureAlways(Character)) return;
	CharacterMovementComponent = Character->GetCharacterMovement();
}

void USThrowAction::ActionActivated()
{
	Super::ActionActivated();
}