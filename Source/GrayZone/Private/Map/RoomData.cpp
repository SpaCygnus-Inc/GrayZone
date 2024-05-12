// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/RoomData.h"
#include "Global/GlobalStatics.h"

RoomData::RoomData() : RoomData(FIntPoint(0, 0), 0, 0, nullptr)
{
    UE_LOG(LogTemp, Error, TEXT("A room without any param was created."));
}

RoomData::RoomData(FIntPoint position,  int Width, int Depth, const UWorld* inWorld, RoomType roomType)
{
    this->m_ID       = UGlobalStatics::GetGameInstance(inWorld).GetUniqueID(); //We get a unique m_ID for this room.

    //We set the different dimension and m_position of this room.
    this->m_width    = Width;
    this->m_depth    = Depth;
    this->m_roomType = roomType;
    this->SetPosition(position);
}

void RoomData::SetPosition(FIntPoint newPos)
{
    if (newPos.X < 0 || newPos.Y < 0)
    {
        UE_LOG(LogTemp, Fatal, TEXT("Room coordinates shouldn't be negative."));
        return;
    }

    this->m_position = newPos;
}

FVector2f RoomData::GetCenterPosition() const
{
    auto centerPosition = FVector2f();

    centerPosition.X = this->m_position.X + (this->m_depth * 0.5f);
    centerPosition.Y = this->m_position.Y + (this->m_width * 0.5f);

    return centerPosition; 
}

FVector RoomData::GetRealCenterPos() const
{
    auto centerPos = this->GetCenterPosition();

    auto realCenterXPos = centerPos.X * UGrayZoneGameInstance::TILE_SIZE;
    auto realCenterYPos = centerPos.Y * UGrayZoneGameInstance::TILE_SIZE;

    return FVector(realCenterXPos, realCenterYPos, 0);
}

bool RoomData::IntersectWithAnotherRoom(const TSharedRef<RoomData> room) const
{
    return this->IntersectWithAnotherRoom(room->GetPosition(), FIntPoint(room->GetDepth(), room->GetWidth()));
}

bool RoomData::IntersectWithAnotherRoom(FIntPoint roomPosition, FIntPoint roomSize) const
{
    auto thisRoomMaxXPos = this->GetPosition().X + this->GetDepth();
    auto thisRoomMaxYPos = this->GetPosition().Y + this->GetWidth();

    auto otherRoomMaxXPos = roomPosition.X + roomSize.X;
    auto otherRoomMaxYPos = roomPosition.Y + roomSize.Y;

    return ((roomPosition.X >= this->GetPosition().X && roomPosition.X <= thisRoomMaxXPos) || (otherRoomMaxXPos >= this->GetPosition().X && otherRoomMaxXPos <= thisRoomMaxXPos))
        && ((roomPosition.Y >= this->GetPosition().Y && roomPosition.Y <= thisRoomMaxYPos) || (otherRoomMaxYPos >= this->GetPosition().Y && otherRoomMaxYPos <= thisRoomMaxYPos));
}

void RoomData::DebugDraw(const UWorld* inWorld)
{
    DrawDebugLine(inWorld, FVector(this->GetRealPosition().X, this->GetRealPosition().Y                                              , 0.2f), FVector(this->GetRealPosition().X + this->GetRealDepth(), this->GetRealPosition().Y                       , 0.2), FColor::Green, true);
    DrawDebugLine(inWorld, FVector(this->GetRealPosition().X + this->GetRealDepth(), this->GetRealPosition().Y                       , 0.2f), FVector(this->GetRealPosition().X + this->GetRealDepth(), this->GetRealPosition().Y + this->GetRealWidth(), 0.2), FColor::Green, true);
    DrawDebugLine(inWorld, FVector(this->GetRealPosition().X + this->GetRealDepth(), this->GetRealPosition().Y + this->GetRealWidth(), 0.2f), FVector(this->GetRealPosition().X, this->GetRealPosition().Y + this->GetRealWidth()                       , 0.2), FColor::Green, true);
    DrawDebugLine(inWorld, FVector(this->GetRealPosition().X, this->GetRealPosition().Y + this->GetRealWidth()                       , 0.2f), FVector(this->GetRealPosition().X, this->GetRealPosition().Y                                              , 0.2), FColor::Green, true);
}

RoomData::~RoomData()
{
}
