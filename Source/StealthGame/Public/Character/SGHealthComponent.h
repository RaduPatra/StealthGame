// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SGHealthComponent.generated.h"


UENUM()
enum class EDamageType : uint8
{
	Default,
	Projectile,
	Melee,
	Stagger
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	AActor* DamageCauser = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AActor* Instigator = nullptr;;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0;

	UPROPERTY(BlueprintReadWrite)
	EDamageType DamageType = EDamageType::Default;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthDepleted, AActor*, DamageCauser);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeDamage, const FDamageInfo&, Damage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEALTHGAME_API USGHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USGHealthComponent();

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UPROPERTY(BlueprintAssignable)
	FOnHealthDepleted OnHealthDepleted;

	UPROPERTY(BlueprintAssignable)
	FOnTakeDamage OnTakeDamageDelegate;

	UFUNCTION(BlueprintCallable)
	static void ApplyDamageToActor(const AActor* Actor, const FDamageInfo& DamageInfo);
protected:
	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator,
	                  AActor* DamageCauser);
	virtual void BeginPlay() override;

	void SetHealth(const float Health);

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(const FDamageInfo& DamageInfo);

private:
	UPROPERTY(EditAnywhere)
	float MaxHealth = 100;
	float CurrentHealth = 1;
};
