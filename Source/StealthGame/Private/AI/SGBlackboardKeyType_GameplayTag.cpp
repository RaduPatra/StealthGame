// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SGBlackboardKeyType_GameplayTag.h"
#include "GameplayTagContainer.h"
const USGBlackboardKeyType_GameplayTag::FDataType USGBlackboardKeyType_GameplayTag::InvalidValue = FGameplayTagContainer::EmptyContainer;

// Copyright Epic Games, Inc. All Rights Reserved.

USGBlackboardKeyType_GameplayTag::USGBlackboardKeyType_GameplayTag(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ValueSize = sizeof(FGameplayTagContainer);
	SupportedOp = EBlackboardKeyOperation::Text;
}

FGameplayTagContainer USGBlackboardKeyType_GameplayTag::GetValue(const USGBlackboardKeyType_GameplayTag* KeyOb, const uint8* RawData)
{
	return GetValueFromMemory<FGameplayTagContainer>(RawData);
}

bool USGBlackboardKeyType_GameplayTag::SetValue(USGBlackboardKeyType_GameplayTag* KeyOb, uint8* RawData, const FGameplayTagContainer& Value)
{
	return SetValueInMemory<FGameplayTagContainer>(RawData, Value);
}

EBlackboardCompare::Type USGBlackboardKeyType_GameplayTag::CompareValues(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock,
	const UBlackboardKeyType* OtherKeyOb, const uint8* OtherMemoryBlock) const
{
	const FGameplayTagContainer MyValue = GetValue(this, MemoryBlock);
	const FGameplayTagContainer OtherValue = GetValue((USGBlackboardKeyType_GameplayTag*)OtherKeyOb, OtherMemoryBlock);

	return (MyValue.HasAll(OtherValue) ? EBlackboardCompare::Equal : EBlackboardCompare::NotEqual);
}

FString USGBlackboardKeyType_GameplayTag::DescribeValue(const UBlackboardComponent& OwnerComp, const uint8* RawData) const
{
	const FGameplayTagContainer CurrentValue = GetValue(this, RawData);
	if (CurrentValue.IsEmpty())
	{
		static const FString EmptyContainer = TEXT("Empty");
		return EmptyContainer;
	}
	return CurrentValue.ToStringSimple();
}

bool USGBlackboardKeyType_GameplayTag::TestTextOperation(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock, ETextKeyOperation::Type Op, const FString& OtherString) const
{
	const FString Value = GetValue(this, MemoryBlock).ToStringSimple();
	switch (Op)
	{
	case ETextKeyOperation::Equal:			return (Value == OtherString);
	case ETextKeyOperation::NotEqual:		return (Value != OtherString);
	case ETextKeyOperation::Contain:		return (Value.Contains(OtherString) == true);
	case ETextKeyOperation::NotContain:		return (Value.Contains(OtherString) == false);
	default: break;
	}

	return false;
}

void USGBlackboardKeyType_GameplayTag::CopyValues(UBlackboardComponent& OwnerComp, uint8* MemoryBlock, const UBlackboardKeyType* SourceKeyOb, const uint8* SourceBlock)
{
	const USGBlackboardKeyType_GameplayTag* GameplayTagSourceKeyOb = Cast<const USGBlackboardKeyType_GameplayTag>(SourceKeyOb);
	if (GameplayTagSourceKeyOb != nullptr)
	{
		SetValue(this, MemoryBlock, GetValue(GameplayTagSourceKeyOb, SourceBlock));
	}
}

void USGBlackboardKeyType_GameplayTag::InitializeMemory(UBlackboardComponent& OwnerComp, uint8* MemoryBlock)
{
	SetValue(this, MemoryBlock, InvalidValue);
}

void USGBlackboardKeyType_GameplayTag::Clear(UBlackboardComponent& OwnerComp, uint8* MemoryBlock)
{
	SetValue(this, MemoryBlock, InvalidValue);
}

bool USGBlackboardKeyType_GameplayTag::IsEmpty(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock) const
{
	const FDataType TagContainer = GetValue(this, MemoryBlock);

	return TagContainer.IsEmpty();
}