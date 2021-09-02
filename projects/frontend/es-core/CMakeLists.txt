project("core")

set(CORE_HEADERS
		src/components/base/Component.h
		src/components/base/IComponent.h
		src/help/HelpStyle.h
		src/help/Help.h
        src/ImageIO.h
		src/input/Input.h
		src/input/InputDevice.h
		src/input/InputEvent.h
		src/input/InputCompactEvent.h
		src/input/InputManager.h
		src/input/InputMapper.h
		src/input/InputStack.h
		src/input/OrderedDevices.h
		src/utils/Log.h
		src/utils/locale/LocaleHelper.h
		src/Renderer.h
		src/RootFolders.h
        src/themes/MenuThemeData.h
		src/VideoEngine.h
        src/WindowManager.h
		src/utils/sdl2/ISynchronousEvent.h
		src/utils/sdl2/ISyncTimer.h
		src/utils/sdl2/SyncronousEventService.h
		src/utils/sdl2/SyncronousEvent.h
		src/themes/ThemeData.h
		src/themes/ThemeElement.h
		src/themes/ThemeExtras.h
		src/themes/ThemeException.h

		# SDL2
		src/sdl2/ISdl2EventNotifier.h
		src/sdl2/Sdl2Runner.h
		src/sdl2/Sdl2Init.h

		# Hardware
		src/hardware/Board.h
		src/hardware/messaging/HardwareMessage.h
		src/hardware/messaging/HardwareMessageSender.h
		src/hardware/IBoardInterface.h
		src/hardware/messaging/IHardwareNotifications.h
		src/hardware/boards/NullBoard.h
		src/hardware/boards/pc/PcComputers.h
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo2Board.h
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo2JackEventReader.h
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo2PowerEventReader.h
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo2SpecialButtonsReader.h
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo3VolumeReader.h
		src/hardware/messaging/MessageTypes.h

		# Audio
		src/audio/AudioManager.h
		src/audio/AudioController.h
		src/audio/AudioIcons.h
		src/audio/AudioMode.h
		src/audio/DeviceDescriptor.h
		src/audio/IAudioController.h
		src/audio/Music.h
		src/audio/Sound.h
		src/audio/NameFiltering.h
		#src/audio/alsa/AlsaCard.h
		#src/audio/alsa/AlsaController.h
		#src/audio/alsa/AlsaMixer.h
		#src/audio/alsa/AlsaSwitch.h
		#src/audio/alsa/AlsaVolume.h
		#src/audio/alsa/Raspberry.h
		#src/audio/alsa/OdroidAdvanceGo2Alsa.h
		src/audio/pulseaudio/PulseAudioController.h

		# Utils
		src/utils/cplusplus/StaticLifeCycleControler.h
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
		src/utils/os/system/ProcessTree.h
		src/utils/os/system/Signal.h
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
		src/utils/storage/Allocator.h
		src/utils/storage/Array.h
		src/utils/storage/Common.h
		src/utils/storage/HashMap.h
		src/utils/storage/MessageFactory.h
		src/utils/storage/Queue.h
		src/utils/storage/Set.h
		src/utils/storage/Stack.h
		src/utils/IniFile.h
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
		src/components/ClippingContainer.h
		src/components/ComponentGrid.h
		src/components/ComponentList.h
		src/components/DateTimeComponent.h
		src/components/EditableComponent.h
        src/components/GameClipContainer.h
		src/components/HelpComponent.h
		src/components/IEditableComponent.h
		src/components/IList.h
		src/components/IOptionListComponent.h
		src/components/IOptionListMultiComponent.h
		src/components/ISliderComponent.h
		src/components/ISwitchComponent.h
		src/components/ImageComponent.h
		src/components/VideoComponent.h
		src/components/ImageGridComponent.h
		src/components/MenuComponent.h
		src/components/NinePatchComponent.h
		src/components/OptionListComponent.h
		src/components/ProgressBarComponent.h
		src/components/ScrollableContainer.h
		src/components/SliderComponent.h
		src/components/SwitchComponent.h
		src/components/TextComponent.h
		src/components/TextEditComponent.h

		# Guis
		src/guis/Gui.h
		src/guis/GuiDetectDevice.h
		src/guis/GuiInfoPopup.h
		src/guis/GuiInputConfig.h
		src/guis/GuiMsgBox.h
		src/guis/GuiMsgBoxScroll.h
		src/guis/GuiTextEditPopup.h
		src/guis/GuiTextEditPopupKeyboard.h
		src/guis/GuiArcadeVirtualKeyboard.h
        src/guis/IGuiArcadeVirtualKeyboardInterface.h

		# Resources
		src/resources/Font.h
		src/resources/IReloadable.h
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
        src/help/HelpStyle.cpp
		src/help/Help.cpp
        src/ImageIO.cpp
		src/input/InputDevice.cpp
		src/input/InputEvent.cpp
		src/input/InputCompactEvent.cpp
		src/input/InputManager.cpp
		src/input/InputMapper.cpp
		src/input/InputStack.cpp
		src/utils/Log.cpp
		src/Renderer.cpp
		src/RootFolders.cpp
        src/themes/MenuThemeData.cpp
		src/VideoEngine.cpp
        src/WindowManager.cpp
		src/utils/sdl2/SyncTimer.cpp
		src/utils/sdl2/SyncronousEventService.cpp
		src/utils/sdl2/SyncronousEvent.cpp
		src/themes/ThemeData.cpp
		src/themes/ThemeExtras.cpp

		# SDL2
		src/sdl2/Sdl2Runner.cpp
		src/sdl2/Sdl2Init.cpp

		# Hardware
		src/hardware/Board.cpp
		src/hardware/messaging/HardwareMessageSender.cpp
		src/hardware/boards/NullBoard.cpp
		src/hardware/boards/pc/PcComputers.cpp
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo2Board.cpp
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo2JackEventReader.cpp
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo2PowerEventReader.cpp
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo2SpecialButtonsReader.cpp
		src/hardware/boards/odroidadvancego2/OdroidAdvanceGo3VolumeReader.cpp

		# Audio
		src/audio/AudioManager.cpp
		src/audio/AudioController.cpp
		src/audio/AudioMode.cpp
		src/audio/Music.cpp
		src/audio/Sound.cpp
		src/audio/NameFiltering.cpp
		#src/audio/alsa/AlsaCard.cpp
		#src/audio/alsa/AlsaController.cpp
		#src/audio/alsa/AlsaMixer.cpp
		#src/audio/alsa/AlsaSwitch.cpp
		#src/audio/alsa/AlsaVolume.cpp
		#src/audio/alsa/Raspberry.cpp
		#src/audio/alsa/OdroidAdvanceGo2Alsa.cpp
		src/audio/pulseaudio/PulseAudioController.cpp

		# Utils
		src/utils/os/system/Thread.cpp
		src/utils/os/system/Mutex.cpp
		src/utils/os/system/ProcessTree.cpp
		src/utils/os/system/Signal.cpp
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
		src/utils/IniFile.cpp
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
		src/components/ClippingContainer.cpp
		src/components/ComponentGrid.cpp
		src/components/ComponentList.cpp
		src/components/DateTimeComponent.cpp
		src/components/EditableComponent.cpp
        src/components/GameClipContainer.cpp
		src/components/HelpComponent.cpp
		src/components/ImageComponent.cpp
		src/components/VideoComponent.cpp
		src/components/MenuComponent.cpp
		src/components/NinePatchComponent.cpp
		src/components/ProgressBarComponent.cpp
		src/components/ScrollableContainer.cpp
		src/components/SliderComponent.cpp
		src/components/SwitchComponent.cpp
		src/components/TextComponent.cpp
		src/components/TextEditComponent.cpp

		# Guis
		src/guis/Gui.cpp
		src/guis/GuiDetectDevice.cpp
		src/guis/GuiInfoPopup.cpp
		src/guis/GuiInputConfig.cpp
		src/guis/GuiMsgBox.cpp
		src/guis/GuiMsgBoxScroll.cpp
		src/guis/GuiTextEditPopup.cpp
		src/guis/GuiTextEditPopupKeyboard.cpp
		src/guis/GuiArcadeVirtualKeyboard.cpp

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
        src/components/GameClipNoVideoContainer.cpp src/components/GameClipNoVideoContainer.h)

file(GLOB_RECURSE EMBEDDED_ASSET_SOURCES ${emulationstation-all_SOURCE_DIR}/data/*.cpp)

list(APPEND CORE_SOURCES ${EMBEDDED_ASSET_SOURCES})

set(FFMPEGLIBS avcodec avformat swresample avutil swscale avdevice)

include_directories(${COMMON_INCLUDE_DIRS})
add_library(es-core STATIC ${CORE_SOURCES} ${CORE_HEADERS} src/RecalboxConf.cpp src/RecalboxConf.h)
target_link_libraries(es-core ${COMMON_LIBRARIES} ${FFMPEGLIBS} curl zip procps)

