// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHUnit.h"

// UnrealEngine
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// BOH
#include "BOH/AI/BOHAIController.h"
#include "BOH/Component/Path/BOHUnitPathComponent.h"
#include "BOH/GAS/Abilities/BOHGameplayAbility.h"
#include "BOH/GAS/Attributes/BOHUnitAttributeSet.h"
#include "BOH/GAS/Components/BOHAbilitySystemComponent.h"
#include "BOH/Tags/BOHGameplayTagCollection.h"
#include "BOH/Utils/BOHUtils.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

DEFINE_LOG_CATEGORY(LogBOHUnit);

////////////////////////////////////////////////////////////////////////////////////
// FBOHUnitInfo
////////////////////////////////////////////////////////////////////////////////////

FBOHUnitInfo::FBOHUnitInfo(): UnitID(-1), TeamID(-1), UnitType(EBOHUnitType::None), EvasionRadius(-1.f),
                              ReachRadius(-1.f)
{
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

FBOHUnitInfo::FBOHUnitInfo(int32 InUnitID, int32 InTeamID, EBOHUnitType InUnitType, float InSpeedRadius, float InEvasionRadius,
                           float InReachRadius) : UnitID(InUnitID),
                                                     TeamID(InTeamID), UnitType(InUnitType), Speed(InSpeedRadius),
                                                     EvasionRadius(InEvasionRadius), ReachRadius(InReachRadius)
{
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

bool FBOHUnitInfo::operator==(const FBOHUnitInfo& Other) const
{
	return Other.UnitID == UnitID
		&& Other.TeamID == TeamID
		&& Other.UnitType == UnitType
		&& Other.Speed == Speed
		&& Other.EvasionRadius == EvasionRadius
		&& Other.ReachRadius == ReachRadius;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

bool FBOHUnitInfo::IsValid() const
{
	return UnitID != -1
		|| TeamID != -1
		|| UnitType != EBOHUnitType::None
		|| UnitType != EBOHUnitType::MAX
		|| Speed < 0.f
		|| EvasionRadius < 0.f
		|| ReachRadius < 0.f;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

FString FBOHUnitInfo::ToString() const
{
	return FString::Printf(
		TEXT("UnitID: %d - TeamID: %d - UnitType: %s\nSpeed: %f - EvasionRadius: %f - ReachRadius: %f"), UnitID,
		TeamID, *UEnum::GetValueAsString(UnitType), Speed, EvasionRadius, ReachRadius);
}

////////////////////////////////////////////////////////////////////////////////////
// ABOHUnit
////////////////////////////////////////////////////////////////////////////////////

ABOHUnit::ABOHUnit()
{
	bReplicates = true;
	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ASC = CreateDefaultSubobject<UBOHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	UnitSet = CreateDefaultSubobject<UBOHUnitAttributeSet>(TEXT("UnitSet"));

	UnitPath = CreateDefaultSubobject<UBOHUnitPathComponent>(TEXT("UnitPathComponent"));
	
	EvasionCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EvasionCollider"));
	EvasionCollider->SetupAttachment(RootComponent);
	EvasionCollider->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());

	ReachCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ReachCollider"));
	ReachCollider->SetupAttachment(RootComponent);
	ReachCollider->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	ReachCollider->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnReachBeginOverlap);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABOHUnit, UnitInfo);
	DOREPLIFETIME(ABOHUnit, Player);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::SetIsUnitSelected(bool bNewIsSelected)
{
	bIsUnitSelected = bNewIsSelected;
	OnUnitIsSelectedChanged.Broadcast(bIsUnitSelected);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::SetUnitPath(const TArray<FVector>& InUnitPath) const
{
	if (!UnitPath)
	{
		return;
	}

	UnitPath->SetUnitPath(InUnitPath);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::BeginPlay()
{
	Super::BeginPlay();
	
	BOH_LOG(LogBOHUnit, Warning, "ABOHUnit::BeginPlay %s created owned by %s.", *GetName(), *GetNetOwner()->GetName());
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!ASC)
	{
		return;
	}

	ASC->InitAbilityActorInfo(this, this);

	ASC->GetGameplayAttributeValueChangeDelegate(UnitSet->GetSpeedAttribute()).AddUObject(
		this, &ThisClass::OnSpeedAttributeChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(UnitSet->GetEvasionRadiusAttribute()).AddUObject(
		this, &ThisClass::OnEvasionRadiusAttributeChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(UnitSet->GetReachRadiusAttribute()).AddUObject(
		this, &ThisClass::OnReachRadiusAttributeChanged);
	
	InitializeEffects();
	InitializeAbilities();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UAbilitySystemComponent* ABOHUnit::GetAbilitySystemComponent() const
{
	return ASC;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UBOHAbilitySystemComponent* ABOHUnit::GetBOHAbilitySystemComponent() const
{
	return Cast<UBOHAbilitySystemComponent>(GetAbilitySystemComponent());
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::OnSpeedAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	bool bSpeedHasChanged = MovementComponent
		                        ? OnAttributeChangeData.Attribute == UnitSet->GetSpeedAttribute() &&
		                        OnAttributeChangeData.NewValue != OnAttributeChangeData.OldValue
		                        : false;
	if (!bSpeedHasChanged)
	{
		return;
	}

	MovementComponent->MaxWalkSpeed = OnAttributeChangeData.NewValue * BOHUnitConstants::MetersToCentimeters;
	UnitInfo.Speed = OnAttributeChangeData.NewValue;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::OnEvasionRadiusAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	bool bEvasionRadiusHasChanged = EvasionCollider
		                                ? OnAttributeChangeData.Attribute == UnitSet->GetEvasionRadiusAttribute() &&
		                                OnAttributeChangeData.NewValue != OnAttributeChangeData.OldValue
		                                : false;
	if (!bEvasionRadiusHasChanged)
	{
		return;
	}

	EvasionCollider->SetCapsuleRadius(OnAttributeChangeData.NewValue);
	UnitInfo.EvasionRadius = OnAttributeChangeData.NewValue;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::OnReachRadiusAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	bool bReachRadiusHasChanged = ReachCollider
		                                 ? OnAttributeChangeData.Attribute == UnitSet->GetReachRadiusAttribute() &&
		                                 OnAttributeChangeData.NewValue != OnAttributeChangeData.OldValue
		                                 : false;
	if (!bReachRadiusHasChanged)
	{
		return;
	}

	ReachCollider->SetCapsuleRadius(OnAttributeChangeData.NewValue);
	UnitInfo.ReachRadius = OnAttributeChangeData.NewValue;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::InitializeAbilities()
{
	if (!ASC || !HasAuthority())
	{
		return;
	}

	for (const TSubclassOf<UBOHGameplayAbility>& Ability : DefaultAbilities)
	{
		FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(
			FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->GetAbilityInputID()),
			                     this));
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::InitializeEffects()
{
	if (!ASC)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(UnitInitializationEffect, 1, EffectContext);
	if (EffectSpecHandle.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(UBOHGameplayTagCollection::Get().Tag_SetByCaller_UnitAttributeSet_Speed, UnitInfo.Speed);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(UBOHGameplayTagCollection::Get().Tag_SetByCaller_UnitAttributeSet_ReachRadius, UnitInfo.ReachRadius);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(UBOHGameplayTagCollection::Get().Tag_SetByCaller_UnitAttributeSet_EvasionRadius, UnitInfo.EvasionRadius);
		FActiveGameplayEffectHandle GEHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
	// for (const TSubclassOf<UGameplayEffect>& Effect : DefaultGameplayEffects)
	// {
	// 	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(Effect, 1, EffectContext);
	// 	if (EffectSpecHandle.IsValid())
	// 	{
	// 		FActiveGameplayEffectHandle GEHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	// 	}
	// }
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHUnit::OnReachBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this)
	{
		return;
	}
	
	ABOHUnit* OtherUnit = OtherActor ? Cast<ABOHUnit>(OtherActor) : nullptr;
	UPrimitiveComponent* EvasionComponent = OtherUnit && OtherComp == OtherUnit->GetEvasionCollider()
		                                        ? OtherComp
		                                        : nullptr;
	
	UBOHAbilitySystemComponent* OtherASC = EvasionComponent ? OtherUnit->GetBOHAbilitySystemComponent() : nullptr;
	if (!OtherASC)
	{
		return;
	}

	FBOHUnitOrder StopOrder;
	StopOrder.OrderState = EUnitOrderState::Queued;
	StopOrder.OrderType = EUnitOrderType::Stop;
	StopOrder.TargetActor = nullptr;
	StopOrder.bCanBeInterrupted = false;
	StopOrder.OrderSortingPolicy = EUnitOrderSortingPolicy::InterruptCurrentQueueable;

	// ToDo Replace for attack ability and its GEffect
	OtherASC->AddLooseGameplayTag(UBOHGameplayTagCollection::Get().Tag_Character_Status_Down);
	
	ABOHAIController* ThisAIController = Cast<ABOHAIController>(GetController());
	if (ThisAIController)
	{
		
		ThisAIController->SendUnitOrder(StopOrder);
	}

	ABOHAIController* OtherAIController = Cast<ABOHAIController>(OtherUnit->GetController());
	if (OtherAIController)
	{
		OtherAIController->SendUnitOrder(StopOrder);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
