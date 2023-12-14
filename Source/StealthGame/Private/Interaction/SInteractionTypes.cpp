#include "Interaction/SInteractionTypes.h"

#include "Interaction/SInteractorSelectionStrategy.h"


USInteractionSelectionFilter* USInteractionFilters::GetFilterByType(const EFilterType FilterType) const
{
	switch (FilterType)
	{
	case EFilterType::FT_Default:
		return DefaultFilter;
	case EFilterType::FT_Dot:
		return DotFilter;
	case EFilterType::FT_Type:
		return TypeFilter;
	default:
		return DefaultFilter;
	}
}

void USInteractionFilters::ChangeTypeFilterTag(FGameplayTag Tag) const
{
	TypeFilter->SetFilterTag(Tag);
}

USInteractionFilters* USInteractionFilters::MAKE(AActor* Owner)
{
	USInteractionFilters* InteractionFilters = NewObject<USInteractionFilters>(Owner);
	InteractionFilters->Init();
	return InteractionFilters;
}

void USInteractionFilters::Init()
{
	DefaultFilter = NewObject<USDefaultInteractionFilter>(GetOuter());
	DotFilter = NewObject<USDotInteractionFilter>(GetOuter());
	TypeFilter = NewObject<USTypeInteractionFilter>(GetOuter());
}
