// Fill out your copyright notice in the Description page of Project Settings.


#include "DistractionProjectile.h"

// Sets default values
ADistractionProjectile::ADistractionProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->bAutoActivate = false;
	
}

// Called when the game starts or when spawned
void ADistractionProjectile::BeginPlay()
{
	Super::BeginPlay();
	// add on hit events
	
	
}

void ADistractionProjectile::Throw(const FVector ThrowVelocity) const
{
	if (MovementComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MovementComponent is null"));
		return;
	}
	MovementComponent->Velocity = ThrowVelocity;
	MovementComponent->Activate();
	
}

// Called every frame
void ADistractionProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

