// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GrayZoneGameInstance.h"
#include "Map/Tile.h"
#include "Map/Grid.h"
#include "Map/DungeonGeneratorComponent.h"
#include "GlobalStatics.generated.h"

/**
 * This store all the static functions that can be used in different situations.
 */
UCLASS()
class GRAYZONE_API UGlobalStatics : public UObject
{
	GENERATED_BODY()

    public:

    static UGrayZoneGameInstance&   GetGameInstance(const UWorld* inWorld);
    static TArray<TSharedRef<Tile>> GetBestPath(UDungeonGeneratorComponent& dungeon, TSharedRef<Tile> const startingTile, TSharedRef<Tile> const endingTile); //Using a modified A* algorithm, we get the best path from the specified starting position to the specified destination.

    //Using the specified map (keys = items to return, values = item chance to spawn) we return a random item.
    template <typename T>
    static T GetItemRandomlyUsingPercentage(TMap<T, float> itemsWithPercentageToSpawn)
    {
        auto randomNumber = FMath::RandRange(0.0f, 100.0f); //Get a random number between 0 and 100.
        auto chanceSum = 0.0f;

        auto result = TOptional<T>();

        for (auto entry : itemsWithPercentageToSpawn)
        {
            chanceSum += entry.Value;
            result = entry.Key;
            if (randomNumber <= chanceSum) break; //If the random number is inferior the current sum then we break from this loop and return that item.
        }

        if (!result.IsSet() && chanceSum != 100)
            UE_LOG(LogTemp, Fatal, TEXT("The sum percentage of items to be spawned should always be 100"));

        return result.GetValue();
    }

    inline static FIntPoint SwapFIntPoints(FIntPoint point) { return FIntPoint(point.Y, point.X); }

    private:

    static int GetTileCost(TSharedRef<Tile> const tile, UDungeonGeneratorComponent& dungeon, int startingCost);
	
};
