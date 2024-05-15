// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Map/Tile.h"
#include "GrayZoneGameInstance.h"

//Help us specify what kind of room we're interacting with.
enum RoomType
{
    NORMAL_ROOM = 1 << 0,
    SPAWN_ROOM  = 1 << 1,
    EXIT_ROOM   = 1 << 2
};

/**
 * 
 */
class GRAYZONE_API RoomData
{
public:

    RoomData();
	RoomData(FIntPoint position, int width, int depth, TMap<FIntPoint, TWeakPtr<Tile>> tiles, const UWorld* inWorld, RoomType roomType = RoomType::NORMAL_ROOM);

    inline int        GetID()           const { return this->m_ID; }
    inline FIntPoint  GetPosition()     const { return this->m_position; }
    inline FIntPoint  GetRealPosition() const { return this->m_position * UGrayZoneGameInstance::TILE_SIZE; }
    inline int        GetWidth()        const { return this->m_width; }
    inline int        GetDepth()        const { return this->m_depth; }
    inline FIntPoint  GetSize()         const { return FIntPoint(this->GetDepth(), this->GetWidth()); }
    inline int        GetRealWidth()    const { return this->m_width * UGrayZoneGameInstance::TILE_SIZE; }
    inline int        GetRealDepth()    const { return this->m_depth * UGrayZoneGameInstance::TILE_SIZE; }
    inline RoomType   GetRoomType()     const { return this->m_roomType;}

    //We get all then wall tiles of this room in an ordered way : BA98
    //                                                            C  7
    //                                                            D  6
    //                                                            E  5
    //                                                            1234       
    TArray<TWeakPtr<Tile>> GetWallTiles();

    void SetPosition(FIntPoint newPos);

    FVector2f GetCenterPosition() const;
    FVector   GetRealCenterPos()  const; //We get the center position in world space of this room.
    void DebugDraw(const UWorld* inWorld);

    bool IntersectWithAnotherRoom(const TSharedRef<RoomData> room) const;                           //Will return true if the two m_rooms intersect with each other.
    bool IntersectWithAnotherRoom(FIntPoint roomPosition, FIntPoint roomSize) const; //Will return true if this room intersect with the specified supposed room params.
    
	~RoomData();
    
private:

    int m_ID;    

    FIntPoint m_position; //The m_position of the room from the top left.
    RoomType  m_roomType; //Specify what kidn of room this is.
    TMap<FIntPoint, TWeakPtr<Tile>> m_roomTiles;

    int m_width; //The number of m_tiles on the x axis.
    int m_depth; //The number of m_tiles on the y axis.
};
