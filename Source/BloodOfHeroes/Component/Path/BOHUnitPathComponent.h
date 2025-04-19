// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// BOH
#include "BOHUnitPathComponent.generated.h"

//// ForwardDeclarations
// NOH
class ABOHPathLineActor;
class ABOHPathPointActor;

/**
 * Base class for Unit Path Component. It stores current intended path for the unit and launches its movement along the
 * path.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLOODOFHEROES_API UBOHUnitPathComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	// Path actor class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABOHPathPointActor> PathPointActorClass = nullptr;
	
	// Path actor class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABOHPathLineActor> PathLineActorClass = nullptr;
	
public:
	/**
	 * Constructor. Enables tick.
	 */
	UBOHUnitPathComponent();
	
	/**
	 * Adds a point to the unit path at NewPointPosition.
	 * @param NewPoint 
	 * @param NewPointPosition 
	 */
	UFUNCTION(BlueprintCallable)
	void AddPointToPath(FVector NewPoint, int32 NewPointPosition);

	/**
	 * Adds a point to the unit path at last position.
	 * @param NewPoint 
	 */
	UFUNCTION(BlueprintCallable)
	void AppendPointToPath(FVector NewPoint);

	/**
	 * Removes point from path at PointToRemovePosition.
	 * @param PointToRemovePosition 
	 */
	UFUNCTION(BlueprintCallable)
	void RemovePointFromPath(int32 PointToRemovePosition);

	/**
	 * Removes last point from unit path.
	 */
	UFUNCTION(BlueprintCallable)
	void RemoveLastPointFromPath();

	/**
	 * Returns unit path's points.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<FVector> GetUnitPath() const { return UnitPath; };

#if WITH_EDITOR
	/**
	 * Enables/Disables debug drawing.
	 * @param bNewVisibility 
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetDebugVisible(bool bNewVisibility) { bShowDebug = bNewVisibility; };
#endif // WITH_EDITOR

protected:
	// Overriden to: Show debug on editor.
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Overriden to: Bind to owners is selected delegate.
	virtual void BeginPlay() override;
	
	// Overriden to: Unbind to owners is selected delegate.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;;
private:
#if WITH_EDITOR
	/**
	 * Draw Debug lines.
	 */
	void DrawDebug();
#endif //WITH_EDITOR

protected:
	// Current unit path to follow.
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> UnitPath;

	// Path point actor references.
	UPROPERTY(Transient)
	TArray<TObjectPtr<ABOHPathPointActor>> PathPointActors; 
	
	// Path line actor references.
	UPROPERTY(Transient)
	TArray<TObjectPtr<ABOHPathLineActor>> PathLineActors; 
	
private:
#if WITH_EDITOR
	// Show debug for path.
	bool bShowDebug = true;
#endif // WITH_EDITOR
};
