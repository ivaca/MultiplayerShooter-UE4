// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "HAL/IConsoleManager.h"
#include "DrawDebugHelpers.h"

#include "Kismet/GameplayStatics.h"


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
}


void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * 10000);
	UE_LOG(LogTemp, Warning, TEXT("Vector is: %s"), *EyeRotation.Vector().ToString());

	FVector ShotDir = EyeRotation.Vector();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.bTraceComplex = true;

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility) || !HitResult.
		bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Cyan, false, 1.0f, 0, 1.0f);
		}
		if (HitResult.bBlockingHit)
		{
			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDir, HitResult, MyOwner->GetInstigatorController(),

			                                   this, DamageType);
			if (ImpactEffect)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.ImpactPoint,
				                                         HitResult.ImpactNormal.Rotation());
		}
		PlayFireEffects();
		PlayerCameraShake();
	}
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
