// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"

// BOH
#include "BOHPlayerPawn.generated.h"

//// ForwardDeclarations
// UnrealEngine
class UCameraComponent;
class USpringArmComponent;

/**
 * Base class for camera pawn.
 */
UCLASS()
class BLOODOFHEROES_API ABOHPlayerPawn : public ADefaultPawn
{
	GENERATED_BODY()
	
public:
	/**
	 * Constructor. It initializes camera component.
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
	
private:
	// Top down camera.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent = nullptr;

	// Camera boom positioning the camera above the ground.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;
};
