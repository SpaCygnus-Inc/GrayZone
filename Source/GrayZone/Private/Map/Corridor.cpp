// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Corridor.h"

ACorridor::ACorridor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    this->SetActorEnableCollision(false);

}

// Called when the game starts or when spawned
void ACorridor::BeginPlay()
{
	Super::BeginPlay();
	
}

