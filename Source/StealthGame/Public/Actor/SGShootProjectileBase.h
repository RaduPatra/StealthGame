// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/ObjectPooling/ISGPooledObject.h"
#include "GameFramework/Actor.h"
#include "SGShootProjectileBase.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;
class UParticleSystemComponent;

UCLASS(Blueprintable, BlueprintType)
class STEALTHGAME_API ASGShootProjectileBase : public AActor, public IISGPooledObject
{
public:
	virtual void PooledBeginPlay_Implementation() override;
	virtual void PooledEndPlay_Implementation() override;

private:
	GENERATED_BODY()

public:
	ASGShootProjectileBase();
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;
	virtual void LifeSpanExpired() override;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleSystem;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	UParticleSystem* ImpactParticle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Projectile)
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float GravityScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float LifeTime;
};
