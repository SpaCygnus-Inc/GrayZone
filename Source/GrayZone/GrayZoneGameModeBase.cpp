// Copyright Epic Games, Inc. All Rights Reserved.


#include "GrayZoneGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AGrayZoneGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    this->DefaultPawnClass      = nullptr;
}

void AGrayZoneGameModeBase::StartPlay()
{
    Super::StartPlay();

    UGameplayStatics::GetPlayerController(this->GetWorld(), 0)->bShowMouseCursor = true; //We make sure to show the cursor ingame.
}