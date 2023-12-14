// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayAction.h"
#include "UObject/Object.h"
#include "SPrepareThrowAction.generated.h"

/**
 * 
 */

UCLASS(Blueprintable, BlueprintType, Abstract)
class STEALTHGAME_API USPrepareThrowAction : public USGameplayAction
{
	GENERATED_BODY()

public:

	virtual bool CanStart(FGameplayActionData& MyActionData, USGameplayActionComponent* OwnerComp) const override;

	virtual void CancelAction() override;
	
	virtual void ActionActivated() override;

	virtual void ActionEnded() override;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* SplineMeshToUse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	float SplineScale = .2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Throw, meta = (AllowPrivateAccess = "true"))
	float ProjectileRadius = 10.f;

	UPROPERTY()
	TArray<class USplineMeshComponent*> SplineMeshComponents;

	UPROPERTY(BlueprintReadWrite)
	FVector FinalThrowVelocity;

	UFUNCTION(BlueprintCallable)
	void CleanupSpline();

	UFUNCTION(BlueprintCallable)
	void DisplayPredictedPath();

	void DrawSplineMesh();

	void PredictPath(TArray<FVector>& OutPathPositions);

	virtual void Initialize(FGameplayActionData& NewActionData, USGameplayActionComponent* NewOwnerComponent) override;

	UPROPERTY()
	USplineComponent* ThrowSplineComponent;
};
