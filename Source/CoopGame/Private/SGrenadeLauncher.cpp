// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeLauncher.h"

void ASGrenadeLauncher::Fire()
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner || !GrenadeProjectile) return;

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;



	AActor* Projectile = GetWorld()->SpawnActor<
		AActor>(GrenadeProjectile, MuzzleLocation, EyeRotation, SpawnParameters);
}
