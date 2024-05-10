// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/GlobalStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Global/TPriorityQueue.h"
//#include "Algo/Reverse.h"

UGrayZoneGameInstance& UGlobalStatics::GetGameInstance(const UWorld* inWorld)
{
    auto gameInstance = Cast<UGrayZoneGameInstance>(UGameplayStatics::GetGameInstance(inWorld));

    if (gameInstance == nullptr) UE_LOG(LogTemp, Fatal, TEXT("No game instance of type UGrayZoneGameInstance was found."));

    return *gameInstance;
}

int UGlobalStatics::GetTileCost(TSharedPtr<Tile> const tile, UDungeonGeneratorComponent& dungeon, int startingCost)
{
    if (tile.Get() == nullptr) {
        UE_LOG(LogTemp, Fatal, TEXT("The specified tile should never be null."));
        return 0;
    }

    auto tileDescription = tile.Get()->GetTileDescription();
    int  finalCost       = startingCost;

    if (tileDescription & TileDescription::CORRIDOR_TILE) finalCost += 1;
    else if (tileDescription & TileDescription::ROOM_TILE) //When it's a room tile, the closer it is to the center of the room the less it costs.
    {
        auto roomReferencedByTile = dungeon.GetRoom(tile.Get()->GetTileTypeID());
        finalCost += FMath::Min(FMath::Abs(roomReferencedByTile.Get()->GetCenterPosition().X - tile.Get()->GetPosition().X), FMath::Abs(roomReferencedByTile.Get()->GetCenterPosition().Y - tile.Get()->GetPosition().Y)) * 2;
    }
    else return INT_MAX;

    if      (tileDescription & TileDescription::DOOR_TILE) finalCost += 1;
    else if (tileDescription & TileDescription::WALL_TILE) finalCost += 5;

    return finalCost;
}

TArray<TSharedPtr<Tile>> UGlobalStatics::GetBestPath(UDungeonGeneratorComponent& dungeon, TSharedPtr<Tile> const startingTile, TSharedPtr<Tile> const endingTile)
{
    auto openList  = TPriorityQueue<TSharedPtr<Tile>>();         //The tiles that we will be checking next, the tiles with lesser cost takes priority.
    auto tilesCost = TMap<TSharedPtr<Tile>, int>();              //The tiles that were already checked with the cost that they were checked with.
    auto path      = TArray<TSharedPtr<Tile>>();                 //The final path that we will return.
    auto parents   = TMap<TSharedPtr<Tile>, TSharedPtr<Tile>>(); //Tile -> parent tile.

    openList.Enqueue(startingTile, 0);  //We start by adding the starting tile with a cost of 0.
    tilesCost.Add(startingTile, 0);     //We add the starting with 0 cost.
    parents.Add(startingTile, nullptr); //We set the parent of the starting tile as null.

    while (!openList.IsEmpty())
    {
        TSharedPtr<Tile> currentTile = openList.Pop(); //We get the tile with the lowest cost and remove it from the open list.

        //If this tile is the goal, then we build the path and get out of the loop.
        if (currentTile.Get() == endingTile.Get())
        {
            auto parent = currentTile;
            while (parent != nullptr)
            {
                path.Add(parent);
                parent = parents[parent];
            }
            
            //Algo::Reverse(path);
            break;
        }
        
        //We get all the valid neighbors of the current tile.
        TArray<TSharedPtr<Tile>> neighbors = dungeon.GetGrid().Get()->GetTileNeighbors(currentTile);

        for (auto neighbor : neighbors)
        {
            if (neighbor.Get()->GetTileDescription() & TileDescription::NONE_TILE) continue; //If the tile is not used then we ignore it.
            
            //We get the cost of the neighbor tile...
            auto cost = UGlobalStatics::GetTileCost(neighbor, dungeon, tilesCost[currentTile]); 

            ///Then if it wasn't already added or the added cost was higher, we make sure to add it to the open list. 
            if (!tilesCost.Contains(neighbor) || cost < tilesCost[neighbor])
            {
                tilesCost.FindOrAdd(neighbor) = cost;
                parents.FindOrAdd(neighbor)   = currentTile;

                auto manhattanDistance = FMath::Abs(currentTile.Get()->GetPosition().X - neighbor.Get()->GetPosition().X) + FMath::Abs(currentTile.Get()->GetPosition().Y - neighbor.Get()->GetPosition().Y);
                openList.Enqueue(neighbor, cost + manhattanDistance);
            }
        }     
    }

    //if (test >= 5000)
    //{
    //    auto testtest = openList.IsEmpty();
    //    UE_LOG(LogTemp, Fatal, TEXT("No path found!"));
    //}

    return path;
}

