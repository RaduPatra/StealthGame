// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/AITarget.h"
#include "Actor/DistractionProjectile.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Character/SGCombatInterface.h"
#include "Character/SThrower.h"
#include "Character/TurnInPlace/TurnInPlaceComponent.h"
#include "GameFramework/Character.h"
#include "StealthGameCharacter.generated.h"

class USGameplayActionComponent;
class UInputConfig;
class USGHealthComponent;
class USGSpringArmComponent;
class UCameraComponent;

struct FInputActionValue;

UCLASS(config=Game)
class AStealthGameCharacter : public ACharacter, public ISThrower, public IAITarget, public ISGCombatInterface
{
	GENERATED_BODY()

public:
	AStealthGameCharacter();

	// AI Target Interface
	virtual bool RequestTokens_Implementation(AActor* Requester, int RequestedTokens) override;
	virtual bool ReturnTokens_Implementation(AActor* Requester) override;
	virtual bool CanRequestTokens_Implementation(const int Tokens) const override;
	// End AI Target Interface

	// Combat Interface
	virtual FGameplayTag GetTeamTag_Implementation() const override;
	// End Combat Interface

	UFUNCTION(BlueprintCallable)
	void DisableMovementInput() const;

	UFUNCTION(BlueprintCallable)
	void DisableInputForTag(FGameplayTag& Tag) const;

	UFUNCTION(BlueprintCallable)
	void EnableMovementInput();

	UFUNCTION()
	void HandleHealthDepleted(AActor* Killer);

	FORCEINLINE USGSpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

#pragma region Movement
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	virtual void Jump() override;
#pragma endregion Movement

#pragma region Throw
	virtual FVector GetThrowVelocity_Implementation() const override;
	virtual USplineComponent* GetThrowSplineComponent_Implementation() override;
	virtual FVector GetThrowLocation_Implementation() const override;
#pragma endregion Throw

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_AbilityTriggered(const FGameplayTag AbilityTag, const FGameplayTag EventTag);
	void Input_AbilityCompleted(FGameplayTag AbilityTag, FGameplayTag EventTag);

	UPROPERTY(EditDefaultsOnly, Category=Input)
	UInputConfig* InputConfig;

	UPROPERTY(EditDefaultsOnly)
	int MaxTokens = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag TeamTag;

	UPROPERTY(BlueprintReadWrite)
	bool bBlockJump = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTurnInPlaceComponent* TurnInPlaceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USGHealthComponent* HealthComponent;

	UPROPERTY()
	UTokenHandler* TokenHandler;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USGSpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	USceneComponent* ThrowLocationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	USplineComponent* ThrowSplineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	FVector CameraDirOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	float ThrowLaunchSpeed = 700.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USGameplayActionComponent* ActionComponent;
	
};
