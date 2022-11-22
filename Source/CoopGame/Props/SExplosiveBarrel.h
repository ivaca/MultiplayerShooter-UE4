// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHealthComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "SExplosiveBarrel.generated.h"

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Barrel")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, Category="Barrel")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category="Barrel")
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(EditDefaultsOnly, Category="Barrel")
	UMaterial* ExplodedBarrelMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Barrel")
	float ExplosionImpulse;

	UPROPERTY(EditDefaultsOnly, Category="Barrel")
	UParticleSystem* ExplosionEffect;

	float bIsDead;
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta,
	                     const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
public:
};
