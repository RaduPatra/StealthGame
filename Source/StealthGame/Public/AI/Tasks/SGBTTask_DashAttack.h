// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionSystem/SGameplayAction.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SGBTTask_DashAttack.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class STEALTHGAME_API USGBTTask_DashAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USGBTTask_DashAttack();


	// UFUNCTION()
	// void OnWallHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UFUNCTION()
	void OnDashFinished(bool bWasCancelled);
	
	UFUNCTION()
	void OnAttackFinished(bool bWasCancelled);

	UFUNCTION(BlueprintCallable)
	void FinishTask(EBTNodeResult::Type TaskResult) const;

	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere)
	float AttackRange;

	UPROPERTY(EditAnywhere)
	float WallDamage = 1111.f;
	
	UPROPERTY(EditAnywhere)
	float AttackTokens;

	UPROPERTY(EditAnywhere)
	FGameplayTag DashActionTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag AttackActionTag;

	UPROPERTY(EditAnywhere)
	FRotator AttackRotationRate;


	UFUNCTION(BlueprintImplementableEvent)
	void SetupDamageCollision();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveTaskFinished(EBTNodeResult::Type TaskResult);


	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UFUNCTION()
	void OnDashLoopStarted();

	UPROPERTY(BlueprintReadWrite)
	class USGameplayAction* DashGameplayAction;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DashLoopStartedEventTag;

private:
	
	
	UPROPERTY()
	USGameplayAction* AttackAction;

	UPROPERTY()
	AActor* TargetActor;

	class IAITarget* AITarget;

	UPROPERTY()
	AAIController* MyController;

	UPROPERTY()
	UBehaviorTreeComponent* MyOwnerComp;

	UPROPERTY()
	class UCharacterMovementComponent* CharMovement;
	
	bool bIsAtacking = false;

	FRotator OriginalRotationRate;
	
	
};
