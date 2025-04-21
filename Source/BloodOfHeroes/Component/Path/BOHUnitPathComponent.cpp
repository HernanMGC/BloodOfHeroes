// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHUnitPathComponent.h"

// UnrealEngine
#include "Components/CapsuleComponent.h"

// BOH
#include "BOHPathLineActor.h"
#include "BOHPathPointActor.h"
#include "BloodOfHeroes/Characters/BOHCharacter.h"
#include "BloodOfHeroes/Utils/BOHUtils.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UBOHUnitPathComponent::UBOHUnitPathComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::AddPointToPath(const FVector& NewPoint, const int32& NewPointPosition)
{
	if (NewPointPosition < 0)
	{
		return;
	}
	FVector InNewPoint = NewPoint;
	InNewPoint.Z = FootPoint.Z;
	UnitPath.Insert(InNewPoint, NewPointPosition);
	UpdateActorsPool();
	UpdatePathActors();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::AppendPointToPath(const FVector& NewPoint)
{
	AddPointToPath(NewPoint, UnitPath.Num());
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::RemovePointFromPath(const int32& PointToRemovePosition)
{
	UnitPath.RemoveAt(PointToRemovePosition);
	UpdatePathActors();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::RemoveLastPointFromPath()
{
	RemovePointFromPath(UnitPath.Num() - 1);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::ModifyPointFromPath(const int32& PointToModifyPosition, const FVector& NewPoint)
{
	if (!UnitPath.IsValidIndex(PointToModifyPosition))
	{
		return;
	}
	
	FVector InNewPoint = NewPoint;
	InNewPoint.Z = FootPoint.Z;
	UnitPath[PointToModifyPosition] = InNewPoint;
	UpdatePathActors();
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
	UCapsuleComponent* CapsuleComponent = Owner ? Owner->GetCapsuleComponent() : nullptr;
	if (!CapsuleComponent)
	{
		return;
	}
	
#if WITH_EDITOR
	Owner->OnUnitIsSelectedChanged.AddUniqueDynamic(this, &ThisClass::UBOHUnitPathComponent::OnUnitIsSelectedChange);	
#endif //WITH_EDITOR

	FootPoint = CapsuleComponent->GetComponentLocation();
	FootPoint.Z -= CapsuleComponent->GetScaledCapsuleHalfHeight();
	AppendPointToPath(FootPoint);
	SetPathActorsVisibility(false);
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

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::OnUnitIsSelectedChange(bool bNewIsSelected)
{
#if WITH_EDITOR
	bShowDebug = bNewIsSelected;
#endif // WITH_EDITOR

	SetPathActorsVisibility(bNewIsSelected);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::SetPathActorsVisibility(const bool& bNewVisibility)
{
	int32 UnitPathLastIndex = UnitPath.Num() - 1;
	for (int32 i = 0; i < PathLineActors.Num(); i++)
	{
		if (!PathLineActors[i]) { continue; }

		bool bLineIsVisible = bNewVisibility && (UnitPathLastIndex - 1 >= i);
		PathLineActors[i]->SetActorHiddenInGame(!bLineIsVisible);
		PathLineActors[i]->SetActorEnableCollision(bLineIsVisible);
	}

	for (int32 i = 0; i < PathPointActors.Num(); i++)
	{
		if (!PathPointActors[i]) { continue; }

		bool bPointIsVisible = bNewVisibility && (UnitPathLastIndex >= i);
		PathPointActors[i]->SetActorHiddenInGame(!bPointIsVisible);
		PathPointActors[i]->SetActorEnableCollision(bPointIsVisible);
	}
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHUnitPathComponent::UpdateActorsPool()
{
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

	int32 PointsToCreate = FMath::Max(0, UnitPath.Num() - PathPointActors.Num());
	for (int32 i = 0; i < PointsToCreate; i++)
	{
		ABOHPathPointActor* PathPointActor = World->SpawnActorDeferred<ABOHPathPointActor>(PathPointActorClass, FTransform::Identity, Owner, Owner);
		if (!PathPointActor)
		{
			continue;
		}

		PathPointActor->FinishSpawning(FTransform::Identity);
		PathPointActors.Add(PathPointActor);
		PathPointActor->SetCanBeEdit(PathPointActors.Num() != 1);
		PathPointActor->SetPathPointIndex(UnitPath.Num() - 1);
	}

	int32 LinesToCreate = FMath::Max(0, UnitPath.Num() - PathLineActors.Num() - 1);
	for (int32 i = 0; i < LinesToCreate; i++)
	{
		ABOHPathLineActor* PathLineActor = World->SpawnActorDeferred<ABOHPathLineActor>(PathLineActorClass, FTransform::Identity, Owner, Owner);
		if (!PathLineActor)
		{
			continue;
		}
	
		PathLineActor->FinishSpawning(FTransform::Identity);
		PathLineActors.Add(PathLineActor);
		PathLineActor->SetCanBeEdit(true);
		PathLineActor->SetPathPointIndex(UnitPath.Num() - 1);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
///
void UBOHUnitPathComponent::UpdatePathActors()
{
	SetPathActorsVisibility(false);
	float TotalLineLength = 0.f;
	for (int32 i = 0; i < UnitPath.Num(); i++ )
	{
		FVector PathPoint = UnitPath[i];

		if (UnitPath.IsValidIndex(i))
		{
			PathPointActors[i]->SetActorLocation(PathPoint);
		}

		if (UnitPath.IsValidIndex(i) && UnitPath.IsValidIndex(i+1))
		{
			PathLineActors[i]->SetActorLocation(PathPoint);
			FVector NextPathPoint = UnitPath[i+1];
			FVector LineVector = NextPathPoint - PathPoint;
			float LineLength = LineVector.Length() * BOHUnitConstants::CentimetersToMeters;
			TotalLineLength += LineLength;
			PathLineActors[i]->SetLineLength(LineLength, TotalLineLength);
			PathLineActors[i]->SetActorRotation(LineVector.Rotation());
		}
	}
	SetPathActorsVisibility(true);
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

	DrawDebugLine(World, Owner->GetActorLocation(), UnitPath[0], FColor::Purple, false, 0.01f, 0, 2);
	DrawDebugSphere(World, UnitPath[0], 10.f, 8, FColor::Purple, false, 0.01f, 0, 2);
	for (int32 i = 0; i < UnitPath.Num() - 1; i++)
	{
		DrawDebugLine(World, UnitPath[i], UnitPath[i+1], FColor::Purple, false, 0.01f, 0, 2);
		DrawDebugSphere(World, UnitPath[i+1], 10.f, 8, FColor::Purple, false, 0.01f, 0, 2);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
