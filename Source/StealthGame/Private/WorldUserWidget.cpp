// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"

void UWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();

		UE_LOG(LogTemp, Warning, TEXT("AttachedActor no longer valid, removing Health Widget."));
		return;
	}

	FVector2D ScreenPosition;

	auto CameraLocation = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->
		GetCameraLocation();

	auto DirFromCamera = (AttachedActor->GetActorLocation() - CameraLocation).GetSafeNormal();

	auto CameraOffset = FVector::CrossProduct(AttachedActor->GetActorUpVector(), DirFromCamera) * CameraRelativeOffset;
	auto Pos = AttachedActor->GetActorLocation() + WorldOffset + CameraOffset;

	bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), Pos, ScreenPosition);

	if (bIsOnScreen)
	{
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

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
