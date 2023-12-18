// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SDashAction.h"

#include "ActionSystem/SGDashAnimationData.h"
#include "Player/SGSpringArmComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

USDashAction::USDashAction()
{
	bShouldTick = true;
}

void USDashAction::Initialize(FGameplayActionData& NewActionData, USGameplayActionComponent* NewOwnerComponent)
{
	Super::Initialize(NewActionData, NewOwnerComponent);

	Character = Cast<ACharacter>(GetOwnerActor());
	CharacterMovementComponent = Character->GetCharacterMovement();
	MontageEndedDelegate.BindUObject(this, &USDashAction::OnMontageEnded);
	AnimInstance = Character->GetMesh()->GetAnimInstance();
	CameraBoom = Character->FindComponentByClass<USGSpringArmComponent>();
}


float USDashAction::GetSectionDuration(const int SectionIndex) const
{
	float DashStart_StartTime = 0.f;
	float DashStart_EndTime = 0.f;
	DashMontage->GetSectionStartAndEndTime(SectionIndex, DashStart_StartTime, DashStart_EndTime);
	const float DashStartSectionDuration = DashStart_EndTime - DashStart_StartTime;
	return DashStartSectionDuration;
}

void USDashAction::SetupAbility()
{
	DashMontage = DashAnimationData->DashMontage;
	RotationRateBeforeDash = CharacterMovementComponent->RotationRate;
	DefaultTimeDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());

	/*if (const UDashActionData* DashParams = Cast<UDashActionData>(ActionData.CustomActionParams))
	{
		DashLoopTime = DashParams->DashSpeed;
	}*/

	DashRemainingTime = DashLoopTime;
	CurrentLoopingSeconds = 0.f;
	bIsLerpingTimeDilation = false;
	CurrentTimeDilation = DashTimeDilation;
	bLoopStarted = false;

	CharacterMovementComponent->RotationRate = DashRotationRate;

	//maybe I should listen to the action tag in player character instead of doing this here
	TrySetTimeDilation(CurrentTimeDilation);

	if (CameraBoom)
	{
		CameraBoom->EnableCameraLag(CameraLagSpeed);
	}

	CharacterMovementComponent->SetMovementMode(MOVE_Flying);

	float StartSectionDuration = GetSectionDuration(0);
	const int EndSectionIndex = DashMontage->GetSectionIndex(DashEndSectionName);
	float EndSectionDuration = GetSectionDuration(EndSectionIndex);
}

void USDashAction::ActionActivated()
{
	SetupAbility();

	Character->PlayAnimMontage(DashMontage, 1.f);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DashMontage);
	Super::ActionActivated();
}

void USDashAction::ActionEnded()
{
	if (!bShouldLerpTimeDilation)
	{
		TrySetTimeDilation(DefaultTimeDilation);
	}
	if (CameraBoom)
	{
		CameraBoom->DisableCameraLag();
	}
	CharacterMovementComponent->RotationRate = RotationRateBeforeDash;
	CharacterMovementComponent->SetMovementMode(MOVE_Walking);

	AnimInstance->Montage_Stop(DashMontage->GetDefaultBlendOutTime(), DashMontage);
	AnimInstance->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly); //save RM mode in start?

	Super::ActionEnded();
}

void USDashAction::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
	//                                  FString::Printf(TEXT("OnMontageEnded: %s"), *AnimMontage->GetName()));
	if (bInterrupted)
	{
		CancelAction();
	}
	else
	{
		FinishAction();
	}
}


void USDashAction::TickAction(float DeltaTime)
{
	if (IsCurrentMontageInSection(DashLoopSectionName))
	{
		if (!bLoopStarted)
		{
			bLoopStarted = true;
			AnimInstance->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
			Execute_SendGameplayEvent2(this, LoopStartedEventTag);
		}

		UpdateDash(DeltaTime);

		DashRemainingTime -= DeltaTime;
		if (DashRemainingTime <= 0)
		{
			StartEndingDash();
		}
	}

	if (bIsLerpingTimeDilation)
	{
		//todo -  move this to character because ability could finish before its done lerping
		const float LerpAlpha = CurrentLoopingSeconds / TimeDilationLerpTime;
		CurrentTimeDilation = FMath::Lerp(DashTimeDilation, DefaultTimeDilation, LerpAlpha);
		CurrentLoopingSeconds += DeltaTime * (1.f / CurrentTimeDilation);

		TrySetTimeDilation(CurrentTimeDilation);
		if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) >= DefaultTimeDilation)
		{
			bIsLerpingTimeDilation = false;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), DefaultTimeDilation);
		}
	}

	Super::TickAction(DeltaTime);
}

bool USDashAction::CanStart(FGameplayActionData& MyActionData, USGameplayActionComponent* OwnerComp) const
{
	return Super::CanStart(MyActionData, OwnerComp);
}

void USDashAction::TrySetTimeDilation(float TimeDilation) const
{
	if (!Character->IsPlayerControlled()) return;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);
}

float USDashAction::IsCurrentMontageInSection(const FName SectionName) const
{
	const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(Character->GetCurrentMontage());
	return CurrentSection == SectionName;
}

void USDashAction::UpdateDash(float DeltaTime)
{
	const FVector DashVelocity = Character->GetActorForwardVector() * DashLoopSpeed;
	CharacterMovementComponent->Velocity = DashVelocity;
}

void USDashAction::StartEndingDash()
{
	CharacterMovementComponent->SetMovementMode(MOVE_Walking);
	AnimInstance->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	AnimInstance->Montage_JumpToSection(DashEndSectionName, DashMontage);

	if (bShouldLerpTimeDilation)
	{
		bIsLerpingTimeDilation = true;
	}
	else
	{
		TrySetTimeDilation(DefaultTimeDilation);
	}
}
