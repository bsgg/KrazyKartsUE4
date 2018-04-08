// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"
#include "Components/InputComponent.h"


// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get Acceleration according to the throttle and the max driving force
	FVector force = GetActorForwardVector() * MaxDrivingForce * Throttle; 
	
	FVector acceleration = force / Mass;

	Velocity = Velocity + acceleration * DeltaTime;

	ApplyRotation(DeltaTime);

	UpdateLocationFromVelocity(DeltaTime);

}

void AGoKart::ApplyRotation(float DeltaTime)
{
	// Rotation based on an axis and a degrees in radians
	float rotationAngle = MaxDegreesPerSecond * DeltaTime * SteeringThrow;
	FQuat rotationDelta(GetActorUpVector(), FMath::DegreesToRadians(rotationAngle));

	// Rotate the velocity according to the rotation
	Velocity = rotationDelta.RotateVector(Velocity);

	AddActorWorldRotation(rotationDelta);
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	// Move Actor
	FVector translation = Velocity * 100 * DeltaTime;
	// FHit result if collision with something
	FHitResult hit;
	AddActorWorldOffset(translation, true, &hit);
	if (hit.IsValidBlockingHit()) // IsValidBlockingHit Returns true if there was a blocking hit that was not caused by starting in penetration.
	{
		// Reset velocity
		Velocity = FVector::ZeroVector;
	}
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value)
{
	// Add a throttle (acelerador) based on the input 
	Throttle = Value;
}

void AGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
}

