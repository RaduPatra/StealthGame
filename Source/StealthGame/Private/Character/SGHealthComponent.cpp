// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SGHealthComponent.h"

#include "ActionSystem/SGameplayActionComponent.h"
#include "Game/SNativeGameplayTags.h"


USGHealthComponent::USGHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USGHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	// GetOwner()->OnTakeAnyDamage.AddDynamic(this, &USGHealthComponent::OnTakeDamage);
}

void USGHealthComponent::SetHealth(const float Health)
{
	if (Health <= 0 && !bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health Depleeted"));
		// OnHealthDepleted.Broadcast();
	}
	else
	{
		CurrentHealth = Health;
	}
}

void USGHealthComponent::TakeDamage(const FDamageInfo& DamageInfo)
{
	const auto NewHealth = CurrentHealth - DamageInfo.BaseDamage;

	if (NewHealth <= 0 && !bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health Depleeted"));
		OnHealthDepleted.Broadcast(DamageInfo.DamageCauser);

		USGameplayActionComponent* ActionComponent = GetOwner()->FindComponentByClass<USGameplayActionComponent>();
		if (ActionComponent)
		{
			ActionComponent->AddToActiveTags(FGameplayTagContainer(TAG_ACTOR_STATE_DEAD), 1);
		}

		bIsDead = true;
	}
	else
	{
		CurrentHealth = NewHealth;
		OnTakeDamageDelegate.Broadcast(DamageInfo);
	}

	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), NewHealth);
}

void USGHealthComponent::ApplyDamageToActor(const AActor* Actor, const FDamageInfo& DamageInfo)
{
	if (USGHealthComponent* HealthComponent = Actor->FindComponentByClass<USGHealthComponent>())
	{
		HealthComponent->TakeDamage(DamageInfo);
	}
}

float USGHealthComponent::GetHealth() const
{
	return CurrentHealth;
}

void USGHealthComponent::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                      AController* Instigator, AActor* DamageCauser)
{
	const auto NewHealth = CurrentHealth - Damage;
	FDamageInfo DamageInfo;
	DamageInfo.BaseDamage = Damage;
	DamageInfo.DamageCauser = DamageCauser;
	DamageInfo.Instigator = Instigator;
	DamageInfo.DamageType = EDamageType::Default;

	if (NewHealth <= 0 && !bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health Depleeted"));
		OnHealthDepleted.Broadcast(DamageCauser);

		USGameplayActionComponent* ActionComponent = GetOwner()->FindComponentByClass<USGameplayActionComponent>();
		if (ActionComponent)
		{
			ActionComponent->AddToActiveTags(FGameplayTagContainer(TAG_ACTOR_STATE_DEAD), 1);
		}

		bIsDead = true;
	}
	else
	{
		CurrentHealth = NewHealth;
		OnTakeDamageDelegate.Broadcast(DamageInfo);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), NewHealth);
}
