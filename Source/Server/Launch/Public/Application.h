#pragma once

#include "Network.h"
#include "Simulation.h"
#include "SpriteData.h"
#include "PeriodicCaller.h"
#include "SDLNetInitializer.h"

#include "SDL2pp/SDL.hh"

#include <atomic>
#include <thread>

namespace AM
{
namespace Server
{
/**
 * Maintains the lifetime of all app modules (sim, network, etc) and manages
 * the main thread's loop.
 */
class Application
{
public:
    Application();

    ~Application();

    /**
     * Begins the application. Assumes control of the thread until the
     * application exits.
     */
    void start();

private:
    /**
     * Thread function that checks for command line input.
     */
    void receiveCliInput();

    /** We sleep for 1ms when possible to reduce our CPU usage. We can't trust
        the scheduler to come back to us after exactly 1ms though, so we need
        to give it some leeway. Picked .003 = 3ms as a reasonable number. Open
        for tweaking. */
    static constexpr double SLEEP_MINIMUM_TIME_S = .003;

    SDL2pp::SDL sdl;
    SDLNetInitializer sdlNetInit;

    SpriteData spriteData;

    Network network;
    PeriodicCaller networkCaller;

    Simulation sim;
    PeriodicCaller simCaller;

    /** Flags when to end the application. */
    std::atomic<bool> exitRequested;

    /** Thread to check for command line input */
    std::thread inputThreadObj;
};

} // End namespace Server
} // End namespace AM
