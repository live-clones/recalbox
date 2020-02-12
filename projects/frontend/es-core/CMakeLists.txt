project("core")

set(CORE_HEADERS
		src/components/base/Component.h
		src/components/base/IComponent.h
		src/AsyncHandle.h
        src/audio/AudioManager.h
		src/help/HelpStyle.h
		src/help/Help.h
		src/HttpReq.h
		src/ImageIO.h
		src/input/InputDevice.h
		src/input/InputEvent.h
		src/input/InputCompactEvent.h
		src/input/InputManager.h
		src/input/InputStack.h
		src/utils/Log.h
		src/utils/locale/LocaleHelper.h
		src/audio/Music.h
		src/platform.h
		src/Renderer.h
		src/Settings.h
		src/audio/Sound.h
		src/RootFolders.h
		src/MenuThemeData.h
		src/VideoEngine.h
		src/Window.h
		src/utils/sdl2/ISynchronousEvent.h
		src/utils/sdl2/ISyncTimer.h
		src/utils/sdl2/SyncronousEventService.h
		src/utils/sdl2/SyncronousEvent.h
		src/themes/ThemeData.h
		src/themes/ThemeElement.h
		src/themes/ThemeExtras.h
		src/themes/ThemeException.h

		# Utils
		src/utils/os/fs/Path.h
		src/utils/os/fs/StringMapFile.h
		src/utils/os/fs/watching/EventType.h
		src/utils/os/fs/watching/FileSystemEvent.h
		src/utils/os/fs/watching/FileSystemWatcher.h
		src/utils/os/fs/watching/IFileSystemWatcherNotification.h
        src/utils/os/fs/watching/FileNotifier.h
		src/utils/os/system/Thread.h
		src/utils/os/system/ThreadPool.h
        src/utils/os/system/IThreadPoolWorkerInterface.h
		src/utils/os/system/Mutex.h
		src/utils/datetime/HighResolutionTimer.h
		src/utils/locale/Internationalizer.h
		src/utils/math/Vectors.h
		src/utils/math/Vector2i.h
		src/utils/math/Vector2f.h
		src/utils/math/Vector3f.h
		src/utils/math/Vector4i.h
		src/utils/math/Vector4f.h
		src/utils/math/Misc.h
		src/utils/math/Transform4x4f.h
		src/utils/Files.h
		src/utils/Http.h
		src/utils/Strings.h
		src/utils/Stringize.h
		src/utils/Xml.h
		src/utils/Zip.h

		# Animations
		src/animations/Animation.h
		src/animations/AnimationController.h
		src/animations/LambdaAnimation.h

		# GuiComponents
		src/components/AnimatedImageComponent.h
		src/components/BusyComponent.h
		src/components/ButtonComponent.h
		src/components/ComponentGrid.h
		src/components/ComponentList.h
		src/components/DateTimeComponent.h
		src/components/HelpComponent.h
		src/components/IList.h
		src/components/ImageComponent.h
		src/components/VideoComponent.h
		src/components/ImageGridComponent.h
		src/components/MenuComponent.h
		src/components/NinePatchComponent.h
		src/components/OptionListComponent.h
		src/components/ScrollableContainer.h
		src/components/SliderComponent.h
		src/components/SwitchComponent.h
		src/components/TextComponent.h
		src/components/TextEditComponent.h

		# Guis
		src/guis/Gui.h
		src/guis/GuiDetectDevice.h
		src/guis/GuiInputConfig.h
		src/guis/GuiMsgBox.h
		src/guis/GuiMsgBoxScroll.h
		src/guis/GuiTextEditPopup.h
		src/guis/GuiTextEditPopupKeyboard.h

		# Resources
		src/resources/Font.h
		src/resources/ResourceManager.h
		src/resources/TextureResource.h
		src/resources/TextureData.h
		src/resources/TextureDataManager.h

		# Datetime
		src/utils/datetime/DateTime.h
		src/utils/datetime/TimeSpan.h

		# Embedded assets (needed by ResourceManager)
		${emulationstation-all_SOURCE_DIR}/data/Resources.h

        # Hash
        src/utils/hash/Md5.h
		src/utils/hash/Crc32.h
		src/utils/hash/Crc32File.h
)

set(CORE_SOURCES
		src/components/base/Component.cpp
		src/components/base/IComponent.cpp
        src/audio/AudioManager.cpp
        src/help/HelpStyle.cpp
		src/help/Help.cpp
		src/HttpReq.cpp
		src/ImageIO.cpp
		src/input/InputDevice.cpp
		src/input/InputEvent.cpp
		src/input/InputCompactEvent.cpp
		src/input/InputManager.cpp
		src/input/InputStack.cpp
		src/utils/Log.cpp
		src/audio/Music.cpp
		src/platform.cpp
		src/Renderer.cpp
		src/Settings.cpp
		src/audio/Sound.cpp
		src/RootFolders.cpp
		src/MenuThemeData.cpp
		src/VideoEngine.cpp
		src/Window.cpp
		src/utils/sdl2/SyncTimer.cpp
		src/utils/sdl2/SyncronousEventService.cpp
		src/utils/sdl2/SyncronousEvent.cpp
		src/themes/ThemeData.cpp
		src/themes/ThemeExtras.cpp

		# Utils
		src/utils/os/system/Thread.cpp
		src/utils/os/system/Mutex.cpp
		src/utils/os/fs/Path.cpp
		src/utils/os/fs/StringMapFile.cpp
		src/utils/locale/Internationalizer.cpp
		src/utils/os/fs/watching/EventType.cpp
		src/utils/os/fs/watching/FileSystemWatcher.cpp
        src/utils/os/fs/watching/FileNotifier.cpp
		src/utils/math/Vector2i.cpp
		src/utils/math/Vector2f.cpp
		src/utils/math/Vector3f.cpp
		src/utils/math/Vector4i.cpp
		src/utils/math/Vector4f.cpp
		src/utils/math/Misc.cpp
		src/utils/math/Transform4x4f.cpp
		src/utils/Http.cpp
		src/utils/Files.cpp
		src/utils/Strings.cpp
		src/utils/Zip.cpp

		# Animations
		src/animations/AnimationController.cpp

		# GuiComponents
		src/components/AnimatedImageComponent.cpp
		src/components/BusyComponent.cpp
		src/components/ButtonComponent.cpp
		src/components/ComponentGrid.cpp
		src/components/ComponentList.cpp
		src/components/DateTimeComponent.cpp
		src/components/HelpComponent.cpp
		src/components/ImageComponent.cpp
		src/components/VideoComponent.cpp
		src/components/MenuComponent.cpp
		src/components/NinePatchComponent.cpp
		src/components/ScrollableContainer.cpp
		src/components/SliderComponent.cpp
		src/components/SwitchComponent.cpp
		src/components/TextComponent.cpp
		src/components/TextEditComponent.cpp

		# Guis
		src/guis/Gui.cpp
		src/guis/GuiDetectDevice.cpp
		src/guis/GuiInputConfig.cpp
		src/guis/GuiMsgBox.cpp
		src/guis/GuiMsgBoxScroll.cpp
		src/guis/GuiTextEditPopup.cpp
		src/guis/GuiTextEditPopupKeyboard.cpp

		# Resources
		src/resources/Font.cpp
		src/resources/ResourceManager.cpp
		src/resources/TextureResource.cpp
		src/resources/TextureData.cpp
		src/resources/TextureDataManager.cpp

		# Datetime
		src/utils/datetime/DateTime.cpp
		src/utils/datetime/TimeSpan.cpp

		# Hash
        src/utils/hash/Md5.cpp
		src/utils/hash/Crc32.cpp
		src/utils/hash/Crc32File.cpp
)

file(GLOB_RECURSE EMBEDDED_ASSET_SOURCES ${emulationstation-all_SOURCE_DIR}/data/*.cpp)

list(APPEND CORE_SOURCES ${EMBEDDED_ASSET_SOURCES})

set(FFMPEGLIBS avcodec avformat swresample avutil swscale avdevice)

include_directories(${COMMON_INCLUDE_DIRS})
add_library(es-core STATIC ${CORE_SOURCES} ${CORE_HEADERS} src/RecalboxConf.cpp src/RecalboxConf.h)
target_link_libraries(es-core ${COMMON_LIBRARIES} ${FFMPEGLIBS} curl zip)

