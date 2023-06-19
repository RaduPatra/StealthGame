// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DistractionProjectile.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "StealthGameCharacter.generated.h"

UCLASS(config=Game)
class AStealthGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	USceneComponent* ThrowLocationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	USplineComponent* ThrowSplineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	FVector CameraDirOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	float ThrowLaunchSpeed = 700.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	float ProjectileRadius = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	float SplineScale = .2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ADistractionProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* SplineMeshToUse;

	TArray<class USplineMeshComponent*> SplineMeshComponents;
public:
	AStealthGameCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	UFUNCTION(BlueprintCallable)
	void MakeNoiseTest();

	UFUNCTION(BlueprintCallable)
	void DisplayPredictedPath();

	void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void ThrowProjectile();
	
	FVector GetThrowVelocity() const;
	UFUNCTION(BlueprintCallable)
	void CleanupSpline();
	FVector GetThrowLocation() const;
	void PredictPath(TArray<FVector>& OutPathPositions);
	void DrawSplineMesh();

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
