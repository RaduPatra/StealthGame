// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldUserWidget.generated.h"

/**
 * Source: https://github.com/tomlooman/ActionRoguelike/blob/master/Source/ActionRoguelike/Public/SWorldUserWidget.h
 */
UCLASS()
class STEALTHGAME_API UWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> ParentSizeBox;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


public:
	static void CreateAndAddWidgetToViewport(TSubclassOf<UWorldUserWidget> WidgetClass, UWorldUserWidget*& CurrentWidgetInst, AActor* WidgetAttachActor);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	FVector WorldOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	float CameraRelativeOffset;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (ExposeOnSpawn=true))
	TObjectPtr<AActor> AttachedActor;

	UFUNCTION(BlueprintImplementableEvent)
	void SetWidgetText(const FName& NewText); //Temp - Remove after implementing state change events for interaction
};
