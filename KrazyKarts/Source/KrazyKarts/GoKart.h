// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:	

	// Get air resistance
	FVector GetAirResistance();

	// Get air resistance
	FVector GetRollingResistance();

	void UpdateLocationFromVelocity(float DeltaTime); 
	void ApplyRotation(float DeltaTime);

	// The mass of the car (Kg)
	UPROPERTY(EditAnywhere)
	float Mass = 1000; 

	// The force applied to the car when the throttle is fully down (Newtons N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// The minimun radious of th ecar turning circle at full lock (m)
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	// Higher means more drag (Kg/meters)
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	// Higher means rolling resistance
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;


	void MoveForward(float Value);
	void MoveRight(float Value);


	// Make function executed on the server
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);

	FVector Velocity;

	// Each actor maintains a list of properties that can be marked for replication to clients. 
	// Whenever the value of the variable changes on the server side, the server sends the client the updated value
	UPROPERTY(replicated)
	FVector ReplicatedLocation;

	UPROPERTY(replicated)
	FRotator ReplicatedRotation;
	
	// Upwards, backwards acc force
	float Throttle;	

	// Left, right acc force
	float SteeringThrow;
	
};
