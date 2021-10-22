#pragma once

#include "QueuedEvents.h"
#include "Network.h"
#include "WorldSim.h"
#include "PeriodicCaller.h"
#include <SDL2/SDL_stdinc.h>
#include <atomic>

namespace AM
{
namespace LTC
{
/**
 * Represents a single simulated client.
 * Maintains only as much state as is necessary to keep the connection going
 * and send inputs.
 */
class SimulatedClient
{
public:
    SimulatedClient();

    /**
     * Calls worldSim.connect().
     */
    void connect();

    /**
     * Calls the sim and network ticks.
     */
    void tick();

    void setNetstatsLoggingEnabled(bool inNetstatsLoggingEnabled);

private:
    EventDispatcher networkEventDispatcher;

    Client::Network network;
    PeriodicCaller networkCaller;

    WorldSim worldSim;
    PeriodicCaller simCaller;

    std::atomic<bool> isConnected;
};

} // End namespace LTC
} // End namespace AM
