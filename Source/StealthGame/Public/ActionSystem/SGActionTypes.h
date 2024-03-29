﻿#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SGActionTypes.generated.h"

// class SGTypes
// {
// public:
// 	
// };
UENUM(BlueprintType)
namespace EGameplayTagEventType
{
	/** Rather a tag was added or removed, used in callbacks */
	enum Type
	{
		/** Event only happens when tag is new or completely removed */
		NewOrRemoved,

		/** Event happens any time tag "count" changes */
		AnyCountChange
	};
}


DECLARE_MULTICAST_DELEGATE_TwoParams(FSGOnGameplayEffectTagCountChanged, const FGameplayTag, int32);
DECLARE_DELEGATE_RetVal_OneParam(FSGOnGameplayEffectTagCountChanged&, FSGRegisterGameplayTagChangeDelegate,
                                 FGameplayTag);

USTRUCT(BlueprintType)
struct STEALTHGAME_API FSGGameplayTagCountContainer
{
	GENERATED_BODY()

public:
	FSGGameplayTagCountContainer()
	{
	}

	/**
	 * Check if the count container has a gameplay tag that matches against the specified tag (expands to include parents of asset tags)
	 *
	 * @param TagToCheck	Tag to check for a match
	 *
	 * @return True if the count container has a gameplay tag that matches, false if not
	 */
	FORCEINLINE bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const
	{
		return GameplayTagCountMap.FindRef(TagToCheck) > 0;
	}

	/**
	 * Check if the count container has gameplay tags that matches against all of the specified tags (expands to include parents of asset tags)
	 *
	 * @param TagContainer			Tag container to check for a match. If empty will return true
	 *
	 * @return True if the count container matches all of the gameplay tags
	 */
	FORCEINLINE bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
	{
		// if the TagContainer count is 0 return bCountEmptyAsMatch;
		if (TagContainer.Num() == 0)
		{
			return true;
		}

		bool AllMatch = true;
		for (const FGameplayTag& Tag : TagContainer)
		{
			if (GameplayTagCountMap.FindRef(Tag) <= 0)
			{
				AllMatch = false;
				break;
			}
		}
		return AllMatch;
	}

	/**
	 * Check if the count container has gameplay tags that matches against any of the specified tags (expands to include parents of asset tags)
	 *
	 * @param TagContainer			Tag container to check for a match. If empty will return false
	 *
	 * @return True if the count container matches any of the gameplay tags
	 */
	FORCEINLINE bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
	{
		if (TagContainer.Num() == 0)
		{
			return false;
		}

		bool AnyMatch = false;
		for (const FGameplayTag& Tag : TagContainer)
		{
			if (GameplayTagCountMap.FindRef(Tag) > 0)
			{
				AnyMatch = true;
				break;
			}
		}
		return AnyMatch;
	}

	/**
	 * Determine if TagToCheck is present in this container, also checking against parent tags
	 * {"A.1"}.HasTag("A") will return True, {"A"}.HasTag("A.1") will return False
	 * If TagToCheck is not Valid it will always return False
	 *
	 * @return True if TagToCheck is in this container, false if it is not
	 */
	FORCEINLINE_DEBUGGABLE bool HasTag(const FGameplayTag& TagToCheck) const
	{
		return ExplicitTags.HasTag(TagToCheck);
	}

	/**
	 * Determine if TagToCheck is explicitly present in this container, only allowing exact matches
	 * {"A.1"}.HasTagExact("A") will return False
	 * If TagToCheck is not Valid it will always return False
	 *
	 * @return True if TagToCheck is in this container, false if it is not
	 */
	FORCEINLINE_DEBUGGABLE bool HasTagExact(const FGameplayTag& TagToCheck) const
	{
		return ExplicitTags.HasTagExact(TagToCheck);
	}

	/**
	 * Checks if this container contains ANY of the tags in the specified container, also checks against parent tags
	 * {"A.1"}.HasAny({"A","B"}) will return True, {"A"}.HasAny({"A.1","B"}) will return False
	 * If ContainerToCheck is empty/invalid it will always return False
	 *
	 * @return True if this container has ANY of the tags of in ContainerToCheck
	 */

	FORCEINLINE_DEBUGGABLE bool HasAny(const FGameplayTagContainer& ContainerToCheck) const
	{
		return ExplicitTags.HasAny(ContainerToCheck);
	}

	/**
	 * Checks if this container contains ANY of the tags in the specified container, only allowing exact matches
	 * {"A.1"}.HasAny({"A","B"}) will return False
	 * If ContainerToCheck is empty/invalid it will always return False
	 *
	 * @return True if this container has ANY of the tags of in ContainerToCheck
	 */
	FORCEINLINE_DEBUGGABLE bool HasAnyExact(const FGameplayTagContainer& ContainerToCheck) const
	{
		return ExplicitTags.HasAnyExact(ContainerToCheck);
	}

	/**
	 * Checks if this container contains ALL of the tags in the specified container, also checks against parent tags
	 * {"A.1","B.1"}.HasAll({"A","B"}) will return True, {"A","B"}.HasAll({"A.1","B.1"}) will return False
	 * If ContainerToCheck is empty/invalid it will always return True, because there were no failed checks
	 *
	 * @return True if this container has ALL of the tags of in ContainerToCheck, including if ContainerToCheck is empty
	 */
	FORCEINLINE_DEBUGGABLE bool HasAll(const FGameplayTagContainer& ContainerToCheck) const
	{
		return ExplicitTags.HasAll(ContainerToCheck);
	}

	/**
	 * Checks if this container contains ALL of the tags in the specified container, only allowing exact matches
	 * {"A.1","B.1"}.HasAll({"A","B"}) will return False
	 * If ContainerToCheck is empty/invalid it will always return True, because there were no failed checks
	 *
	 * @return True if this container has ALL of the tags of in ContainerToCheck, including if ContainerToCheck is empty
	 */
	FORCEINLINE_DEBUGGABLE bool HasAllExact(const FGameplayTagContainer& ContainerToCheck) const
	{
		return ExplicitTags.HasAllExact(ContainerToCheck);
	}

	/**
	 * Update the specified container of tags by the specified delta, potentially causing an additional or removal from the explicit tag list
	 *
	 * @param Container		Container of tags to update
	 * @param CountDelta	Delta of the tag count to apply
	 */
	FORCEINLINE void UpdateTagCount(const FGameplayTagContainer& Container, int32 CountDelta)
	{
		if (CountDelta != 0)
		{
			for (auto TagIt = Container.CreateConstIterator(); TagIt; ++TagIt)
			{
				UpdateTagMap_Internal(*TagIt, CountDelta);
			}
		}
	}

	/**
	 * Update the specified tag by the specified delta, potentially causing an additional or removal from the explicit tag list
	 *
	 * @param Tag			Tag to update
	 * @param CountDelta	Delta of the tag count to apply
	 *
	 * @return True if tag was *either* added or removed. (E.g., we had the tag and now dont. or didnt have the tag and now we do. We didn't just change the count (1 count -> 2 count would return false).
	 */
	FORCEINLINE bool UpdateTagCount(const FGameplayTag& Tag, int32 CountDelta)
	{
		if (CountDelta != 0)
		{
			return UpdateTagMap_Internal(Tag, CountDelta);
		}

		return false;
	}

	/**
	 * Set the specified tag count to a specific value
	 *
	 * @param Tag			Tag to update
	 * @param Count			New count of the tag
	 *
	 * @return True if tag was *either* added or removed. (E.g., we had the tag and now dont. or didnt have the tag and now we do. We didn't just change the count (1 count -> 2 count would return false).
	 */
	FORCEINLINE bool SetTagCount(const FGameplayTag& Tag, int32 NewCount)
	{
		int32 ExistingCount = 0;
		if (int32* Ptr = ExplicitTagCountMap.Find(Tag))
		{
			ExistingCount = *Ptr;
		}

		int32 CountDelta = NewCount - ExistingCount;
		if (CountDelta != 0)
		{
			return UpdateTagMap_Internal(Tag, CountDelta);
		}

		return false;
	}

	/**
	* return the count for a specified tag
	*
	* @param Tag			Tag to update
	*
	* @return the count of the passed in tag
	*/
	FORCEINLINE int32 GetTagCount(const FGameplayTag& Tag) const
	{
		if (const int32* Ptr = GameplayTagCountMap.Find(Tag))
		{
			return *Ptr;
		}

		return 0;
	}

	int32 Num() const
	{
		return ExplicitTags.Num();
	}

	/**
	 *	Broadcasts the AnyChange event for this tag. This is called when the stack count of the backing gameplay effect change.
	 *	It is up to the receiver of the broadcasted delegate to decide what to do with this.
	 */
	void Notify_StackCountChange(const FGameplayTag& Tag);

	/**
	 * Return delegate that can be bound to for when the specific tag's count changes to or off of zero
	 *
	 * @param Tag	Tag to get a delegate for
	 *
	 * @return Delegate for when the specified tag's count changes to or off of zero
	 */
	FSGOnGameplayEffectTagCountChanged& RegisterGameplayTagEvent(const FGameplayTag& Tag,
	                                                             EGameplayTagEventType::Type EventType =
		                                                             EGameplayTagEventType::NewOrRemoved);

	/**
	 * Return delegate that can be bound to for when the any tag's count changes to or off of zero
	 *
	 * @return Delegate for when any tag's count changes to or off of zero
	 */
	FSGOnGameplayEffectTagCountChanged& RegisterGenericGameplayEvent()
	{
		return OnAnyTagChangeDelegate;
	}

	/** Simple accessor to the explicit gameplay tag list */
	const FGameplayTagContainer& GetExplicitGameplayTags() const
	{
		return ExplicitTags;
	}

	void Reset();

private:
	struct FDelegateInfo
	{
		FSGOnGameplayEffectTagCountChanged OnNewOrRemove;
		FSGOnGameplayEffectTagCountChanged OnAnyChange;
	};

	/** Map of tag to delegate that will be fired when the count for the key tag changes to or away from zero */
	TMap<FGameplayTag, FDelegateInfo> GameplayTagEventMap;

	/** Map of tag to active count of that tag */
	UPROPERTY()
	TMap<FGameplayTag, int32> GameplayTagCountMap;

	/** Map of tag to explicit count of that tag. Cannot share with above map because it's not safe to merge explicit and generic counts */
	UPROPERTY()
	TMap<FGameplayTag, int32> ExplicitTagCountMap;

	/** Delegate fired whenever any tag's count changes to or away from zero */
	FSGOnGameplayEffectTagCountChanged OnAnyTagChangeDelegate;

	/** Container of tags that were explicitly added */
	UPROPERTY()
	FGameplayTagContainer ExplicitTags;

	/** Internal helper function to adjust the explicit tag list & corresponding maps/delegates/etc. as necessary */
	bool UpdateTagMap_Internal(const FGameplayTag& Tag, int32 CountDelta);
};


UCLASS(Abstract, BlueprintType, Blueprintable)
class UActionData : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class UDashActionData : public UActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn=true))
	float DashSpeed;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn=true))
	FVector DashTargetLocation;
};

USTRUCT(BlueprintType)
struct FGameplayActionData
{
	GENERATED_BODY()

	FGameplayActionData(AActor* MyInstigator,
	                    UActionData* MyCustomActionParams = nullptr,
	                    AActor* MyActionTarget = nullptr) : Instigator(MyInstigator),
	                                                        CustomActionParams(MyCustomActionParams),
	                                                        ActionTarget(MyActionTarget)
	{
	}

	FGameplayActionData() {  }
	
	UPROPERTY(BlueprintReadWrite)
	AActor* Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UActionData* CustomActionParams = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AActor* ActionTarget = nullptr;;
};
