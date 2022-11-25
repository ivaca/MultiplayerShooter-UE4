// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"

#include "Net/UnrealNetwork.h"


USHealthComponent::USHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
	DefaultHealth = 100.0f;
}

void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
	}
	Health = DefaultHealth;
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	OnHealthChanged.Broadcast(this, Health, Health - OldHealth
	                          ,
	                          nullptr, nullptr, nullptr
	);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USHealthComponent, Health);
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                            AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f) return;

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	UE_LOG(LogTemp, Error, TEXT("Take damage: %f"), Damage);
	UE_LOG(LogTemp, Error, TEXT("Health: %f"), Health);
	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}
