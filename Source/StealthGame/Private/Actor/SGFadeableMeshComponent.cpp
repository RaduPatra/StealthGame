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

void USGFadeableMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent = GetOwner()->FindComponentByClass<UMeshComponent>();

	if (!ensure(MeshComponent) || !ensure(OccludedMaterial))
	{
		UE_LOG(LogTemp, Warning, TEXT("MeshComponent or OccludedMaterial do not exist."));
		return;
	}

	OriginalMaterials = MeshComponent->GetMaterials();
	if (OriginalMaterials.Num() <= 0)
	{
		return;
	}

	//check if original has required parameter. If not, use given OccludedMaterial
	const bool bCanUseOriginal = ContainsParameter(OriginalMaterials[0], OccludedParamName);
	const bool bUseOriginalMaterial = bTryUseOriginalMaterial && bCanUseOriginal;

	MaterialToFade = bUseOriginalMaterial ? OriginalMaterials[0] : OccludedMaterial;

	if (bUseOriginalMaterial && !bCanUseOriginal)
	{
		UE_LOG(LogTemp, Warning, TEXT("OriginalMaterial doesn't contain required parameter %s"),
		       *OccludedParamName.ToString());
	}

	const bool bCanOcclude = OccludedMaterial && ContainsParameter(OccludedMaterial, OccludedParamName);
	if (!bUseOriginalMaterial && !bCanOcclude)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccludedMaterial doesn't contain required parameter %s"),
		       *OccludedParamName.ToString());
	}

	
}

void USGFadeableMeshComponent::OccludeMesh()
{
	if (!MaterialToFade) return;
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
