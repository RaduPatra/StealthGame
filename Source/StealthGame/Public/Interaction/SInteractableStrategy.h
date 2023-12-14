// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TurnInPlace/TurnInPlaceComponent.h"
#include "Interaction/SInteractor.h"
#include "UObject/Object.h"
#include "SInteractableStrategy.generated.h"

/**
 * 
 */

class UInteractionComponent;
class USInteractor;

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class STEALTHGAME_API USInteractableStrategy : public UObject // could rename to USInteractionHandler?
{
	GENERATED_BODY()

public:
	// friend USInteractor;

	/*owner - the interactable that triggered this strategy
	instigator - the interactor(character) that started the interaction*/
	static USInteractableStrategy* MAKE(USInteractableStrategy* StrategyTemplate, UInteractionComponent* Owner,
	                                    USInteractor* InstigatorInteractor);
	virtual bool CanExecute() { return true; }

	virtual bool Execute()
	{
		BP_ExecuteTest();
		bool b = BP_ExecuteTestRet();
		return true;
	}

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ExecuteTest();

	UFUNCTION(BlueprintImplementableEvent)
	bool BP_ExecuteTestRet();

	virtual void Interrupt()
	{
	}

protected:
	virtual void Initialize(UInteractionComponent* Owner, USInteractor* InstigatorInteractor);

	UPROPERTY(BlueprintReadOnly)
	UInteractionComponent* InteractionComponent;

	UPROPERTY(BlueprintReadOnly)
	USInteractor* Interactor;
};


UCLASS()
class STEALTHGAME_API UPickupInteractableStrat : public USInteractableStrategy
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void MontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	bool SetupAnimMontage(AActor* InteractorOwner);

	virtual bool Execute() override;
	//this should prob only return void. All start conditions should be checked in CanExecute

	virtual void Interrupt() override;

	virtual void Initialize(UInteractionComponent* Owner, USInteractor* InstigatorInteractor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* PickupAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float BlendOutTime = -1.f;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY()
	ACharacter* Character;

	UPROPERTY()
	UTurnInPlaceComponent* TurnInPlaceComp;

#pragma region Debug
	virtual void BeginDestroy() override
	{
		Super::BeginDestroy();
		UE_LOG(LogTemp, Warning, TEXT("destruct BeginDestroy"));
	}

	~UPickupInteractableStrat()
	{
		UE_LOG(LogTemp, Warning, TEXT("destruct UPickupInteractableStrat"));
	}
#pragma endregion Debug

private:
	void FinishMontageInteract(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void InteractionStateChanged(TSubclassOf<USInteractableStrategy> Class, EInteractionState InteractResult);
	UFUNCTION()
	void FinishTurnInPlace();

	void Cleanup();

	FOnMontageEnded EndDelegate;
};


UCLASS()
class STEALTHGAME_API UStartAIBehaviourStrategy : public USInteractableStrategy
{
	GENERATED_BODY()

public:
	virtual bool Execute() override;
	//this should prob only return void. All start conditions should be checked in CanExecute

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="BT"))
	FGameplayTag BTInjectionTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBehaviorTree* BehaviorAsset;
};


UCLASS()
class STEALTHGAME_API UOwnerInteractableStrategy : public USInteractableStrategy
{
	GENERATED_BODY()

public:
	virtual bool Execute() override;
	virtual bool CanExecute() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponentTest;

protected:
};
