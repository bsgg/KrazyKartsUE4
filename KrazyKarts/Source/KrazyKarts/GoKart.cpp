// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"
#include "Components/InputComponent.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"


// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay(); 

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
	
}

void AGoKart::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGoKart, ServerState);

}


FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
		break;
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
		break;
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
		break;
	case ROLE_Authority:
		return "Authority";
		break;
	default:
		return "Error";
		break;
	}
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{ 
		FGoKartMove Move;
		Move.DeltaTime = DeltaTime;
		Move.SteeringThrow = SteeringThrow;
		Move.Throttle = Throttle;
		// TODO: Move.Time = 

		// Send move
		Server_SendMove(Move);
		SimulateMove(Move);
	}

	// Debug string to check the role of this car
	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::White, DeltaTime);	

}

void AGoKart::OnRep_ServerState()
{
	// Update transform when this trigger is send
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;

}

void AGoKart::SimulateMove(FGoKartMove Move)
{
	// Get Acceleration according to the throttle and the max driving force
	FVector force = GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	// Add air and rolling resistance
	force += GetAirResistance();
	force += GetRollingResistance();

	FVector acceleration = force / Mass;

	// Rolling resistance
	Velocity = Velocity + acceleration * Move.DeltaTime;

	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);

	UpdateLocationFromVelocity(Move.DeltaTime);
}

FVector AGoKart::GetAirResistance()
{
	// AirResistance =  -Speed^2 * DragCoefficient
	// GetSafeNormal() Direction of the Velocity
	// Velocity.SizeSquared() = We get the ^2 with this function
	return (- Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient); 
}

FVector AGoKart::GetRollingResistance()
{
	// Rolling resistance =  normal force * RollingResistanceCoefficient
	// GetSafeNormal() Direction of the Velocity
	// Velocity.SizeSquared() = We get the ^2 with this function
	float accelerationDueToGravity = -GetWorld()->GetGravityZ() / 100; // Gravity comes * by 100 instead normal 9.8
	float normalForce = Mass * accelerationDueToGravity;
	return (-Velocity.GetSafeNormal() * RollingResistanceCoefficient * normalForce);
}

void AGoKart::ApplyRotation(float DeltaTime, float SteeringThrow)
{
	float deltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity)  * DeltaTime;
	// Rotation based on an axis and a degrees in radians
	float rotationAngle = deltaLocation / MinTurningRadius * SteeringThrow;
	FQuat rotationDelta(GetActorUpVector(),rotationAngle);

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

	// Handle the binding locally and call the server from those methods. The server will update to the rest of the clients
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value)
{	
	Throttle = Value;
}
void AGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
}

// To implement Server_SendMove, unreal needs  _Implementation and _Validate
void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{

	SimulateMove(Move);

	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = Velocity;
}

// To implement Server_SendMove, unreal needs  _Implementation and _Validate
bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	// For the moment anything coming from the client is valid
	//return (FMath::Abs(Value) <= 1);
	return true; // TODO: Make better validation
}

