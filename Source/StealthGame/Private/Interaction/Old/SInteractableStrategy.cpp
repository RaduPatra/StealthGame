// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/SInteractableStrategy.h"

#include "AIController.h"
#include "Player/StealthGameCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractionComponent.h"
#include "Interaction/SInteractable.h"
#include "Interaction/SInteractor.h"

void USInteractableStrategy::Initialize(UInteractionComponent* Owner, USInteractor* InstigatorInteractor)
{
	InteractionComponent = Owner;
	Interactor = InstigatorInteractor;
}

USInteractableStrategy* USInteractableStrategy::MAKE(USInteractableStrategy* StrategyTemplate,
                                                     UInteractionComponent* Owner,
                                                     USInteractor* InstigatorInteractor)
{
	FName ObjectName = FName(*(StrategyTemplate->GetFName().ToString() + "_MyStrat")); //for debugging purposes
	USInteractableStrategy* NewStrategy = NewObject<USInteractableStrategy>(InstigatorInteractor,
	                                                                        StrategyTemplate->GetClass(),
	                                                                        ObjectName,
	                                                                        RF_NoFlags,
	                                                                        StrategyTemplate);
	NewStrategy->Initialize(Owner, InstigatorInteractor);
	return NewStrategy;
}

// Pickup -----------------------------------------------------------------

void UPickupInteractableStrat::MontageNotifyBegin(FName NotifyName,
                                                  const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	InteractionComponent->GetOwner()->Destroy();
}

bool UPickupInteractableStrat::SetupAnimMontage(AActor* InteractorOwner)
{
	Character = Cast<ACharacter>(InteractorOwner);
	if (!ensure(Character)) return false;

	AnimInstance = Character->GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(PickupAnimation);
	EndDelegate.BindUObject(this, &UPickupInteractableStrat::FinishMontageInteract);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, PickupAnimation);
	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UPickupInteractableStrat::MontageNotifyBegin);

	return true;
}

/*
 * We first turn in place towards the object we want to pick up, then after the turn is finished,
 * we play the pickup animation, which has a notify that destroys the object.
 * After the pickup animation is finished, we finish the interaction and do some cleanup.
 */
bool UPickupInteractableStrat::Execute()
{
	Super::Execute();

	if (!ensure(IsValid(InteractionComponent))) return false;

	const FVector TurnLocation = InteractionComponent->GetOwner()->GetActorLocation();
	const FVector InteractorLocation = Interactor->GetOwner()->GetActorLocation();
	const FRotator TurnRotation = (TurnLocation - InteractorLocation).Rotation();
	TurnInPlaceComp = Interactor->GetOwner()->FindComponentByClass<UTurnInPlaceComponent>();
	TurnInPlaceComp->StartTurnInPlace(TurnRotation);
	// TurnInPlaceComp->OnTurnInPlaceFinished.AddDynamic(this, &UPickupInteractableStrat::FinishTurnInPlace);

	return true;
}


void UPickupInteractableStrat::FinishMontageInteract(UAnimMontage* Montage, bool bInterrupted)
{
	Interactor->FinishInteract(false);
	Cleanup();
}

void UPickupInteractableStrat::InteractionStateChanged(TSubclassOf<USInteractableStrategy> Class,
                                                       EInteractionState InteractResult)
{
	switch (InteractResult)
	{
	case EInteractionState::Interrupted:
		Interrupt();
		break;
	default: ;
	}
}

void UPickupInteractableStrat::FinishTurnInPlace()
{
	if (const bool bMontageSetupSuccessful = SetupAnimMontage(Interactor->GetOwner()); !bMontageSetupSuccessful)
	{
		// Interactor->InterruptInteract();
		Cleanup();
		return;
	}
	// Interactor->OnInteractChanged.AddDynamic(this, &UPickupInteractableStrat::InteractionStateChanged);
	// TurnInPlaceComp->OnTurnInPlaceFinished.RemoveDynamic(this, &UPickupInteractableStrat::FinishTurnInPlace);
}

void UPickupInteractableStrat::Interrupt()
{
	AnimInstance->Montage_Stop(BlendOutTime, PickupAnimation);
	TurnInPlaceComp->InterruptTurnInPlace();
	Cleanup();
}

void UPickupInteractableStrat::Initialize(UInteractionComponent* Owner, USInteractor* InstigatorInteractor)
{
	Super::Initialize(Owner, InstigatorInteractor);
	if (!ensureMsgf(PickupAnimation, TEXT("Pickup animation not set for %s"), *GetName())) return;
}

void UPickupInteractableStrat::Cleanup()
{
	// Interactor->OnInteractChanged.RemoveDynamic(this, &UPickupInteractableStrat::InteractionStateChanged);
	EndDelegate.Unbind();
	// TurnInPlaceComp->OnTurnInPlaceFinished.RemoveDynamic(this, &UPickupInteractableStrat::FinishTurnInPlace);
}

//-------------------------------------------------------------------------
bool UStartAIBehaviourStrategy::Execute()
{
	const AAIController* AIController = Cast<AAIController>(Interactor->GetOwner());
	if (!ensureMsgf(AIController, TEXT("Interactor owner is not an AIController!"))) return false;
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
	if (!ensureMsgf(BTComp, TEXT("AIController does not have a valid BehaviorTreeComponent!"))) return false;

	BTComp->SetDynamicSubtree(BTInjectionTag, BehaviorAsset);

	return true;
}

//-------------------------------------------------------------------------

bool UOwnerInteractableStrategy::Execute()
{
	AActor* Owner = InteractionComponent->GetOwner();
	ISInteractable::Execute_TryStartInteract(Owner, Interactor);
	return true;
}

bool UOwnerInteractableStrategy::CanExecute()
{
	AActor* Owner = InteractionComponent->GetOwner();
	if (!Owner->Implements<USInteractable>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner of InteractionComponent does not implement USInteractable!"));
		return false;
	}

	if (!ISInteractable::Execute_CanInteract(Owner, Interactor)) return false;
	return true;
}
