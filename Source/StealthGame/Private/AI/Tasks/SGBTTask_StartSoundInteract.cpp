// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SGBTTask_StartSoundInteract.h"

#include "AIController.h"
#include "AI/SGAIInteractableBehaviorData.h"
#include "AI/AISettings.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interaction/SInteractor.h"

USGBTTask_StartSoundInteract::USGBTTask_StartSoundInteract()
{
	bCreateNodeInstance = true;
	
	SoundKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(USGBTTask_StartSoundInteract, SoundKey));
}

EBTNodeResult::Type USGBTTask_StartSoundInteract::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                              uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	const APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;

	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	Interactor = ControlledPawn->FindComponentByClass<USInteractor>();
	const FName SoundName = BlackboardComp->GetValueAsName(SoundKey.SelectedKeyName);

	//get the stimuli SoundType assigned by the AIManager
	const FGameplayTag SoundTag = FGameplayTag::RequestGameplayTag(SoundName, true);

	if (!SoundTag.IsValid()) return EBTNodeResult::Failed;

	//update the interaction scanner to search exclusively for interactables with the tag associated with the heard sound
	Interactor->GetInteractionFilters()->ChangeTypeFilterTag(SoundTag);
	Interactor->SetInteractionFilter(EFilterType::FT_Type);
	Interactor->UpdateInteractable();

	const UAISettings* AISettings = ControlledPawn->FindComponentByClass<UAISettings>();
	if (!AISettings) return EBTNodeResult::Failed;

	const USGAIInteractableBehaviorData* BehaviorData = AISettings->GetInteractableBehaviorData();

	auto& BehaviorMap = BehaviorData->GetInteractableBehaviorMap();
	const FGameplayTag InteractableType = ISInteractable::Execute_GetInteractableType(
		Interactor->GetCurrentPreviewData().Interactable.GetObject());

	/*For some interactables you may want additional PreInteract and PostInteract behavior for example the reaction for
	 *finding a coin may be different than the reaction for finding a gun*/

	// Start a dynamic subtree representing the corresponding response (similar to a smart object)
	if (const auto BehaviorToInject = BehaviorMap.Find(InteractableType))
	{
		OwnerComp.SetDynamicSubtree(InteractBehaviorInjectionTag, *BehaviorToInject);
	}

	return EBTNodeResult::Succeeded;
}
