#include "MainScreen.h"
#include "UserInterface.h"
#include "AUI/Core.h"
#include "nfd.h"
#include "Log.h"

namespace AM
{
namespace SpriteEditor
{

MainScreen::MainScreen(SpriteDataModel& inSpriteDataModel)
: Screen("MainScreen")
, spriteDataModel{inSpriteDataModel}
, activeSprite{nullptr}
, spriteSheetPanel(*this, spriteDataModel)
, spritePanel(*this)
, saveButton(*this, "", {1537, 0, 58, 58})
{
    /* Save button. */
    saveButton.normalImage.addResolution({1920, 1080}, "Textures/SaveButton/Normal.png");
    saveButton.hoveredImage.addResolution({1920, 1080}, "Textures/SaveButton/Hovered.png");
    saveButton.pressedImage.addResolution({1920, 1080}, "Textures/SaveButton/Pressed.png");
    saveButton.text.setFont("Fonts/B612-Regular.ttf", 33);
    saveButton.text.setText("");

    // Add a callback to save the current sprite data when pressed.
    saveButton.setOnPressed([this]() {
        // Save the data.
        spriteDataModel.save();

        // TODO: Open the confirmation dialog.
        LOG_INFO("Saved");
    });
}

void MainScreen::loadSpriteData()
{
    // Clear out the old components.
    spriteSheetPanel.clearSpriteSheets();
    spritePanel.clearSprites();
    activeSprite = nullptr;

    // For each sprite sheet in the model.
    for (const SpriteSheet& sheet : spriteDataModel.getSpriteSheets()) {
        // Add a Thumbnail component that displays the sheet.
        spriteSheetPanel.addSpriteSheet(sheet);

        // For each sprite in the sheet.
        for (const SpriteStaticData& sprite : sheet.sprites) {
            // Add a Thumbnail component that displays the sprite.
            spritePanel.addSprite(sheet, sprite);
        }
    }
}

void MainScreen::setActiveSprite(const SpriteStaticData* inActiveSprite)
{
    activeSprite = inActiveSprite;
}

const SpriteStaticData* MainScreen::getActiveSprite()
{
    return activeSprite;
}

void MainScreen::render()
{
    // Fill the background with the correct color.
    SDL_Renderer* renderer = AUI::Core::GetRenderer();
    SDL_SetRenderDrawColor(renderer, 17, 17, 19, 255);
    SDL_RenderClear(renderer);

    // Render our children.
    spriteSheetPanel.render();

    spritePanel.render();

    saveButton.render();
}

} // End namespace SpriteEditor
} // End namespace AM
