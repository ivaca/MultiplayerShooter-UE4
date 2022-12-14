// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SGrenadeLauncher.generated.h"

/**
 * 
 */ 
UCLASS()
class COOPGAME_API ASGrenadeLauncher : public ASWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AActor> GrenadeProjectile;
private:
	virtual void Fire() override;
};
