// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Tile.h"

Tile::Tile(int x, int y, int size)
{
    this->m_position       = FIntPoint(x, y);
    this->m_size           = size;
    this->m_centerPosition = FVector2f(x/2.f, y/2.f);
    this->ResetAndSetTileType(TileDescription::NONE_TILE, -1);
}

Tile::Tile(FIntPoint pos, int size) : Tile(pos.X, pos.Y, size)
{
}

Tile::~Tile()
{   
}

void Tile::ResetAndSetTileType(TileDescription type, int typeID)
{
    this->m_tileDescription   = type;
    this->m_tileTypeID = typeID;
}

void Tile::AddTileDescription(TileDescription description)
{
    switch (description)
    {
        case TileDescription::NONE_TILE | TileDescription::CORRIDOR_TILE | TileDescription::ROOM_TILE :
            UE_LOG(LogTemp, Fatal, TEXT("A tile type can't be added as a description."));
            break;
        default:
            this->m_tileDescription = static_cast<TileDescription>(this->m_tileDescription | description);
            break;
    }
}

void Tile::RemoveTileDescription(TileDescription description)
{
    switch (description)
    {
    case TileDescription::NONE_TILE | TileDescription::CORRIDOR_TILE | TileDescription::ROOM_TILE:
        UE_LOG(LogTemp, Fatal, TEXT("A tile type can't be removed. a tile type can only be set during dungeon generation."));
        break;
    default:
        this->m_tileDescription = static_cast<TileDescription>(this->m_tileDescription & ~description);
        break;
    }
}

void Tile::DebugDraw(const UWorld *inWorld, FColor color, float zPos)
{
    FVector StartPoint(this->GetRealPosition().X, this->GetRealPosition().Y, zPos);
    FVector EndPoint(this->GetRealPosition().X + this->GetRealSize(), this->GetRealPosition().Y, zPos);
    DrawDebugLine(inWorld, StartPoint, EndPoint, color, true);

    StartPoint = EndPoint;
    EndPoint = FVector(this->GetRealPosition().X + this->GetRealSize(), this->GetRealPosition().Y + this->GetRealSize(), zPos);
    DrawDebugLine(inWorld, StartPoint, EndPoint, color, true);

    StartPoint = EndPoint;
    EndPoint = FVector(this->GetRealPosition().X, this->GetRealPosition().Y + this->GetRealSize(), zPos);
    DrawDebugLine(inWorld, StartPoint, EndPoint, color, true);

    StartPoint = EndPoint;
    EndPoint = FVector(this->GetRealPosition().X, this->GetRealPosition().Y, zPos);
    DrawDebugLine(inWorld, StartPoint, EndPoint, color, true);
}
