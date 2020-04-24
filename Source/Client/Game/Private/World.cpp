#include <World.h>
#include <iostream>

AM::World::World()
: entityNames {},
  positions {},
  movements {},
  inputs {},
  sprites {},
  componentFlags {},
  playerID(0)
{
}

void AM::World::AddEntity(const std::string& name, EntityID ID)
{
    playerID = ID;
    entityNames[ID] = name;
}

void AM::World::RemoveEntity(EntityID entityID)
{
    componentFlags[entityID] = 0;
    entityNames[entityID] = "";
}

bool AM::World::entityExists(EntityID entityID)
{
    return (componentFlags[entityID] == 0) ? false : true;
}

void AM::World::AttachComponent(EntityID entityID, ComponentFlag::FlagType componentFlag)
{
    // If the entity doesn't have the component, add it.
    if ((componentFlags[entityID] & componentFlag) == 0) {
        componentFlags[entityID] |= componentFlag;
    }
    else {
        std::cerr << "Tried to add component when entity already has it." << std::endl;
    }
}

void AM::World::RemoveComponent(EntityID entityID, ComponentFlag::FlagType componentFlag)
{
    // If the entity has the component, remove it.
    if ((componentFlags[entityID] & componentFlag) == componentFlag) {
        componentFlags[entityID] |= componentFlag;
    }
    else {
        std::cerr << "Tried to remove component when entity doesn't have it."
        << std::endl;
    }
}

void AM::World::registerPlayerID(EntityID inPlayerID)
{
    playerID = inPlayerID;
}

AM::EntityID AM::World::getPlayerID()
{
    return playerID;
}
