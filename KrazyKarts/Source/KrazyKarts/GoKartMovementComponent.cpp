// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKartMovementComponent.h"


// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



void UGoKartMovementComponent::SimulateMove(const FGoKartMove Move)
{
	// Get Acceleration according to the throttle and the max driving force
	FVector force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	// Add air and rolling resistance
	force += GetAirResistance();
	force += GetRollingResistance();

	FVector acceleration = force / Mass;

	// Rolling resistance
	Velocity = Velocity + acceleration * Move.DeltaTime;

	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);

	UpdateLocationFromVelocity(Move.DeltaTime);
}


FGoKartMove UGoKartMovementComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.Time = GetWorld()->TimeSeconds;

	return Move;
}


FVector UGoKartMovementComponent::GetAirResistance()
{
	// AirResistance =  -Speed^2 * DragCoefficient
	// GetSafeNormal() Direction of the Velocity
	// Velocity.SizeSquared() = We get the ^2 with this function
	return (-Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient);
}

FVector UGoKartMovementComponent::GetRollingResistance()
{
	// Rolling resistance =  normal force * RollingResistanceCoefficient
	// GetSafeNormal() Direction of the Velocity
	// Velocity.SizeSquared() = We get the ^2 with this function
	float accelerationDueToGravity = -GetWorld()->GetGravityZ() / 100; // Gravity comes * by 100 instead normal 9.8
	float normalForce = Mass * accelerationDueToGravity;
	return (-Velocity.GetSafeNormal() * RollingResistanceCoefficient * normalForce);
}

void UGoKartMovementComponent::ApplyRotation(float DeltaTime, float SteeringThrow)
{
	float deltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity)  * DeltaTime;
	// Rotation based on an axis and a degrees in radians
	float rotationAngle = deltaLocation / MinTurningRadius * SteeringThrow;
	FQuat rotationDelta(GetOwner()->GetActorUpVector(), rotationAngle);

	// Rotate the velocity according to the rotation
	Velocity = rotationDelta.RotateVector(Velocity);

	GetOwner()->AddActorWorldRotation(rotationDelta);
}


void UGoKartMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	// Move Actor
	FVector translation = Velocity * 100 * DeltaTime;
	// FHit result if collision with something
	FHitResult hit;
	GetOwner()->AddActorWorldOffset(translation, true, &hit);
	if (hit.IsValidBlockingHit()) // IsValidBlockingHit Returns true if there was a blocking hit that was not caused by starting in penetration.
	{
		// Reset velocity
		Velocity = FVector::ZeroVector;
	}
}

