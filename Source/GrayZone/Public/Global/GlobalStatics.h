// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GrayZoneGameInstance.h"
#include "Map/Tile.h"
#include "Map/Grid.h"
#include "Map/DungeonGenerator.h"
#include "GlobalStatics.generated.h"

/**
 * This store all the static functions that can be used in different situations.
 */
UCLASS()
class GRAYZONE_API UGlobalStatics : public UObject
{
	GENERATED_BODY()

    public:

    static UGrayZoneGameInstance& GetGameInstance(const UWorld* inWorld);
    static TArray<TSharedPtr<Tile>> GetBestPath(UDungeonGenerator& dungeon, TSharedPtr<Tile> const startingTile, TSharedPtr<Tile> const endingTile);

    private:

    static int GetTileCost(TSharedPtr<Tile> const tile, UDungeonGenerator& dungeon, int startingCost);
	
};
