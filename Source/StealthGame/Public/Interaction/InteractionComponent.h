// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractable.h"
#include "SInteractor.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


class USInteractor;
class USInteractableStrategy;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, USInteractor*, Interactor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractFinished, USInteractor*, Interactor, bool, bCancelled);

//ACTOR SPECIFIC INTERACT

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FInteractResult, FOnCanInteract, USInteractor*, Interactor);
DECLARE_DYNAMIC_DELEGATE(FOnCanInteract2);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class STEALTHGAME_API UInteractionComponent : public UActorComponent, public ISInteractable
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	UPROPERTY(BlueprintReadWrite, BlueprintAssignable)
	FOnInteract OnInteract; //implemented by actor

	UPROPERTY(BlueprintReadWrite, BlueprintAssignable)
	FOnInteractFinished OnInteractFinished; //implemented by actor

	UPROPERTY(BlueprintReadWrite)
	FOnCanInteract OnCanInteract; //implemented by actor

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FInteractionData InteractionData;

public:
	virtual FInteractResult TryStartInteract_Implementation(USInteractor* Interactor) override;
	virtual FInteractResult CanInteract_Implementation(USInteractor* Interactor) const override;

	virtual void PreviewInteraction_Implementation(USInteractor* Interactor) override;
	virtual void StopPreviewInteraction_Implementation(USInteractor* Interactor) override;

	virtual void StopInteract_Implementation(bool bCancelled) override;
	virtual AActor* GetInteractableOwnerActor() override;
	virtual const FInteractionData& GetInteractionData() override;

	UFUNCTION(BlueprintCallable)
	void ChangeInteractionData(const FInteractionData& NewData);

	virtual FGameplayTag GetInteractableType_Implementation() const override { return InteractableType; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	USInteractor* CurrentInteractor = nullptr;

	bool bCanPreviewInteraction = true;

	bool IsBeingInteractedWith() const { return CurrentInteractor != nullptr; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InteractableType;
};
