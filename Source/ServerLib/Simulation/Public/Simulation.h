#pragma once

#include "World.h"
#include "ClientConnectionSystem.h"
#include "TileUpdateSystem.h"
#include "ClientAOISystem.h"
#include "InputSystem.h"
#include "MovementSystem.h"
#include "MovementUpdateSystem.h"
#include "ChunkStreamingSystem.h"
#include <SDL_stdinc.h>
#include <atomic>

namespace AM
{
namespace Server
{
class Network;
class SpriteData;

/**
 * Manages the simulation, including world state and system processing.
 *
 * The simulation is built on an ECS architecture:
 *   Entities exist in a registry, owned by the World class.
 *   Components that hold data are attached to each entity.
 *   Systems that act on sets of components are owned and ran by this class.
 */
class Simulation
{
public:
    /** An unreasonable amount of time for the sim tick to be late by. */
    static constexpr double SIM_DELAYED_TIME_S = .001;

    Simulation(EventDispatcher& inNetworkEventDispatcher, Network& inNetwork,
               SpriteData& inSpriteData);

    /**
     * Updates accumulatedTime. If greater than the tick timestep, processes
     * the next sim iteration.
     */
    void tick();

    Uint32 getCurrentTick();

private:
    /** Used to receive events (through the Network's dispatcher) and to
        send messages. */
    Network& network;

    World world;

    /** The tick number that we're currently on. */
    std::atomic<Uint32> currentTick;

    //-------------------------------------------------------------------------
    // Systems
    //-------------------------------------------------------------------------
    ClientConnectionSystem clientConnectionSystem;
    TileUpdateSystem tileUpdateSystem;
    ClientAOISystem clientAOISystem;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    MovementUpdateSystem movementUpdateSystem;
    ChunkStreamingSystem chunkStreamingSystem;
};

} // namespace Server
} // namespace AM