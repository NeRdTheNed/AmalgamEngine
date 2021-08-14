#pragma once

#include "NetworkDefs.h"
#include "Position.h"

#include "entt/entity/registry.hpp"

#include <unordered_map>
#include <random>

namespace AM
{
namespace Server
{
/**
 * Owns and manages the persistence of all world state.
 *
 * The server's world state consists of:
 *   Map data
 *     See TileMap.h
 *   Entity data
 *     Maintained at runtime in an ECS registry.
 *     Eventually will be persisted in a database.
 *
 * Also provides helpers for common uses of world state.
 */
class World
{
public:
    World();

    /** Entity data registry. */
    entt::registry registry;

    /** Maps network IDs to entity IDs, used for interfacing with the
        Network. */
    std::unordered_map<NetworkID, entt::entity> netIdMap;

    /**
     * Returns a random spawn point position, with all points being within a
     * single AoI.
     */
    Position getRandomSpawnPoint();

    /**
     * Returns the next spawn point, trying to build groups of 10.
     */
    Position getGroupedSpawnPoint();

private:
    // For random spawn points.
    std::random_device device;
    std::mt19937 generator;
    std::uniform_real_distribution<float> xDistribution;
    std::uniform_real_distribution<float> yDistribution;

    // For grouped spawn points.
    static constexpr unsigned int GROUP_SIZE = 10;
    float baseX;
    float baseY;
    std::array<float, GROUP_SIZE> groupOffsetsX;
    std::array<float, GROUP_SIZE> groupOffsetsY;
    unsigned int offsetSelector;
};

} // namespace Server
} // namespace AM
