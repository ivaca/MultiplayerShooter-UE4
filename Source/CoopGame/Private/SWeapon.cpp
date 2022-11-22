// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "HAL/IConsoleManager.h"
#include "DrawDebugHelpers.h"
#include "CoopGame/CoopGame.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVarDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing,
                                               TEXT("Draw debug lines for weapons."), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MuzzleSocketName = "MuzzleSocket";

	BaseDamage = 20.0f;
	HeadshotDamageModifier = 2.5f;
	FireRate = 600;
	CurrentBulletSpread = 0.0f;
	BulletSpread = 1.5f;
	BulletSpreadInterpSpeed = 1.0f;
}


void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const auto HalfRad = FMath::DegreesToRadians(CurrentBulletSpread);

	const FVector HitSpread = FMath::VRandCone(EyeRotation.Vector(), HalfRad);

	FVector TraceEnd = EyeLocation + HitSpread * 10000;
	UE_LOG(LogTemp, Warning, TEXT("Vector is: %s"), *EyeRotation.Vector().ToString());

	FVector ShotDir = EyeRotation.Vector();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;


	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams) || !
		HitResult.
		bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Cyan, false, 1.0f, 0, 1.0f);
		}
		if (HitResult.bBlockingHit)
		{
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
			float ActualDamage = BaseDamage;

			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= HeadshotDamageModifier;
			}
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDir, HitResult,
			                                   MyOwner->GetInstigatorController(),

			                                   this, DamageType);


			UParticleSystem* SelectedEffect = nullptr;
			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}
			if (SelectedEffect)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, HitResult.ImpactPoint,
				                                         HitResult.ImpactNormal.Rotation());
		}
		PlayFireEffects();
		PlayerCameraShake();
	}
	CurrentBulletSpread = FMath::FInterpTo(CurrentBulletSpread, BulletSpread, GetWorld()->DeltaTimeSeconds,
	                                       BulletSpreadInterpSpeed);
	UE_LOG(LogTemp, Error, TEXT("%f"), CurrentBulletSpread);
}

void ASWeapon::StartFire()
{
	GetWorldTimerManager().SetTimer(TH_AutomaticFireTimer, this, &ASWeapon::Fire, 60.0f / FireRate, true, 0.0f);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TH_AutomaticFireTimer);
	CurrentBulletSpread = 0.0f;
}

void ASWeapon::PlayFireEffects()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}
}

void ASWeapon::PlayerCameraShake()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (!MyOwner) return;
	APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
	if (!PlayerController)return;
	PlayerController->ClientPlayCameraShake(FireCamShake);
}
