target_sources(SpriteEditor
    PRIVATE
        Private/Widgets/BoundingBoxGizmo.cpp
        Private/Widgets/ConfirmationButton.cpp
        Private/Widgets/MainTextInput.cpp
        Private/Widgets/MainThumbnail.cpp
        Private/Widgets/TitleButton.cpp
        Private/Windows/AddSheetDialog.cpp
        Private/Windows/PropertiesPanel.cpp
        Private/Windows/SaveButtonWindow.cpp
        Private/Windows/SpriteEditStage.cpp
        Private/Windows/SpritePanel.cpp
        Private/Windows/SpriteSheetPanel.cpp
        Private/Windows/TitleWindow.cpp
        Private/MainScreen.cpp
        Private/TitleScreen.cpp
        Private/UserInterface.cpp
    PUBLIC
        Public/Widgets/BoundingBoxGizmo.h
        Public/Widgets/ConfirmationButton.h
        Public/Widgets/MainTextInput.h
        Public/Widgets/MainThumbnail.h
        Public/Widgets/TitleButton.h
        Public/Windows/AddSheetDialog.h
        Public/Windows/PropertiesPanel.h
        Public/Windows/SaveButtonWindow.h
        Public/Windows/SpriteEditStage.h
        Public/Windows/SpritePanel.h
        Public/Windows/SpriteSheetPanel.h
        Public/Windows/TitleWindow.h
        Public/MainScreen.h
        Public/TitleScreen.h
        Public/UserInterface.h
)

target_include_directories(SpriteEditor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Widgets
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Windows
)
