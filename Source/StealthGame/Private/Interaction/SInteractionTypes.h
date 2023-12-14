#pragma once
#include "GameplayTagContainer.h"
#include "SInteractionTypes.generated.h"
class USDefaultInteractionFilter;
class USDotInteractionFilter;
class USTypeInteractionFilter;
class USInteractionSelectionFilter;

UENUM(BlueprintType)
enum class EFilterType : uint8
{
	FT_Default,
	FT_Dot,
	FT_Type
};


UCLASS(BlueprintType)
class STEALTHGAME_API USInteractionFilters : public UObject
{
	GENERATED_BODY()

public:
	static USInteractionFilters* MAKE(AActor* Owner);

	UFUNCTION(BlueprintCallable)
	USInteractionSelectionFilter* GetFilterByType(EFilterType FilterType) const;

	UFUNCTION(BlueprintCallable)
	void ChangeTypeFilterTag(FGameplayTag Tag) const;

protected:
	UPROPERTY(BlueprintReadOnly)
	USTypeInteractionFilter* TypeFilter;

	UPROPERTY(BlueprintReadOnly)
	USDotInteractionFilter* DotFilter;

	UPROPERTY(BlueprintReadOnly)
	USDefaultInteractionFilter* DefaultFilter;
	
	void Init();
};
