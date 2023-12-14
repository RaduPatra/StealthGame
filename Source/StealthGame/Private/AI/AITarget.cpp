// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AITarget.h"

UTokenHandler* UTokenHandler::MAKE(AActor* Owner, const int MaxTokens)
{
	UTokenHandler* TokenHandler = NewObject<UTokenHandler>(Owner, "TokenHandler");
	TokenHandler->CurrentTokens = MaxTokens;
	return TokenHandler;
}

bool UTokenHandler::RequestTokens(AActor* Requester, const int RequestedTokens)
{
	const int RemainingTokens = CurrentTokens - RequestedTokens;
	if (RemainingTokens < 0)
	{
		return false;
	}

	CurrentTokens = RemainingTokens;
	TokensForActorsMap.Add(Requester, RequestedTokens);
	return true;
}

bool UTokenHandler::ReturnTokens(const AActor* Requester)
{
	if (TokensForActorsMap.Contains(Requester))
	{
		const int Tokens = TokensForActorsMap[Requester];
		CurrentTokens += Tokens;
		TokensForActorsMap.Remove(Requester);
		return true;
	}
	return false;
}

bool UTokenHandler::CanRequestTokens(const int Tokens) const
{
	return (CurrentTokens - Tokens) >= 0;
}
