// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SGFadeableMeshComponent.generated.h"

const FName OccludedParamName = "Occluded";


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class STEALTHGAME_API USGFadeableMeshComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USGFadeableMeshComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void OccludeMesh();
	void RevertMesh();

	UPROPERTY(Transient)
	UMeshComponent* MeshComponent;

	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMat;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* OccludedMaterial;

	UPROPERTY(EditAnywhere)
	bool bTryUseOriginalMaterial = false;

	UPROPERTY(EditAnywhere, meta=(UIMin = "0", UIMax = "1"))
	float OpacityValue = 0.5f;

	UPROPERTY()
	UMaterialInterface* MaterialToFade = nullptr;

	static bool ContainsParameter(const UMaterialInterface* Material, const FName& ParamName);
};
