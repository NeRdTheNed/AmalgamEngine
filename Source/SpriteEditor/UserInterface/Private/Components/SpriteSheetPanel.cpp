#include "SpriteSheetPanel.h"
#include "MainThumbnail.h"
#include "Ignore.h"

namespace AM
{

SpriteSheetPanel::SpriteSheetPanel(AUI::Screen& screen)
: AUI::Component(screen, "SpriteSheetPanel", {0, 0, 399, 708})
, backgroundImage(screen, "", logicalExtent)
, spritesheetContainer(screen, "SpriteSheetContainer", {18, 24, 306, 636})
, remSheetButton(screen, "", {342, 0, 45, 63})
, addSheetButton(screen, "", {342, 63, 45, 88})
, remSheetDialog(screen, spritesheetContainer, remSheetButton)
, addSheetDialog(screen, spritesheetContainer)
{
    /* Background image */
    backgroundImage.addResolution({1920, 1080}, "Textures/SpriteSheetPanel/Background_1920.png"
                                  , {4, 4, 399, 708});
    backgroundImage.addResolution({1600, 900}, "Textures/SpriteSheetPanel/Background_1600.png"
                                  , {4, 4, 333, 590});
    backgroundImage.addResolution({1280, 720}, "Textures/SpriteSheetPanel/Background_1280.png"
                                  , {3, 3, 266, 472});

    /* Container */
    spritesheetContainer.setNumColumns(2);
    spritesheetContainer.setCellWidth(156);
    spritesheetContainer.setCellHeight(162);

    // Add temp sheets.
    for (unsigned int i = 0; i < 19; ++i) {
        std::unique_ptr<AUI::Component> thumbnailPtr{
            std::make_unique<MainThumbnail>(screen, "")};
        MainThumbnail& thumbnail{static_cast<MainThumbnail&>(*thumbnailPtr)};

        thumbnail.thumbnailImage.addResolution({1280, 720}, "Textures/Temp/iso_test_sprites.png");
        thumbnail.setText("Component " + std::to_string(i));
        thumbnail.setIsActivateable(false);

        // Add a callback to deselect all other components when this one
        // is selected.
        thumbnail.setOnSelected([&](AUI::Thumbnail* selectedThumb){
            // Deselect all other thumbnails.
            for (auto& componentPtr : spritesheetContainer) {
                MainThumbnail& otherThumb = static_cast<MainThumbnail&>(*componentPtr);
                if (otherThumb.getIsSelected() && (&otherThumb != selectedThumb)) {
                    otherThumb.deselect();
                }
            }

            // Make sure the remove button is enabled.
            remSheetButton.enable();
        });

        // Add a callback to disable the remove button if nothing is selected.
        thumbnail.setOnDeselected([&](AUI::Thumbnail* deselectedThumb){
            ignore(deselectedThumb);

            // Check if any thumbnails are selected.
            bool thumbIsSelected{false};
            for (auto& componentPtr : spritesheetContainer) {
                MainThumbnail& otherThumb = static_cast<MainThumbnail&>(*componentPtr);
                if (otherThumb.getIsSelected()) {
                    thumbIsSelected = true;
                }
            }

            // If none of the thumbs are selected, disable the remove button.
            if (!thumbIsSelected) {
                remSheetButton.disable();
            }
        });

        spritesheetContainer.push_back(std::move(thumbnailPtr));
    }

    /* Remove sheet button */
    remSheetButton.normalImage.addResolution({1920, 1080}, "Textures/SpriteSheetPanel/RemoveNormal.png");
    remSheetButton.hoveredImage.addResolution({1920, 1080}, "Textures/SpriteSheetPanel/RemoveHovered.png");
    remSheetButton.pressedImage.addResolution({1920, 1080}, "Textures/SpriteSheetPanel/RemoveNormal.png");
    remSheetButton.disabledImage.addResolution({1920, 1080}, "Textures/SpriteSheetPanel/RemoveDisabled.png");
    remSheetButton.text.setFont("Fonts/B612-Regular.ttf", 33);
    remSheetButton.text.setText("");
    remSheetButton.disable();

    // Add a callback to remove a selected component on button press.
    remSheetButton.setOnPressed([this](){
        // Bring up the confirmation dialog.
        remSheetDialog.setIsVisible(true);
    });

    /* Add sheet button */
    addSheetButton.normalImage.addResolution({1920, 1080}, "Textures/SpriteSheetPanel/AddNormal.png");
    addSheetButton.hoveredImage.addResolution({1920, 1080}, "Textures/SpriteSheetPanel/AddHovered.png");
    addSheetButton.pressedImage.addResolution({1920, 1080}, "Textures/SpriteSheetPanel/AddNormal.png");
    addSheetButton.text.setFont("Fonts/B612-Regular.ttf", 33);
    addSheetButton.text.setText("");

    addSheetButton.setOnPressed([&](){
        // Bring up the add dialog.
        addSheetDialog.setIsVisible(true);
    });

    /* Dialogs. */
    remSheetDialog.setIsVisible(false);
    addSheetDialog.setIsVisible(false);

    // Register for the events that we want to listen for.
    registerListener(AUI::InternalEvent::MouseButtonDown);
}

bool SpriteSheetPanel::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // If the click event was outside our extent.
    if (!(containsPoint({event.x, event.y}))) {
        // Deselect any selected component.
        for (auto& componentPtr : spritesheetContainer) {
            MainThumbnail& thumbnail = static_cast<MainThumbnail&>(*componentPtr);
            if (thumbnail.getIsSelected()) {
                thumbnail.deselect();
                break;
            }
        }
    }

    return false;
}

void SpriteSheetPanel::render(const SDL_Point& parentOffset)
{
    // Keep our scaling up to date.
    refreshScaling();

    // Save the extent that we're going to render at.
    lastRenderedExtent = scaledExtent;
    lastRenderedExtent.x += parentOffset.x;
    lastRenderedExtent.y += parentOffset.y;

    // Children should render at the parent's offset + this component's offset.
    SDL_Point childOffset{parentOffset};
    childOffset.x += scaledExtent.x;
    childOffset.y += scaledExtent.y;

    // Render our children.
    backgroundImage.render(parentOffset);

    spritesheetContainer.render(parentOffset);

    remSheetButton.render(parentOffset);

    addSheetButton.render(parentOffset);

    remSheetDialog.render(parentOffset);

    addSheetDialog.render(parentOffset);
}

} // End namespace AM