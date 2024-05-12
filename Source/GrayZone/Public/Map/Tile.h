// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrayZoneGameInstance.h"

enum TileDescription
{
    //These are always set first to indicate what type of tile this is.
    NONE_TILE     = 1 << 0,
    ROOM_TILE     = 1 << 1,
    CORRIDOR_TILE = 1 << 2,
    
    //These added to describe what can be found on the tile.
    WALL_TILE = 1 << 3,
    DOOR_TILE = 1 << 4
};

/**
 * This is will hold information about a single tile in the grid.
 */
class GRAYZONE_API Tile
{
public:

    Tile();
	Tile(int x, int y);
    Tile(FIntPoint pos);

	~Tile();

    void DebugDraw(const UWorld* inWorld, FColor color, float zPos); //This allow us to draw a single tile using red lines.

    void ResetAndSetTileType(TileDescription type, int typeID = -1); //This will reset the tile description property and then assign it the specified tile type.
    void AddTileDescription(TileDescription description);
    void RemoveTileDescription(TileDescription description);
    inline const TileDescription GetTileDescription() const { return this->m_tileDescription; }

    inline int  GetTileTypeID()         const { return this->m_tileTypeID; }

    inline FVector2f GetRealCenterPosition() const { return this->m_realCenterPosition; }
    inline FIntPoint GetPosition()           const { return this->m_position; }
    inline FIntPoint GetRealPosition()       const { return this->m_position * UGrayZoneGameInstance::TILE_SIZE; }

private:

    FIntPoint m_position;
    FVector2f m_realCenterPosition;

    TileDescription m_tileDescription;
    int m_tileTypeID;
};
