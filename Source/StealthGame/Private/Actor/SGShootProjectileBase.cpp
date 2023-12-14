// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SGShootProjectileBase.h"

#include "Game/SGBlueprintFunctionLibrary.h"
#include "Game/ObjectPooling/SGObjectPoolingSubsystem.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASGShootProjectileBase::ASGShootProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(BoxComponent);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(BoxComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 555.f;
	ProjectileMovement->MaxSpeed = 555.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bRotationRemainsVertical = false;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void ASGShootProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}
void ASGShootProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	BoxComponent->OnComponentHit.AddDynamic(this, &ASGShootProjectileBase::OnHit);
}

void ASGShootProjectileBase::PooledBeginPlay_Implementation()
{
	BoxComponent->IgnoreActorWhenMoving(GetOwner(), true);
	USGBlueprintFunctionLibrary::ResetMovementComp(ProjectileMovement);
	ProjectileMovement->Activate(true);
	ParticleSystem->Activate();
	SetLifeSpan(LifeTime);
}

void ASGShootProjectileBase::PooledEndPlay_Implementation()
{
	BoxComponent->IgnoreActorWhenMoving(GetOwner(), false);
	ProjectileMovement->Deactivate();
	ParticleSystem->Deactivate();
}

void ASGShootProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticle, Hit.Location, GetActorRotation());

	USGObjectPoolingSubsystem* ObjectPoolingSubsystem = GetWorld()->GetSubsystem<USGObjectPoolingSubsystem>();
	if (ensure(ObjectPoolingSubsystem))
	{
		ObjectPoolingSubsystem->ReleaseActorToPool(this);
	}
}

void ASGShootProjectileBase::LifeSpanExpired()
{
	USGObjectPoolingSubsystem* ObjectPoolingSubsystem = GetWorld()->GetSubsystem<USGObjectPoolingSubsystem>();
	if (ensure(ObjectPoolingSubsystem))
	{
		ObjectPoolingSubsystem->ReleaseActorToPool(this);
	}
}

