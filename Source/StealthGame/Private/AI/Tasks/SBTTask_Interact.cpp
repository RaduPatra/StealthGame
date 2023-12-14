// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SBTTask_Interact.h"

#include "AIController.h"
#include "Interaction/SInteractor.h"

USBTTask_Interact::USBTTask_Interact()
{
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type USBTTask_Interact::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController) return EBTNodeResult::Failed;

	const APawn* MyPawn = MyController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	InteractionComponent = MyPawn->FindComponentByClass<USInteractor>();
	if (!ensure(InteractionComponent)) return EBTNodeResult::Failed;

	InteractionComponent->OnInteractFinished.AddDynamic(this, &USBTTask_Interact::InteractionFinished);
	CachedOwnerComp = &OwnerComp;
	const bool bInteractionSuccessful = InteractionComponent->StartInteract();

	return bInteractionSuccessful ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

EBTNodeResult::Type USBTTask_Interact::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!IsValid(InteractionComponent)) return EBTNodeResult::Aborted;
	InteractionComponent->OnInteractFinished.RemoveDynamic(this, &USBTTask_Interact::InteractionFinished);
	InteractionComponent->CancelInteract();
	return EBTNodeResult::Aborted;
}

void USBTTask_Interact::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                       EBTNodeResult::Type TaskResult)
{
	InteractionComponent->OnInteractFinished.RemoveDynamic(this, &USBTTask_Interact::InteractionFinished);
}

void USBTTask_Interact::InteractionFinished(bool bWasCancelled)
{
	//in the future there could be cases where ability gets cancelled, but with a success result. But for now, we assume that if it was cancelled, it was a failure
	//Eg. All main logic was executed (eg- projectile was thrown succesfully) but there is still a part of the animation left to play
	if (bWasCancelled)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
	}
	else
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
	InteractionComponent->OnInteractFinished.RemoveDynamic(this, &USBTTask_Interact::InteractionFinished);
}
