// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "SGBlackboardKeyType_GameplayTag.generated.h"

/**
 * 
 */

struct FGameplayTagContainer;

class UBlackboardComponent;

UCLASS(EditInlineNew, meta = (DisplayName = "Gameplay Tag"))
class STEALTHGAME_API USGBlackboardKeyType_GameplayTag : public UBlackboardKeyType
{
	GENERATED_BODY()
public:

	USGBlackboardKeyType_GameplayTag(const FObjectInitializer& ObjectInitializer);

	typedef FGameplayTagContainer FDataType;
	static const FDataType InvalidValue;

	static FGameplayTagContainer GetValue(const USGBlackboardKeyType_GameplayTag* KeyOb, const uint8* RawData);
	static bool SetValue(USGBlackboardKeyType_GameplayTag* KeyOb, uint8* RawData, const FGameplayTagContainer& Value);

	virtual EBlackboardCompare::Type CompareValues(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock,
		const UBlackboardKeyType* OtherKeyOb, const uint8* OtherMemoryBlock) const override;

protected:
	virtual FString DescribeValue(const UBlackboardComponent& OwnerComp, const uint8* RawData) const override;
	virtual bool TestTextOperation(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock, ETextKeyOperation::Type Op, const FString& OtherString) const override;
	virtual void CopyValues(UBlackboardComponent& OwnerComp, uint8* MemoryBlock, const UBlackboardKeyType* SourceKeyOb, const uint8* SourceBlock) override;
	virtual void InitializeMemory(UBlackboardComponent& OwnerComp, uint8* MemoryBlock) override;
	virtual void Clear(UBlackboardComponent& OwnerComp, uint8* MemoryBlock) override;
	virtual bool IsEmpty(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock) const override;
};



