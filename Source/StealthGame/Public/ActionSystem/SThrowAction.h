// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/Object.h"
#include "SThrowAction.generated.h"

/**
 * 
 */


UCLASS(Blueprintable, BlueprintType, Abstract)
class STEALTHGAME_API USThrowAction : public USGameplayAction
{
	GENERATED_BODY()

public:
	virtual bool CanStart(FGameplayActionData& MyActionData, USGameplayActionComponent* OwnerComp) const override;

	virtual void CancelAction() override;

protected:
	UFUNCTION(BlueprintCallable)
	void SetThrowVelocity();

	UFUNCTION(BlueprintCallable)
	void ThrowProjectile();

	virtual void Initialize(FGameplayActionData& NewActionData, USGameplayActionComponent* NewOwnerComponent) override;

	virtual void ActionActivated() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADistractionProjectile> ProjectileClass;

	FVector FinalThrowVelocity;
	FVector FinalThrowLocation;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* CharacterMovementComponent;
};
