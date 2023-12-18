// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AITarget.generated.h"


UCLASS()
class UTokenHandler : public UObject
{
	GENERATED_BODY()

public:
	static UTokenHandler* MAKE(AActor* Owner, int MaxTokens);
	bool RequestTokens(AActor* Requester, int RequestedTokens);
	bool ReturnTokens(const AActor* Requester);
	bool CanRequestTokens(const int Tokens) const;

private:
	UPROPERTY()
	TMap<AActor*, int> TokensForActorsMap;

	int CurrentTokens = 0;
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAITarget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEALTHGAME_API IAITarget
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	bool RequestTokens(AActor* Requester, int RequestedTokens);
	virtual bool RequestTokens_Implementation(AActor* Requester, int RequestedTokens) { return false; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	bool ReturnTokens(AActor* Requester);
	virtual bool ReturnTokens_Implementation(AActor* Requester) { return false; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	bool CanRequestTokens(const int Tokens) const;
	virtual bool CanRequestTokens_Implementation(const int Tokens) const { return false; };
};
