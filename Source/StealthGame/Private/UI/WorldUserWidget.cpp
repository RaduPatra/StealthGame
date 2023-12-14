// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WorldUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"

void UWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();

		UE_LOG(LogTemp, Warning, TEXT("AttachedActor no longer valid, removing Widget."));
		return;
	}

	FVector2D ScreenPosition;

	const auto CameraLocation = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->
		GetCameraLocation();

	

	const auto DirFromCamera = (AttachedActor->GetActorLocation() - CameraLocation).GetSafeNormal();

	const auto CameraOffset = FVector::CrossProduct(AttachedActor->GetActorUpVector(), DirFromCamera) *
		CameraRelativeOffset;
	const auto Pos = AttachedActor->GetActorLocation() + WorldOffset + CameraOffset;

	const bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), Pos, ScreenPosition);

	if (bIsOnScreen)
	{
		const float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

		ScreenPosition /= Scale;

		if (ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}

	if (ParentSizeBox)
	{
		ParentSizeBox->SetVisibility(bIsOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UWorldUserWidget::CreateAndAddWidgetToViewport(TSubclassOf<UWorldUserWidget> WidgetClass,
                                                    UWorldUserWidget *& CurrentWidgetInst, AActor* WidgetAttachActor)
{
 	if (CurrentWidgetInst == nullptr && ensure(WidgetClass))
	{
		CurrentWidgetInst = CreateWidget<UWorldUserWidget>(WidgetAttachActor->GetWorld(), WidgetClass);
	}

	if (CurrentWidgetInst)
	{
		CurrentWidgetInst->AttachedActor = WidgetAttachActor;

		if (!CurrentWidgetInst->IsInViewport())
		{
			CurrentWidgetInst->AddToViewport();
		}
	}
}
