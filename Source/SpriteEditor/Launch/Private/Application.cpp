#include "Application.h"
#include "Config.h"
#include "Paths.h"
#include "Log.h"

#include <SDL.h>
#include "nfd.hpp"

#include <memory>
#include <functional>
#include "Ignore.h"

namespace AM
{
namespace SpriteEditor
{
Application::Application()
: sdl(SDL_INIT_VIDEO)
, sdlWindow("Amalgam Sprite Editor", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, Config::ACTUAL_SCREEN_WIDTH,
            Config::ACTUAL_SCREEN_HEIGHT, SDL_WINDOW_SHOWN)
, sdlRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED)
, assetCache(sdlRenderer.Get())
, spriteDataModel(sdlRenderer.Get())
, userInterface(sdlRenderer.Get(), assetCache, spriteDataModel)
, uiCaller(std::bind_front(&UserInterface::tick, &userInterface),
           Config::UI_TICK_TIMESTEP_S, "UserInterface", true)
, renderer(sdlRenderer.Get(), userInterface)
, rendererCaller(std::bind_front(&Renderer::render, &renderer),
                 Renderer::FRAME_TIMESTEP_S, "Renderer", true)
, eventHandlers{this, &userInterface, &renderer}
, exitRequested(false)
{
    // Initialize nativefiledialog.
    if (NFD_Init() != NFD_OKAY) {
        LOG_FATAL("Nativefiledialog failed to initialize properly.");
    }

    // Set fullscreen mode.
    switch (Config::FULLSCREEN_MODE) {
        case 0:
            sdlWindow.SetFullscreen(0);
            break;
        case 1:
            sdlWindow.SetFullscreen(SDL_WINDOW_FULLSCREEN);
            break;
        case 2:
            sdlWindow.SetFullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);
            break;
        default:
            LOG_FATAL("Invalid fullscreen value: %d", Config::FULLSCREEN_MODE);
    }

    // Set scaling quality.
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, Config::SCALING_QUALITY);

    // Set up our event filter.
    SDL_SetEventFilter(&Application::filterEvents, this);
}

Application::~Application()
{
    // De-initialize nativefiledialog.
    NFD_Quit();
}

void Application::start()
{
    // Prime the timers so they don't start at 0.
    rendererCaller.initTimer();
    uiCaller.initTimer();
    while (!exitRequested) {
        // Let the renderer render if it needs to.
        rendererCaller.update();

        // Let the UI widgets tick if they need to.
        uiCaller.update();

        // If we have enough time, dispatch events.
        if (enoughTimeTillNextCall(DISPATCH_MINIMUM_TIME_S)) {
            dispatchEvents();
        }

        // If we have enough time, sleep.
        if (enoughTimeTillNextCall(SLEEP_MINIMUM_TIME_S)) {
            // We have enough time to sleep for a few ms.
            // Note: We try to delay for 1ms because the OS will generally end
            //       up delaying us for 1-3ms.
            SDL_Delay(1);
        }
    }
}

bool Application::handleOSEvent(SDL_Event& event)
{
    switch (event.type) {
        case SDL_QUIT:
            exitRequested = true;
            return true;
    }

    return false;
}

void Application::dispatchEvents()
{
    // Dispatch all waiting SDL events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Pass the event to each handler in order, stopping if it returns as
        // handled.
        for (OSEventHandler* handler : eventHandlers) {
            if (handler->handleOSEvent(event)) {
                break;
            }
        }
    }
}

bool Application::enoughTimeTillNextCall(double minimumTime)
{
    if ((uiCaller.getTimeTillNextCall() > minimumTime)
        && (rendererCaller.getTimeTillNextCall() > minimumTime)) {
        return true;
    }
    else {
        return false;
    }
}

int Application::filterEvents(void* userData, SDL_Event* event)
{
    Application* app{static_cast<Application*>(userData)};
    ignore(event);
    ignore(app);

    // Currently no events that we care to filter.
    //
    // switch (event->type) {
    // }

    return 1;
}

} // End namespace SpriteEditor
} // End namespace AM
