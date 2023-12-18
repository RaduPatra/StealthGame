// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SGBTTask_DashAttack.h"

#include "AIController.h"
#include "AI/AITarget.h"
#include "Game/SGBlueprintFunctionLibrary.h"
#include "Character/SGHealthComponent.h"
#include "ActionSystem/SGameplayActionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"

USGBTTask_DashAttack::USGBTTask_DashAttack()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USGBTTask_DashAttack, TargetKey), AActor::StaticClass());
}

void USGBTTask_DashAttack::OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Hit.bBlockingHit) return;

	//temp! - this will prevent the AI from also insta-dying from bullets. todo- Need to find a better way to do this
	if (OtherComp && OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_Destructible) return;

	DashGameplayAction->CancelAction();
	APawn* MyPawn = MyController->GetPawn();
	MyPawn->TakeDamage(WallDamage, FDamageEvent(), MyController, MyPawn);

	FDamageInfo DamageInfo;
	DamageInfo.Instigator = MyPawn;
	DamageInfo.BaseDamage = WallDamage;
	DamageInfo.DamageType = EDamageType::Stagger;
	USGHealthComponent::ApplyDamageToActor(MyPawn, DamageInfo);
	FinishTask(EBTNodeResult::Failed);
}

EBTNodeResult::Type USGBTTask_DashAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MyController = OwnerComp.GetAIOwner();
	if (!MyController) return EBTNodeResult::Failed;

	APawn* ControlledPawn = MyController->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;

	MyOwnerComp = &OwnerComp;
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp != nullptr)
	{
		TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
	}
	// AITarget = Cast<IAITarget>(TargetActor);
	// if (!AITarget) return EBTNodeResult::Failed;

	// if (!IAITarget::Execute_CanRequestTokens(TargetActor, AttackTokens)) return EBTNodeResult::Failed;
	CharMovement = MyController->GetPawn()->FindComponentByClass<UCharacterMovementComponent>();
	OriginalRotationRate = CharMovement->RotationRate;

	FGameplayActionData ActionData(ControlledPawn);
	DashGameplayAction = USGBlueprintFunctionLibrary::TryStartActionWithTag(
		MyController->GetPawn(), DashActionTag, ActionData);

	if (!DashGameplayAction) return EBTNodeResult::Failed;
	MyController->SetFocus(TargetActor);

	// IAITarget::Execute_RequestTokens(TargetActor, ControlledPawn, AttackTokens);

	DashGameplayAction->OnActionFinished.AddDynamic(this, &USGBTTask_DashAttack::OnDashFinished);
	FSGOnGameplayTagEvent2 DashLoopEvent;
	DashLoopEvent.BindDynamic(this, &USGBTTask_DashAttack::OnDashLoopStarted);
	// DashGameplayAction->RegisterGameplayTagEvent(DashLoopStartedEventTag, DashLoopEvent);

	IISGTagEventInterface::Execute_RegisterGameplayTagEvent2(DashGameplayAction, DashLoopStartedEventTag,
	                                                        DashLoopEvent);

	UCapsuleComponent* CapsuleComp = ControlledPawn->FindComponentByClass<UCapsuleComponent>();
	CapsuleComp->OnComponentHit.AddDynamic(this, &USGBTTask_DashAttack::OnWallHit);

	bIsAtacking = false;

	SetupDamageCollision();

	return EBTNodeResult::InProgress;
}


void USGBTTask_DashAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const float DistanceToTarget = FVector::Distance(MyController->GetPawn()->GetActorLocation(),
	                                                 TargetActor->GetActorLocation());

	return;
	if (DistanceToTarget < AttackRange && !bIsAtacking)
	{
		DashGameplayAction->CancelAction();
		/*FGameplayActionData ActionData(MyController->GetPawn());
		AttackAction = USGBlueprintFunctionLibrary::TryStartActionWithTag(
			MyController->GetPawn(), AttackActionTag, ActionData);

		if (AttackAction)
		{
			AttackAction->OnActionFinished.AddDynamic(this, &USGBTTask_DashAttack::OnAttackFinished);
			CharMovement->RotationRate = AttackRotationRate;
			bIsAtacking = true;
		}*/

		bIsAtacking = true;
		FinishTask(EBTNodeResult::Succeeded);
	}
}

void USGBTTask_DashAttack::OnDashFinished(bool bWasCancelled)
{
	if (bWasCancelled) return;
	FinishTask(EBTNodeResult::Succeeded);
}

void USGBTTask_DashAttack::OnAttackFinished(bool bWasCancelled)
{
	FinishTask(EBTNodeResult::Succeeded);
}

void USGBTTask_DashAttack::FinishTask(EBTNodeResult::Type TaskResult) const
{
	if (AttackAction)
	{
		AttackAction->OnActionFinished.RemoveAll(this);
	}

	DashGameplayAction->OnActionFinished.RemoveAll(this);
	MyController->GetPawn()->FindComponentByClass<UCapsuleComponent>()->OnComponentHit.RemoveAll(this);

	// MyController->ClearFocus(EAIFocusPriority::Gameplay);
	// IAITarget::Execute_ReturnTokens(TargetActor, MyController->GetPawn());

	CharMovement->RotationRate = OriginalRotationRate;

	FinishLatentTask(*MyOwnerComp, TaskResult);
}

void USGBTTask_DashAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                          EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	ReceiveTaskFinished(TaskResult);
}

void USGBTTask_DashAttack::OnDashLoopStarted()
{
	MyController->ClearFocus(EAIFocusPriority::Gameplay);
}

EBTNodeResult::Type USGBTTask_DashAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FinishTask(EBTNodeResult::Aborted);
	return Super::AbortTask(OwnerComp, NodeMemory);
}
