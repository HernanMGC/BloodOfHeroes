// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHUnitPathComponent.h"

#include "BOHPathActor.h"
#include "BOHPathLineActor.h"
#include "BOHPathPointActor.h"
#include "BloodOfHeroes/Characters/BOHCharacter.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UBOHUnitPathComponent::UBOHUnitPathComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::AddPointToPath(FVector NewPoint, int32 NewPointPosition)
{
	if (NewPointPosition < 0)
	{
		return;
	}

	UnitPath.EmplaceAt(NewPointPosition, NewPoint);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::AppendPointToPath(FVector NewPoint)
{
	// UnitPath.Add(NewPoint);
	AddPointToPath(NewPoint, UnitPath.Num());

	UWorld* World = GetWorld();
	ABOHCharacter* Owner = World ? Cast<ABOHCharacter>(GetOwner()) : nullptr;
	if (!Owner)
	{
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Owner;
	SpawnParams.Owner = Owner;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform PathPointActorTransform = FTransform::Identity;
	PathPointActorTransform.SetLocation(NewPoint);
	ABOHPathPointActor* PathPointActor = World->SpawnActorDeferred<ABOHPathPointActor>(PathPointActorClass, PathPointActorTransform, Owner, Owner);
	if (!PathPointActor)
	{
		return;
	}

	PathPointActor->FinishSpawning(PathPointActorTransform);
	PathPointActors.Add(PathPointActor);

	if (PathPointActors.Num() <= 1)
	{
		return;
	}

	FTransform PathLineActorTransform = FTransform::Identity;
	FVector PrevUnitPathPoint = UnitPath[UnitPath.Num() - 2];
	PathLineActorTransform.SetLocation(PrevUnitPathPoint);
	PathLineActorTransform.SetRotation((NewPoint - PrevUnitPathPoint).Rotation().Quaternion());
	FVector PathLineActorScale = FVector(1.f);
	PathLineActorScale.X = (NewPoint - PrevUnitPathPoint).Length() / 100.f;
	PathLineActorTransform.SetScale3D(PathLineActorScale);
	ABOHPathLineActor* PathLineActor = World->SpawnActorDeferred<ABOHPathLineActor>(PathLineActorClass, PathLineActorTransform, Owner, Owner);
	if (!PathLineActor)
	{
		return;
	}
	
	PathLineActor->FinishSpawning(PathLineActorTransform);
	PathLineActors.Add(PathLineActor);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::RemovePointFromPath(int32 PointToRemovePosition)
{
	UnitPath.RemoveAt(PointToRemovePosition);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::RemoveLastPointFromPath()
{
	RemovePointFromPath(UnitPath.Num() - 1);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_EDITOR
	DrawDebug();
#endif // WITH_EDITOR
	
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::BeginPlay()
{
	Super::BeginPlay();

	ABOHCharacter* Owner = Cast<ABOHCharacter>(GetOwner());
	if (!Owner)
	{
		return;
	}
	
#if WITH_EDITOR
	Owner->OnUnitIsSelectedChanged.AddUniqueDynamic(this, &ThisClass::UBOHUnitPathComponent::SetDebugVisible);	
#endif //WITH_EDITOR
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ABOHCharacter* Owner = Cast<ABOHCharacter>(GetOwner());
	
#if WITH_EDITOR
	if (Owner)
	{
		Owner->OnUnitIsSelectedChanged.RemoveDynamic(this, &ThisClass::UBOHUnitPathComponent::SetDebugVisible);	
	}
#endif //WITH_EDITOR
	
	Super::EndPlay(EndPlayReason);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::DrawDebug()
{
	const UWorld* World = bShowDebug && UnitPath.Num() > 0 ? GetWorld() : nullptr;
	const AActor* Owner = World ? GetOwner() : nullptr;

	if (!Owner)
	{
		return;
	}

	DrawDebugLine(World, Owner->GetActorLocation(), UnitPath[0], FColor::Cyan, false, 0.01f, 0, 2);
	DrawDebugSphere(World, UnitPath[0], 10.f, 8, FColor::Cyan, false, 0.01f, 0, 2);
	for (int32 i = 0; i < UnitPath.Num() - 1; i++)
	{
		DrawDebugLine(World, UnitPath[i], UnitPath[i+1], FColor::Cyan, false, 0.01f, 0, 2);
		DrawDebugSphere(World, UnitPath[i+1], 10.f, 8, FColor::Cyan, false, 0.01f, 0, 2);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
