#include "SpriteSheetPanel.h"
#include "MainScreen.h"
#include "MainThumbnail.h"
#include "SpriteDataModel.h"
#include "AssetCache.h"
#include "Paths.h"
#include "Ignore.h"

namespace AM
{
namespace SpriteEditor
{
SpriteSheetPanel::SpriteSheetPanel(AssetCache& inAssetCache,
                                   MainScreen& inScreen,
                                   SpriteDataModel& inSpriteDataModel)
: AUI::Window({0, 0, 399, 708}, "SpriteSheetPanel")
, assetCache{inAssetCache}
, mainScreen{inScreen}
, spriteDataModel{inSpriteDataModel}
, backgroundImage({0, 0, 399, 708})
, spriteSheetContainer({18, 24, 306, 650}, "SpriteSheetContainer")
, remSheetButton({342, 0, 45, 63})
, addSheetButton({342, 63, 45, 88})
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(spriteSheetContainer);
    children.push_back(remSheetButton);
    children.push_back(addSheetButton);

    /* Background image */
    backgroundImage.addResolution(
        {1920, 1080},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/Background_1920.png"),
        {4, 4, 399, 708});
    backgroundImage.addResolution(
        {1600, 900},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/Background_1600.png"),
        {4, 4, 333, 590});
    backgroundImage.addResolution(
        {1280, 720},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/Background_1280.png"),
        {3, 3, 266, 472});

    /* Container */
    spriteSheetContainer.setNumColumns(2);
    spriteSheetContainer.setCellWidth(156);
    spriteSheetContainer.setCellHeight(162);

    /* Remove sheet button */
    remSheetButton.normalImage.addResolution(
        {1920, 1080},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/RemoveNormal.png"));
    remSheetButton.hoveredImage.addResolution(
        {1920, 1080},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/RemoveHovered.png"));
    remSheetButton.pressedImage.addResolution(
        {1920, 1080},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/RemoveNormal.png"));
    remSheetButton.disabledImage.addResolution(
        {1920, 1080},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/RemoveDisabled.png"));
    remSheetButton.text.setFont((Paths::FONT_DIR + "B612-Regular.ttf"), 33);
    remSheetButton.text.setText("");
    remSheetButton.disable();

    // Add a callback to remove a selected widget on button press.
    remSheetButton.setOnPressed([&]() {
        // Set up our data for the confirmation dialog.
        std::string bodyText{
            "Remove the selected sprite sheet and all associated sprites?"};

        std::function<void(void)> onConfirmation = [&]() {
            // Try to find a selected sprite sheet in the container.
            int selectedIndex{-1};
            for (unsigned int i = 0; i < spriteSheetContainer.size(); ++i) {
                MainThumbnail& thumbnail{
                    static_cast<MainThumbnail&>(spriteSheetContainer[i])};
                if (thumbnail.getIsSelected()) {
                    selectedIndex = i;
                    break;
                }
            }

            // If we found a selected sprite sheet.
            if (selectedIndex != -1) {
                // Remove the sheet from the model.
                spriteDataModel.remSpriteSheet(selectedIndex);

                // Refresh the UI.
                mainScreen.loadSpriteData();

                // Disable the button since nothing is selected.
                remSheetButton.disable();
            }
            else {
                LOG_INFO("Failed to find selected sheet during remove.");
            }
        };

        // Bring up the confirmation dialog.
        mainScreen.openConfirmationDialog(bodyText, "REMOVE",
                                          std::move(onConfirmation));
    });

    /* Add sheet button */
    addSheetButton.normalImage.addResolution(
        {1920, 1080},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/AddNormal.png"));
    addSheetButton.hoveredImage.addResolution(
        {1920, 1080},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/AddHovered.png"));
    addSheetButton.pressedImage.addResolution(
        {1920, 1080},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "SpriteSheetPanel/AddNormal.png"));
    addSheetButton.text.setFont((Paths::FONT_DIR + "B612-Regular.ttf"), 33);
    addSheetButton.text.setText("");

    addSheetButton.setOnPressed([this]() {
        // Bring up the add dialog.
        mainScreen.openAddSheetDialog();
    });
}

void SpriteSheetPanel::addSpriteSheet(const SpriteSheet& sheet)
{
    std::unique_ptr<AUI::Widget> thumbnailPtr{
        std::make_unique<MainThumbnail>(assetCache, "")};
    MainThumbnail& thumbnail{static_cast<MainThumbnail&>(*thumbnailPtr)};

    thumbnail.thumbnailImage.addResolution(
        {1280, 720},
        assetCache.loadTexture(spriteDataModel.getWorkingTexturesDir()
                               + sheet.relPath));
    thumbnail.setText(sheet.relPath);
    thumbnail.setIsActivateable(false);

    // Add a callback to deselect all other widgets when this one
    // is selected.
    thumbnail.setOnSelected([&](AUI::Thumbnail* selectedThumb) {
        // Deselect all other thumbnails.
        for (auto& widgetPtr : spriteSheetContainer) {
            MainThumbnail& otherThumb{static_cast<MainThumbnail&>(*widgetPtr)};
            if (otherThumb.getIsSelected() && (&otherThumb != selectedThumb)) {
                otherThumb.deselect();
            }
        }

        // Make sure the remove button is enabled.
        remSheetButton.enable();
    });

    // Add a callback to disable the remove button if nothing is selected.
    thumbnail.setOnDeselected([&](AUI::Thumbnail* deselectedThumb) {
        ignore(deselectedThumb);

        // Check if any thumbnails are selected.
        bool thumbIsSelected{false};
        for (auto& widgetPtr : spriteSheetContainer) {
            MainThumbnail& otherThumb = static_cast<MainThumbnail&>(*widgetPtr);
            if (otherThumb.getIsSelected()) {
                thumbIsSelected = true;
            }
        }

        // If none of the thumbs are selected, disable the remove button.
        if (!thumbIsSelected) {
            remSheetButton.disable();
        }
    });

    spriteSheetContainer.push_back(std::move(thumbnailPtr));
}

void SpriteSheetPanel::clearSpriteSheets()
{
    spriteSheetContainer.clear();
}

} // End namespace SpriteEditor
} // End namespace AM
