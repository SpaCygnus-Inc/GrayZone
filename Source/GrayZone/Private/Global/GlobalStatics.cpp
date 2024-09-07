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

int UGlobalStatics::GetTileCost(TSharedRef<Tile> const tile, UDungeonGeneratorComponent& dungeon, int startingCost)
{
    auto tileDescription = tile->GetTileDescription();
    int  finalCost       = startingCost;

    if (tileDescription & TileDescription::CORRIDOR_TILE) finalCost += 1;
    else if (tileDescription & TileDescription::ROOM_TILE) //When it's a room tile, the closer it is to the center of the room the less it costs.
    {
        auto roomReferencedByTile = dungeon.GetRoom(tile->GetTileTypeID());
        if (roomReferencedByTile.IsSet())
            finalCost += FMath::Min(FMath::Abs(roomReferencedByTile.GetValue()->GetCenterPosition().X - tile->GetPosition().X), FMath::Abs(roomReferencedByTile.GetValue()->GetCenterPosition().Y - tile->GetPosition().Y)) * 2;
    }
    else return INT_MAX;

    if      (tileDescription & TileDescription::DOOR_TILE) finalCost += 1;
    else if (tileDescription & TileDescription::WALL_TILE) finalCost += 5;

    return finalCost;
}

TArray<TSharedRef<Tile>> UGlobalStatics::GetBestPath(UDungeonGeneratorComponent& dungeon, TSharedRef<Tile> const startingTile, TSharedRef<Tile> const endingTile)
{
    auto openList  = TPriorityQueue<TSharedRef<Tile>>();         //The tiles that we will be checking next, the tiles with lesser cost takes priority.
    auto tilesCost = TMap<TSharedRef<Tile>, int>();              //The tiles that were already checked with the cost that they were checked with.
    auto path      = TArray<TSharedRef<Tile>>();                 //The final path that we will return.
    auto parents   = TMap<TSharedRef<Tile>, TSharedRef<Tile>>(); //Tile -> parent tile.

    tilesCost.Add(startingTile, 0);     //We add the starting with 0 cost.
    openList.Enqueue(startingTile, 0);  //We start by adding the starting tile with a cost of 0.
    parents.Add(startingTile, startingTile); //We set the parent of the starting tile as null.

    while (!openList.IsEmpty())
    {
        TSharedRef<Tile> currentTile = openList.Pop(); //We get the tile with the lowest cost and remove it from the open list.

        //If this tile is the goal, then we build the path and get out of the loop.
        if (currentTile == endingTile)
        {
            auto parent = currentTile;

            do
            {
                path.Add(parent);
                parent = parents[parent];
            }while(parent != startingTile);
            
            //Algo::Reverse(path);
            break;
        }
        
        //We get all the valid neighbors of the current tile.
        TArray<TSharedRef<Tile>> neighbors = dungeon.GetGrid()->GetTileNeighbors(currentTile);

        for (auto neighbor : neighbors)
        {
            if (neighbor->GetTileDescription() & TileDescription::NONE_TILE) continue; //If the tile is not used then we ignore it.
            
            //We get the cost of the neighbor tile...
            auto cost = UGlobalStatics::GetTileCost(neighbor, dungeon, tilesCost[currentTile]);

            ///Then if it wasn't already added or the added cost was higher, we make sure to add it to the open list. 
            if (!tilesCost.Contains(neighbor) || cost < tilesCost[neighbor])
            {
                tilesCost.FindOrAdd(neighbor, cost);
                parents.FindOrAdd(neighbor, currentTile);

                auto manhattanDistance = FMath::Abs(currentTile->GetPosition().X - neighbor->GetPosition().X) + FMath::Abs(currentTile->GetPosition().Y - neighbor->GetPosition().Y);
                openList.Enqueue(neighbor, cost + manhattanDistance);
            }
        }     
    }

    return path;
}

FVector UGlobalStatics::GetCursorWorldPositionOnZPlane(const UWorld* world, float zPlane)
{
    //We get the cursor world position and direction.
    auto cursorPos = FVector();
    auto cursorDir = FVector();
    world->GetFirstPlayerController()->DeprojectMousePositionToWorld(cursorPos, cursorDir);
          
    auto multiplier = (zPlane - cursorPos.Z) / cursorDir.Z;           //We calculate how long it will take for the cursor line to intersect with the plane.
    auto cursorZPlaneLocation = cursorPos + (cursorDir * multiplier); //And then we get the line point of intersection with the z plane.

    return cursorZPlaneLocation;
}

