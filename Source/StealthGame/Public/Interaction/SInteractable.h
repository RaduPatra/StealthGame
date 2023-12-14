// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "Game/SNativeGameplayTags.h"
#include "SInteractable.generated.h"


class USGameplayAction;

USTRUCT(BlueprintType, Blueprintable)
struct FInteractionData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<USGameplayAction> ActionToGrant;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName
	InteractionText = "Interact";

	void ChangeInteractionData(const FName NewInteractionText, const TSubclassOf<USGameplayAction> NewActionToGrant);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, FName> InteractionTextByFailReason;
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// TSubclassOf<UWorldUserWidget> InteractionWidget;

	// UPROPERTY(BlueprintReadOnly)
	// TWeakObjectPtr<USGameplayAction> GrantedAction;

	// UPROPERTY(BlueprintReadOnly)
	// TWeakObjectPtr<USGameplayActionComponent> ActionComp;

	// UPROPERTY(BlueprintReadOnly)
	// UWorldUserWidget* ActiveInteractionWidget;

	// UPROPERTY(BlueprintReadOnly)
	// TMap<USInteractor*, USGameplayAction*> InteractorGrantedActions;
};

USTRUCT(BlueprintType)
struct FInteractResult
{
	GENERATED_BODY()

	FInteractResult() {}
	FInteractResult(bool bSucceeded)
	{
		bSuccess = bSucceeded;
		FailReason = TAG_ACTOR_INTERACTABLE_FAILREASON_NONE;
	}

	FInteractResult(bool bSucceeded, FGameplayTag Reason)
	{
		bSuccess = bSucceeded;
		FailReason = Reason;
	}

	bool operator== (const FInteractResult& Other) const
	{
		return bSuccess == Other.bSuccess && FailReason == Other.FailReason;
	}

	bool operator!= (const FInteractResult& Other) const
	{
		return !operator==(Other);
	}

	UPROPERTY(BlueprintReadWrite)
	bool bSuccess = false;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag FailReason = TAG_ACTOR_INTERACTABLE_FAILREASON_NONE;
	
	explicit operator bool() const
	{
		return bSuccess;
	}
};

class USInteractor;
class UInteractionComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USInteractable : public UInterface
{
	GENERATED_BODY()
};


/**
 * 
 */

class STEALTHGAME_API ISInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FInteractResult TryStartInteract(USInteractor* Interactor);
	virtual FInteractResult TryStartInteract_Implementation(USInteractor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FInteractResult CanInteract(USInteractor* Interactor) const;

	virtual FInteractResult CanInteract_Implementation(USInteractor* Interactor) const
	{
		return FInteractResult(false);
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void PreviewInteraction(USInteractor* Interactor);

	virtual void PreviewInteraction_Implementation(USInteractor* Interactor)
	{
	}


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void StopPreviewInteraction(USInteractor* Interactor);

	virtual void StopPreviewInteraction_Implementation(USInteractor* Interactor)
	{
	}


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void StopInteract(bool bCancelled);

	virtual void StopInteract_Implementation(bool bCancelled)
	{
	}

	virtual AActor* GetInteractableOwnerActor()
	{
		AActor* Owner = Cast<AActor>(this);

		ensureMsgf(Owner, TEXT("Interactable needs to have an owner Actor"));
		return Owner;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FGameplayTag GetInteractableType() const;

	virtual FGameplayTag GetInteractableType_Implementation() const
	{
		return FGameplayTag();
	}

	// virtual FGameplayTag GetInteractableType() const = 0;

	virtual const FInteractionData& GetInteractionData() =0;
};
