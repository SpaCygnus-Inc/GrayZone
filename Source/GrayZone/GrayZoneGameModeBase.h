// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GrayZoneGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GRAYZONE_API AGrayZoneGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
};
