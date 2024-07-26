// Copyright Epic Games, Inc. All Rights Reserved.


#include "GrayZoneGameModeBase.h"

void AGrayZoneGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    this->DefaultPawnClass      = nullptr;
}