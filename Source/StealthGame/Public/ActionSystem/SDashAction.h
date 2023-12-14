// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionSystem/SGameplayAction.h"
#include "SDashAction.generated.h"

class USGSpringArmComponent;
class UCharacterMovementComponent;
class ACharacter;
/**
 * 
 */
UCLASS()
class STEALTHGAME_API USDashAction : public USGameplayAction
{
	GENERATED_BODY()

public:
	USDashAction();
	void SetupAbility();

	virtual void ActionActivated() override;
	virtual void ActionEnded() override;
	void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);
	virtual void Initialize(FGameplayActionData& NewActionData, USGameplayActionComponent* NewOwnerComponent) override;
	float GetSectionDuration(int SectionIndex) const;
	virtual void TickAction(float DeltaTime) override;
	virtual bool CanStart(FGameplayActionData& MyActionData, USGameplayActionComponent* OwnerComp) const override;


	//todo - make a config data asset for this
	UPROPERTY(EditDefaultsOnly)
	FRotator DashRotationRate;

	UPROPERTY(EditDefaultsOnly)
	float DashLoopTime;

	UPROPERTY(EditDefaultsOnly)
	float DashTimeDilation;

	UPROPERTY(EditDefaultsOnly)
	float DashLoopSpeed;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldLerpTimeDilation;

	UPROPERTY(EditDefaultsOnly)
	float TimeDilationLerpTime;

	UPROPERTY(EditDefaultsOnly)
	FName DashLoopSectionName;

	UPROPERTY(EditDefaultsOnly)
	FName DashEndSectionName;

	UPROPERTY(EditDefaultsOnly)
	float CameraLagSpeed;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag LoopStartedEventTag;

	UPROPERTY()
	UAnimMontage* DashMontage;

	UPROPERTY(EditDefaultsOnly)
	class USGDashAnimationData* DashAnimationData;

private:
	float DashRemainingTime;
	float CurrentLoopingSeconds;
	float CurrentTimeDilation;
	bool bIsLerpingTimeDilation;
	FRotator RotationRateBeforeDash;
	float DefaultTimeDilation;
	FOnMontageEnded MontageEndedDelegate;
	bool bLoopStarted;

	void TrySetTimeDilation(float TimeDilation) const;
	float IsCurrentMontageInSection(FName SectionName) const;
	void UpdateDash(float DeltaTime);
	void StartEndingDash();


	UPROPERTY()
	ACharacter* Character;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY()
	USGSpringArmComponent* CameraBoom;
};
