// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"



USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	// Upwards, backwards acc force
	UPROPERTY()
	float Throttle;

	// Left, right acc force
	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FGoKartMove LastMove;

};


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

	void SimulateMove(FGoKartMove Move);

	// Get air resistance
	FVector GetAirResistance();

	// Get air resistance
	FVector GetRollingResistance();

	void UpdateLocationFromVelocity(float DeltaTime); 
	void ApplyRotation(float DeltaTime, float SteeringThrow);

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
	void Server_SendMove(FGoKartMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();



	FVector Velocity;
	
	// Upwards, backwards acc force
	float Throttle;	

	// Left, right acc force
	float SteeringThrow;	
};
