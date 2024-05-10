// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrayZoneGameInstance.h"

/**
 * 
 */
class GRAYZONE_API Room
{
public:

	Room(FIntPoint position, int width, int depth, const UWorld* inWorld);

    inline int        GetID()           const { return this->m_ID; }
    inline FIntPoint  GetPosition()     const { return this->m_position; }
    inline FIntPoint  GetRealPosition() const { return this->m_position * UGrayZoneGameInstance::TILE_SIZE; }
    inline int        GetWidth()        const { return this->m_width; }
    inline int        GetDepth()        const { return this->m_depth; }
    inline int        GetRealWidth()    const { return this->m_width * UGrayZoneGameInstance::TILE_SIZE; }
    inline int        GetRealDepth()    const { return this->m_depth * UGrayZoneGameInstance::TILE_SIZE; }

    

    void SetPosition(FIntPoint newPos);

    FVector2f GetCenterPosition() const;
    void DebugDraw(const UWorld* inWorld);

    bool IntersectWithAnotherRoom(const TSharedRef<Room> room) const;                           //Will return true if the two m_rooms intersect with each other.
    bool IntersectWithAnotherRoom(FIntPoint roomPosition, FIntPoint roomSize) const; //Will return true if this room intersect with the specified supposed room params.
    
	~Room();
    
private:

    int m_ID;    

    FIntPoint m_position;       //The m_position of the room from the top left.

    int m_width; //The number of m_tiles on the x axis.
    int m_depth; //The number of m_tiles on the y axis.
  
};
