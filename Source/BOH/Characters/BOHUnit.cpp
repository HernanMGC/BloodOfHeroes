// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHUnit.h"

// UnrealEngine
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// BOH
#include "BOH/GAS/Abilities/BOHGameplayAbility.h"
#include "BOH/GAS/Attributes/BOHUnitAttributeSet.h"
#include "BOH/GAS/Components/BOHAbilitySystemComponent.h"
#include "BOH/Utils/BOHUtils.h"

////////////////////////////////////////////////////////////////////////////////////
// FBOHUnitInfo
////////////////////////////////////////////////////////////////////////////////////

FBOHUnitInfo::FBOHUnitInfo(): UnitID(-1), TeamID(-1), UnitType(EBOHUnitType::None), EvasionRadius(-1.f),
                              BlockingRadius(-1.f)
{
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

FBOHUnitInfo::FBOHUnitInfo(int32 InUnitID, int32 InTeamID, EBOHUnitType InUnitType, float InEvasionRadius,
                           float InBlockingRadius) : UnitID(InUnitID),
                                                     TeamID(InTeamID), UnitType(InUnitType),
                                                     EvasionRadius(InEvasionRadius), BlockingRadius(InBlockingRadius)
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
		&& Other.BlockingRadius == BlockingRadius;
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
		|| BlockingRadius < 0.f;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

FString FBOHUnitInfo::ToString() const
{
	return FString::Printf(
		TEXT("UnitID: %d - TeamID: %d - UnitType: %s\nSpeed: %f - EvasionRadius: %f - BlockingRadius: %f"), UnitID,
		TeamID, *UEnum::GetValueAsString(UnitType), Speed, EvasionRadius, BlockingRadius);
}

////////////////////////////////////////////////////////////////////////////////////
// ABOHUnit
////////////////////////////////////////////////////////////////////////////////////

ABOHUnit::ABOHUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ASC = CreateDefaultSubobject<UBOHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	UnitSet = CreateDefaultSubobject<UBOHUnitAttributeSet>(TEXT("UnitSet"));

	EvasionCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EvasionCollider"));
	EvasionCollider->SetupAttachment(RootComponent);
	EvasionCollider->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());

	BlockingCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BlockingCollider"));
	BlockingCollider->SetupAttachment(RootComponent);
	BlockingCollider->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
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

void ABOHUnit::BeginPlay()
{
	Super::BeginPlay();

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
	ASC->GetGameplayAttributeValueChangeDelegate(UnitSet->GetBlockingRadiusAttribute()).AddUObject(
		this, &ThisClass::OnBlockingRadiusAttributeChanged);
	
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

void ABOHUnit::OnBlockingRadiusAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	bool bBlockingRadiusHasChanged = BlockingCollider
		                                 ? OnAttributeChangeData.Attribute == UnitSet->GetBlockingRadiusAttribute() &&
		                                 OnAttributeChangeData.NewValue != OnAttributeChangeData.OldValue
		                                 : false;
	if (!bBlockingRadiusHasChanged)
	{
		return;
	}

	BlockingCollider->SetCapsuleRadius(OnAttributeChangeData.NewValue);
	UnitInfo.BlockingRadius = OnAttributeChangeData.NewValue;
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

	for (const TSubclassOf<UGameplayEffect>& Effect : DefaultGameplayEffects)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(Effect, 1, EffectContext);
		if (EffectSpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
