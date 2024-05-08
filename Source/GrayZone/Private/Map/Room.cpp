// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Room.h"
#include "Global/GlobalStatics.h"

Room::Room(FIntPoint position,  int Width, int Depth, const UWorld* inWorld)
{
    this->m_ID       = UGlobalStatics::GetGameInstance(inWorld).GetUniqueID(); //We get a unique m_ID for this room.

    //We set the different dimension and m_position of this room.
    this->m_width    = Width;
    this->m_depth    = Depth;
    this->SetPosition(position);
}

void Room::SetPosition(FIntPoint newPos)
{
    if (newPos.X < 0 || newPos.Y < 0)
    {
        UE_LOG(LogTemp, Fatal, TEXT("Room coordinates shouldn't be negative."));
        return;
    }

    this->m_position = newPos;
}

FVector2f Room::GetCenterPosition() const
{
    auto centerPosition = FVector2f();

    centerPosition.X = this->m_position.X + (this->m_depth * 0.5f);
    centerPosition.Y = this->m_position.Y + (this->m_width * 0.5f);

    return centerPosition; 
}

bool Room::IntersectWithAnotherRoom(const Room* room) const
{
    return (room->m_position.X >= this->m_position.X && room->m_position.X <= this->m_position.X + this->m_width) || (room->m_position.X + room->m_width >= this->m_position.X && room->m_position.X + room->m_width <= this->m_position.X + this->m_width)
            && (room->m_position.Y >= this->m_position.Y && room->m_position.Y <= this->m_position.Y + this->m_depth) || (room->m_position.Y + room->m_depth >= this->m_position.Y && room->m_position.Y + room->m_depth <= this->m_position.Y + this->m_depth);
}

void Room::DebugDraw(const UWorld* inWorld)
{
    DrawDebugLine(inWorld, FVector(this->GetRealPosition().X, this->GetRealPosition().Y                                              , 0.2f), FVector(this->GetRealPosition().X + this->GetRealDepth(), this->GetRealPosition().Y                       , 0.2), FColor::Green, true);
    DrawDebugLine(inWorld, FVector(this->GetRealPosition().X + this->GetRealDepth(), this->GetRealPosition().Y                       , 0.2f), FVector(this->GetRealPosition().X + this->GetRealDepth(), this->GetRealPosition().Y + this->GetRealWidth(), 0.2), FColor::Green, true);
    DrawDebugLine(inWorld, FVector(this->GetRealPosition().X + this->GetRealDepth(), this->GetRealPosition().Y + this->GetRealWidth(), 0.2f), FVector(this->GetRealPosition().X, this->GetRealPosition().Y + this->GetRealWidth()                       , 0.2), FColor::Green, true);
    DrawDebugLine(inWorld, FVector(this->GetRealPosition().X, this->GetRealPosition().Y + this->GetRealWidth()                       , 0.2f), FVector(this->GetRealPosition().X, this->GetRealPosition().Y                                              , 0.2), FColor::Green, true);
}

Room::~Room()
{
}
