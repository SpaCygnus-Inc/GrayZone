// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Tile.h"

Tile::Tile()
{
    this->m_position = FIntPoint(-1, -1);
    this->ResetAndSetTileType(TileDescription::NONE_TILE, -1);

    UE_LOG(LogTemp, Error, TEXT("A default tile was made without any params."));
}

Tile::Tile(int x, int y)
{
    this->m_position       = FIntPoint(x, y);
    this->ResetAndSetTileType(TileDescription::NONE_TILE, -1);

    //We calculate the real center position.
    auto centerXPos            = x * UGrayZoneGameInstance::TILE_SIZE + (UGrayZoneGameInstance::TILE_SIZE * 0.5f);
    auto centerYPos            = y * UGrayZoneGameInstance::TILE_SIZE + (UGrayZoneGameInstance::TILE_SIZE * 0.5f);
    this->m_realCenterPosition = FVector(centerXPos, centerYPos, 0);
}

Tile::Tile(FIntPoint pos) : Tile(pos.X, pos.Y)
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
    auto tileSize = UGrayZoneGameInstance::TILE_SIZE;

    FVector StartPoint(this->GetRealPosition().X, this->GetRealPosition().Y, zPos);
    FVector EndPoint(this->GetRealPosition().X + tileSize, this->GetRealPosition().Y, zPos);
    DrawDebugLine(inWorld, StartPoint, EndPoint, color, true);

    StartPoint = EndPoint;
    EndPoint = FVector(this->GetRealPosition().X + tileSize, this->GetRealPosition().Y + tileSize, zPos);
    DrawDebugLine(inWorld, StartPoint, EndPoint, color, true);

    StartPoint = EndPoint;
    EndPoint = FVector(this->GetRealPosition().X, this->GetRealPosition().Y + tileSize, zPos);
    DrawDebugLine(inWorld, StartPoint, EndPoint, color, true);

    StartPoint = EndPoint;
    EndPoint = FVector(this->GetRealPosition().X, this->GetRealPosition().Y, zPos);
    DrawDebugLine(inWorld, StartPoint, EndPoint, color, true);
}
