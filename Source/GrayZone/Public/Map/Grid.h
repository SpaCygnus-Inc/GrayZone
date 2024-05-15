// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "RoomData.h"

/**
 * 
 */
class GRAYZONE_API Grid
{
public:

	Grid(int width, int depth, int tileSize = 1);
	~Grid();

    void DebugDraw(const UWorld* inWorld, bool drawDetailedDungeonTiles = false); //This will allow us to draw the whole grid using red lines.

    FIntPoint GetHalfGridPos() const;
    bool RoomIsInsideGrid(TSharedRef<RoomData> room) const;

    void SetRoomTiles(TSharedRef<RoomData> room);
    void CleanRoomTiles(TSharedRef<RoomData> room);
    void ClearAllTiles();          //We reset all m_tiles types to none and make sure to allow changing them again.

    void GenerateCorridorsTiles(const UWorld* inWorld); //Allow us to generate corridors m_tiles on the empty m_tiles.
    void GenerateCorridordsOnAxis(bool horizontal, const UWorld* inWorld);

    TSharedRef<Tile>         GetRoomCenterTile(TSharedRef<RoomData> const room) const; //We get approximately the center tile of the specified room.
    TArray<TSharedRef<Tile>> GetTileNeighbors(TSharedPtr<Tile> tile)            const; //We get all the neighbor tiles of the specified tile.

    inline int32 GetWidth() const { return this->m_width; }
    inline int32 GetDepth() const { return this->m_depth; }

    TSharedRef<Tile> GetTileAtPos(int x, int y)       const;
    TSharedRef<Tile> GetTileAtPos(FIntPoint position) const;

    TMap<FIntPoint, TWeakPtr<Tile>> GetTiles(FIntPoint position, FIntPoint size); //Get all tiles starting from specified position and going through the specified size.

    TArray<TSharedRef<Tile>> GetAllTilesWithDescription(TileDescription description) const; //We get the refs of all the tiles that have the specified tile description.

    TSet<int> GenerateDoorsFromPathAndReturnAffectedRooms(TArray<TSharedRef<Tile>> path);   //Using the specified path, we set the tiles that will be considered as doors and then return the rooms IDs that the path passed from.
    void GenerateWallsForSpecifiedRoom(TSharedRef<RoomData> room);                          //Make sure to specify the walls tiles for the specified room.
private:
    
    //These are the directions that each tile must check if we want to get its neighbors.
    const TArray<FIntPoint> m_tileNeighborsDirections = { FIntPoint(0, 1), FIntPoint(0, -1), FIntPoint(1, 0), FIntPoint(-1, 0) };

    int m_width;
    int m_depth;

    bool m_corridorsTilesGenerated;

    TMap<FIntPoint, TSharedPtr<Tile>> m_tiles;

    bool AllowChangeTilesTypes() const;
};
