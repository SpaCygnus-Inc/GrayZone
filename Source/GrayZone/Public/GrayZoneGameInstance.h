// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GrayZoneGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GRAYZONE_API UGrayZoneGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

    int static const TILE_SIZE = 100;

    UGrayZoneGameInstance();

    int GetUniqueID();

private:

    int LastUsedID;
};
