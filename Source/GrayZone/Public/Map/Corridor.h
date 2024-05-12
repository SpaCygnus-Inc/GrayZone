// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Corridor.generated.h"

UCLASS()
class GRAYZONE_API ACorridor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACorridor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	

};
