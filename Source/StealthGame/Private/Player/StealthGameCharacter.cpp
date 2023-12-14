// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/StealthGameCharacter.h"

#include "Input/SEnhancedInputComponent.h"
#include "Character/SGHealthComponent.h"
#include "Player/SGSpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Game/SNativeGameplayTags.h"
#include "Game/StealthGameGameMode.h"
#include "ActionSystem/SGameplayActionComponent.h"
#include "Components/SplineComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

//////////////////////////////////////////////////////////////////////////
// AStealthGameCharacter
namespace InputActionNames
{
	const FName MoveFB = "Move Forward / Backward";
	const FName MoveRL = "Move Right / Left";
	const FName Turn = "Turn Right / Left Mouse";
	const FName Jump = "Jump";
}


AStealthGameCharacter::AStealthGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USGSpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	ThrowLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ThrowLocationComponent"));
	ThrowLocationComponent->SetupAttachment(RootComponent);
	ThrowSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("ThrowSplineComponent"));
	ThrowSplineComponent->SetupAttachment(RootComponent);

	TurnInPlaceComponent = CreateDefaultSubobject<UTurnInPlaceComponent>(TEXT("TurnInPlace Component"));

	ActionComponent = CreateDefaultSubobject<USGameplayActionComponent>("ActionComponent");
	HealthComponent = CreateDefaultSubobject<USGHealthComponent>("HealthComponent");
}

void AStealthGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	TokenHandler = UTokenHandler::MAKE(this, MaxTokens);
	HealthComponent->OnHealthDepleted.AddDynamic(this, &ThisClass::HandleHealthDepleted);
}

void AStealthGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AStealthGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	USEnhancedInputComponent* EnhancedInputComponent = Cast<USEnhancedInputComponent>(PlayerInputComponent);

	if (!ensureAlways(EnhancedInputComponent)) return;

	EnhancedInputComponent->BindActionByTag(InputConfig, TAG_Input_Jump, ETriggerEvent::Triggered, this,
	                                        &AStealthGameCharacter::Jump);
	EnhancedInputComponent->BindActionByTag(InputConfig, TAG_Input_Move, ETriggerEvent::Triggered, this,
	                                        &AStealthGameCharacter::Input_Move);
	EnhancedInputComponent->BindActionByTag(InputConfig, TAG_Input_Look, ETriggerEvent::Triggered, this,
	                                        &AStealthGameCharacter::Input_Look);

	EnhancedInputComponent->BindAbilityInputEvents(InputConfig, this, &ThisClass::Input_AbilityTriggered,
	                                               &ThisClass::Input_AbilityCompleted);
}

//just disable all events from tag move. Make a disable all events from tag function
void AStealthGameCharacter::DisableMovementInput() const
{
	FGameplayTag InputMoveTag = TAG_Input_Move.GetTag();
	DisableInputForTag(InputMoveTag);
}

void AStealthGameCharacter::DisableInputForTag(FGameplayTag& Tag) const
{
	if (USEnhancedInputComponent* EnhancedInputComponent = Cast<USEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->UnbindAllActionEventsByTag(Tag);
	}
}

void AStealthGameCharacter::EnableMovementInput()
{
	if (USEnhancedInputComponent* EnhancedInputComponent = Cast<USEnhancedInputComponent>(InputComponent))
	{
		//we check if the tag is bound to the event because we don't want to bind it twice
		if (!EnhancedInputComponent->IsTagBoundToEvent(TAG_Input_Move, ETriggerEvent::Triggered))
		{
			EnhancedInputComponent->BindActionByTag(InputConfig, TAG_Input_Move, ETriggerEvent::Triggered, this,
			                                        &AStealthGameCharacter::Input_Move);
		}
	}
}

void AStealthGameCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputVector = InputActionValue.Get<FVector2D>();

	if (InputVector.X != 0.0f)
	{
		MoveRight(InputVector.X);
	}

	if (InputVector.Y != 0.0f)
	{
		MoveForward(InputVector.Y);
	}
}

void AStealthGameCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	if (Controller != nullptr)
	{
		const FVector2D LookValue = InputActionValue.Get<FVector2D>();

		if (LookValue.X != 0.0f)
		{
			TurnAtRate(LookValue.X);
		}

		if (LookValue.Y != 0.0f)
		{
			LookUpAtRate(LookValue.Y);
		}
	}
}


void AStealthGameCharacter::Input_AbilityTriggered(const FGameplayTag AbilityTag, const FGameplayTag EventTag)
{
	USGameplayAction* Action = ActionComponent->GetActionWithTag(AbilityTag);

	if (!ensure(Action))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trigger Action not found!"))
		return;
	}

	if (!Action->IsRunning())
	{
		FGameplayActionData ActionData(this);
		const auto StartedAction = ActionComponent->TryStartActionWithTag(AbilityTag, ActionData);
		FName TagName = AbilityTag.GetTagName();

		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red,
		                                 FString::Printf(
			                                 TEXT("Ability %s %s"), *TagName.ToString(),
			                                 StartedAction ? TEXT("Started") : TEXT("Failed to Start")));
	}
	else
	{
		// Action->SendGameplayEvent(EventTag);
		IISGTagEventInterface::Execute_SendGameplayEvent2(Action, EventTag);
	}
}

void AStealthGameCharacter::Input_AbilityCompleted(const FGameplayTag AbilityTag, const FGameplayTag EventTag)
{
	USGameplayAction* Action = ActionComponent->GetActionWithTag(AbilityTag);

	if (!ensure(Action))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trigger Action not found!"))
		return;
	}

	//need to check if completed is called in same frame
	if (Action->IsRunning())
	{
		IISGTagEventInterface::Execute_SendGameplayEvent2(Action, EventTag);
	}
}

void AStealthGameCharacter::Jump()
{
	if (bBlockJump) return;
	Super::Jump();
}

void AStealthGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AStealthGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AStealthGameCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AStealthGameCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

bool AStealthGameCharacter::RequestTokens_Implementation(AActor* Requester, int RequestedTokens)
{
	return TokenHandler->RequestTokens(Requester, RequestedTokens);
}

bool AStealthGameCharacter::ReturnTokens_Implementation(AActor* Requester)
{
	return TokenHandler->ReturnTokens(Requester);
}

bool AStealthGameCharacter::CanRequestTokens_Implementation(const int Tokens) const
{
	return TokenHandler->CanRequestTokens(Tokens);
}

FGameplayTag AStealthGameCharacter::GetTeamTag_Implementation() const
{
	return TeamTag;
}

void AStealthGameCharacter::HandleHealthDepleted(AActor* Killer)
{
	AStealthGameGameMode* GM = GetWorld()->GetAuthGameMode<AStealthGameGameMode>();
	if (GM)
	{
		UAIPerceptionStimuliSourceComponent* StimuliSourceComp = FindComponentByClass<
			UAIPerceptionStimuliSourceComponent>();
		StimuliSourceComp->UnregisterFromPerceptionSystem();
		GM->HandlePlayerKilled(this, Killer);
	}
}

FVector AStealthGameCharacter::GetThrowVelocity_Implementation() const
{
	return (GetControlRotation().Vector() + CameraDirOffset) * ThrowLaunchSpeed;
}

USplineComponent* AStealthGameCharacter::GetThrowSplineComponent_Implementation()
{
	return ThrowSplineComponent;
}

FVector AStealthGameCharacter::GetThrowLocation_Implementation() const
{
	// const FVector SocketLocation = GetMesh()->GetSocketLocation("hand_lSocket");
	// return SocketLocation;

	const FRotator CameraRotator = FRotator(0, GetControlRotation().Yaw, 0);
	const FVector ThrowOffset = ThrowLocationComponent->GetRelativeLocation();
	const FVector RotatedThrowOffset = CameraRotator.RotateVector(ThrowOffset);
	return GetActorLocation() + RotatedThrowOffset;
}
