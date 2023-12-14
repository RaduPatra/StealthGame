// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SInteractable.h"
#include "DistractionProjectile.generated.h"
DECLARE_DELEGATE(FInteractStrategy);

USTRUCT(BlueprintType)
struct FInteractableReaction
{
	GENERATED_BODY()

	FInteractStrategy InteractStrategy;

	FName DebugName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* PickupAnimation;
};

class USInteractor;
class UInteractionComponent;

UCLASS()
class STEALTHGAME_API ADistractionProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADistractionProjectile();

	void Throw(FVector ThrowVelocity) const;

	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintCallable)
	FInteractResult CanInteract(USInteractor* Interactor);

	FDelegateHandle OnSleepHandle;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInteractionComponent* InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoiseLoudness = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoiseRange = 1200.f;

	UFUNCTION()
	void OnSleep(UPrimitiveComponent* PrimitiveComponent, FName Name);
	virtual void BeginPlay() override;

private:
	bool bIsSleeping = false;
};
