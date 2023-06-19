// Copyright Epic Games, Inc. All Rights Reserved.

#include "StealthGameCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"

//////////////////////////////////////////////////////////////////////////
// AStealthGameCharacter

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
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
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
}

//////////////////////////////////////////////////////////////////////////
// Input

void AStealthGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AStealthGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AStealthGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AStealthGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AStealthGameCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AStealthGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AStealthGameCharacter::TouchStopped);
}

void AStealthGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AStealthGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
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

void AStealthGameCharacter::MakeNoiseTest()
{
	//do line trace from camera
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	if (!FollowCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("no camera"));
		return;
	}
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + GetControlRotation().Vector() * 100000;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
	if (HitResult.bBlockingHit)
	{
		DrawDebugSphere(GetWorld(),
		                HitResult.Location,
		                12,
		                12,
		                FColor::Red,
		                false,
		                2,
		                0,
		                1);

		UAISense_Hearing::ReportNoiseEvent(GetWorld(),
		                                   HitResult.Location,
		                                   1,
		                                   this,
		                                   1000);
	}
}


void AStealthGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// DisplayPredictedPath();
}

void AStealthGameCharacter::ThrowProjectile()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	if (const ADistractionProjectile* Projectile = GetWorld()->SpawnActor<ADistractionProjectile>(
		ProjectileClass, GetThrowLocation(), FRotator::ZeroRotator, SpawnParams))
	{
		Projectile->Throw(GetThrowVelocity());
	}
}

FVector AStealthGameCharacter::GetThrowVelocity() const
{
	return (GetControlRotation().Vector() + CameraDirOffset) * ThrowLaunchSpeed;
}

void AStealthGameCharacter::CleanupSpline()
{
	ThrowSplineComponent->ClearSplinePoints();
	for (const auto SplineMesh : SplineMeshComponents)
	{
		SplineMesh->DestroyComponent(); //maybe poll this in the future?
	}
	SplineMeshComponents.Empty();
}

FVector AStealthGameCharacter::GetThrowLocation() const
{
	const FRotator CameraRotator = FRotator(0, GetControlRotation().Yaw, 0);
	const FVector ThrowOffset = ThrowLocationComponent->GetRelativeLocation();
	const FVector RotatedThrowOffset = CameraRotator.RotateVector(ThrowOffset);
	return GetActorLocation() + RotatedThrowOffset;
}

void AStealthGameCharacter::PredictPath(TArray<FVector>& OutPathPositions)
{
	FHitResult OutHit;
	FVector OutLastTraceDestination;

	FVector StartPos = GetThrowLocation();
	FVector LaunchVelocity = GetThrowVelocity();
	TArray<AActor*> ActorsToIgnore{this};

	UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(
		GetWorld(),
		OutHit,
		OutPathPositions,
		OutLastTraceDestination,
		StartPos,
		LaunchVelocity,
		true,
		ProjectileRadius,
		ECC_WorldDynamic,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		1,
		15.0f,
		5.0f);

	DrawDebugSphere(GetWorld(),
	                OutHit.ImpactPoint,
	                12,
	                12,
	                FColor::Red,
	                false,
	                0,
	                0,
	                1);
}

void AStealthGameCharacter::DrawSplineMesh()
{
	for (int i = 0; i < ThrowSplineComponent->GetNumberOfSplinePoints() - 2; ++i)
	{
		USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);
		NewSplineMesh->SetStaticMesh(SplineMeshToUse);
		NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NewSplineMesh->CanCharacterStepUpOn = ECB_No;
		NewSplineMesh->SetCastShadow(false);
		NewSplineMesh->SetBoundsScale(3.f);
		// NewSplineMesh->SetMobility(EComponentMobility::Movable);

		FVector StartLocation;
		FVector StartTangent;
		FVector EndLocation;
		FVector EndTangent;
		ThrowSplineComponent->GetLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent,
		                                                         ESplineCoordinateSpace::World);
		ThrowSplineComponent->GetLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent,
		                                                         ESplineCoordinateSpace::World);

		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::Z);
		NewSplineMesh->SetStartScale(FVector2D(SplineScale, SplineScale));
		NewSplineMesh->SetEndScale(FVector2D(SplineScale, SplineScale));
		NewSplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);
		NewSplineMesh->RegisterComponent();
		SplineMeshComponents.Add(NewSplineMesh);
	}
}

void AStealthGameCharacter::DisplayPredictedPath()
{
	CleanupSpline();
	TArray<FVector> OutPathPositions;
	PredictPath(OutPathPositions);
	ThrowSplineComponent->SetSplinePoints(OutPathPositions, ESplineCoordinateSpace::World, true);
	DrawSplineMesh();
}
