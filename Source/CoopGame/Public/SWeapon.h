// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;

//info about single hitscan weapon linetrace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	
	UPROPERTY()
	FVector_NetQuantize TraceEnd;
};
UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TSubclassOf<UDamageType> DamageType;


	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(VisibleDefaultsOnly, Category="Weapon")
	FName MuzzleSocketName;


	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<UCameraShakeBase> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float HeadshotDamageModifier;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
	float BulletSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
	float BulletSpreadInterpSpeed;

	float CurrentBulletSpread;
	void PlayFireEffects();

	void PlayerCameraShake();

	virtual void Fire();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();
	
	FTimerHandle TH_AutomaticFireTimer;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();
public:
	virtual void StartFire();

	virtual void StopFire();
};
