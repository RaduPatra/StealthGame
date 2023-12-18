// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/DistractionProjectile.h"
#include "Components/BoxComponent.h"
#include "Interaction/InteractionComponent.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
ADistractionProjectile::ADistractionProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("InteractionComponent");

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetSimulatePhysics(true);
}

void ADistractionProjectile::PostInitProperties()
{
	Super::PostInitProperties();

	if (!InteractionComponent) return;
	InteractionComponent->OnCanInteract.BindDynamic(this, &ADistractionProjectile::CanInteract);
}

void ADistractionProjectile::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentSleep.AddDynamic(this, &ADistractionProjectile::OnSleep);
	BoxComponent->MoveIgnoreActors.Add(GetInstigator());
}

void ADistractionProjectile::Throw(const FVector ThrowVelocity) const
{
	// BoxComponent->SetPhysicsLinearVelocity(ThrowVelocity);
	BoxComponent->AddImpulse(ThrowVelocity, NAME_None, true);
}

void ADistractionProjectile::OnSleep(UPrimitiveComponent* PrimitiveComponent, FName BoneName)
{
	bIsSleeping = true;

	const FGameplayTag InteractableType = ISInteractable::Execute_GetInteractableType(InteractionComponent);
	const FName NoiseTag = InteractableType.GetTagName();
	PrimitiveComponent->SetSimulatePhysics(false);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), NoiseLoudness, this, NoiseRange, NoiseTag);

				
}


FInteractResult ADistractionProjectile::CanInteract(USInteractor* Interactor)
{
	return bIsSleeping;
}
