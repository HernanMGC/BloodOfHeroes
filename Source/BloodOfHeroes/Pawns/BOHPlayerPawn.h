// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/SpringArmComponent.h"

// BOH
#include "BOHPlayerPawn.generated.h"

//// ForwardDeclarations
// UnrealEngine
class UCameraComponent;
class USpringArmComponent;

/**
 * Base class for camera pawn.
 */
UCLASS(Abstract)
class BLOODOFHEROES_API ABOHPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Camera", meta = (ClampMin = 0.f, Units = "m")) 
	float MinArmLength = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Camera", meta = (ClampMin = 0.f, Units = "m")) 
	float MaxArmLength = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Camera", meta = (ClampMin = 0.f, Units = "m/s")) 
	float ZoomSpeed = 10.0f;
	
public:
	/**
	 * Constructor. Tick removed. It initializes camera component.
	 */
	ABOHPlayerPawn();
	
	/**
	 * Returns Camera component.
	 * @return 
	 */
	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	/**
	 * Returns Camera Boom.
	 * @return 
	 */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	
	/**
	 * Gets camera boom arm length.
	 */
	FORCEINLINE float GetCameraBoomArmLegth() const { return CameraBoom->TargetArmLength; };

	/**
	 * Sets camera boom arm length.
	 */
	void AddCameraBoomArmLegth(float InArmLength) const;
	
protected:
	// Pawn collision component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Camera")
	TObjectPtr<USphereComponent> CollisionComponent = nullptr;
	
	// Top down camera.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Camera")
	TObjectPtr<UCameraComponent> TopDownCameraComponent = nullptr;

	// Camera boom positioning the camera above the ground.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;
	
	/** DefaultPawn movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Movement")
	TObjectPtr<UPawnMovementComponent> MovementComponent = nullptr;
};
