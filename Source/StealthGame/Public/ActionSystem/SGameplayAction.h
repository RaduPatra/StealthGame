// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
// #include "SGameplayActionComponent.h"
#include "ISGTagEventInterface.h"
#include "Character/SThrower.h"
#include "UObject/Object.h"
#include "SGActionTypes.h"
#include "SGameplayAction.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE(FSGOnGameplayTagEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionFinished, bool, bWasCancelled);

class ADistractionProjectile;
class UActionData;
struct FGameplayActionData;

UCLASS(Blueprintable, BlueprintType, Abstract)
class STEALTHGAME_API USGameplayAction : public UObject, public IISGTagEventInterface
{
	GENERATED_BODY()

	friend class USGameplayActionComponent;

public:
	USGameplayAction();
	UFUNCTION(BlueprintCallable)
	virtual void CancelAction();

	UFUNCTION(BlueprintCallable)
	bool TryStartAction(UPARAM(ref) FGameplayActionData& NewActionData, USGameplayActionComponent* OwnerComp);

	virtual bool CanStart(UPARAM(ref) FGameplayActionData& NewActionData, USGameplayActionComponent* OwnerComp) const;

	//Tag used to start this action
	UPROPERTY(EditAnywhere, Meta = (Categories = "Action.Trigger"))
	FGameplayTag ActionStartTag;

	//tags granted to the action component when this action gets started
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer ActivationGrantedTags;

	//active tags that are required to start this action
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer ActivationRequiredTags;

	//active tags that block this action from starting
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer ActivationBlockedTags;

	//tags that cancel other active actions with these tags when this action starts 
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer ActionCancelTags;

	UPROPERTY(BlueprintAssignable)
	FOnActionStarted OnActionStarted;

	UPROPERTY(BlueprintAssignable)
	FOnActionFinished OnActionFinished;
	bool IsRunning() const;
	virtual UWorld* GetWorld() const override;
	AActor* GetInstigator() const { return Instigator; }
	AActor* GetOwnerActor() const { return OwnerActor; }
	USGameplayActionComponent* GetOwnerComponent() const { return OwnerComponent; }

	
protected:
	static USGameplayAction* CreateAction(TSubclassOf<USGameplayAction> ActionClass, FGameplayActionData& NewActionData,
	                                      class USGameplayActionComponent* OwnerComponent);

	virtual void Initialize(UPARAM(ref) FGameplayActionData& NewActionData,
	                        class USGameplayActionComponent* NewOwnerComponent);

	//called by action itself
	UFUNCTION(BlueprintCallable)
	virtual void FinishAction();

	virtual void ActionActivated();
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveActionActivated();

	virtual void ActionEnded();
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveActionEnded();

	virtual void TickAction(float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveTickAction(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	bool ReceiveCanStart(UPARAM(ref) FGameplayActionData& NewActionData, USGameplayActionComponent* OwnerComp) const;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldTick;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldAutoRemoveOnFinish;

	UPROPERTY(BlueprintReadOnly)
	FGameplayActionData ActionData;

	virtual USGGameplayTagEventHandler* GetGameplayTagEventHandler() const override
	{
		return TagEventHandler;
	}

private:
	bool bStarted = false;
	bool bHasBlueprintCanStart = false;
	bool CanStart_Internal() const;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* Instigator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USGameplayActionComponent* OwnerComponent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* OwnerActor;

	UPROPERTY()
	USGGameplayTagEventHandler* TagEventHandler;
};
