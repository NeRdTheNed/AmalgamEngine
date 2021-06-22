#include "TitleScreen.h"
#include "UserInterface.h"
#include "AUI/Core.h"
#include "nfd.h"
#include "Log.h"

namespace AM
{
namespace SpriteEditor
{

MainScreen::MainScreen(SpriteData& inSpriteData)
: Screen("MainScreen")
, spriteData{inSpriteData}
, spritesheetPanel(*this)
{
}

void MainScreen::loadSpriteData()
{
    // TODO: Load spriteData into the UI.
}

void MainScreen::render()
{
    // Fill the background with the correct color.
    SDL_Renderer* renderer = AUI::Core::GetRenderer();
    SDL_SetRenderDrawColor(renderer, 17, 17, 19, 255);
    SDL_RenderClear(renderer);

    // Render our children.
    spritesheetPanel.render();
}

} // End namespace SpriteEditor
} // End namespace AM