// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SGFadeableMeshComponent.h"

USGFadeableMeshComponent::USGFadeableMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool USGFadeableMeshComponent::ContainsParameter(const UMaterialInterface* Material, const FName& ParamName)
{
	float ParamValue = -1;
	Material->GetScalarParameterValue(FHashedMaterialParameterInfo(ParamName), ParamValue);
	return ParamValue != -1;
}

// Called when the game starts
void USGFadeableMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent = GetOwner()->FindComponentByClass<UMeshComponent>();
	ensure(MeshComponent);

	OriginalMaterials = MeshComponent->GetMaterials();

	//check if original has required parameter. If not, use given OccludedMaterial
	const bool bUseOriginalMaterial = bTryUseOriginalMaterial && ContainsParameter(OriginalMaterials[0], OccludedParamName);
	if (bUseOriginalMaterial)
	{
		//todo - check and add all materials instead of just first one
		MaterialToFade = OriginalMaterials[0];
	}
	else
	{
		MaterialToFade = OccludedMaterial;
	}

	if (!bUseOriginalMaterial || !ContainsParameter(OccludedMaterial, OccludedParamName))
	{
		UE_LOG(LogTemp, Warning, TEXT("OccludedMaterial doesn't contain required parameter %s"), *OccludedParamName.ToString());
	}
}

void USGFadeableMeshComponent::OccludeMesh()
{
	if (!DynamicMat)
	{
		DynamicMat = MeshComponent->CreateDynamicMaterialInstance(0, MaterialToFade);
	}

	DynamicMat->SetScalarParameterValue(OccludedParamName, OpacityValue);
	MeshComponent->SetMaterial(0, DynamicMat);
}

void USGFadeableMeshComponent::RevertMesh()
{
	MeshComponent->SetMaterial(0, OriginalMaterials[0]);
}
