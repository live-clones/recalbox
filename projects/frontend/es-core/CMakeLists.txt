project("core")

set(CORE_HEADERS
		src/AsyncHandle.h
		src/AudioManager.h
		src/GuiComponent.h
		src/HelpStyle.h
		src/HttpReq.h
		src/ImageIO.h
		src/input/InputDevice.h
		src/input/InputEvent.h
		src/input/InputCompactEvent.h
		src/input/InputManager.h
		src/input/InputStack.h
		src/utils/Log.h
		src/Locale.h
		src/Music.h
		src/platform.h
		src/Renderer.h
		src/Settings.h
		src/Sound.h
		src/RootFolders.h
		src/MenuThemeData.h
		src/VideoEngine.h
		src/Window.h
		src/utils/sdl2/ISyncronousEvent.h
		src/utils/sdl2/ISyncTimer.h
		src/utils/sdl2/SyncronousEventService.h
		src/utils/sdl2/SyncronousEvent.h
		src/themes/ThemeData.h
		src/themes/ThemeElement.h
		src/themes/ThemeExtras.h

		# Utils
		src/utils/StringUtil.h
		src/utils/os/fs/Path.h
		src/utils/os/fs/StringMapFile.h
		src/utils/os/system/Thread.h
		src/utils/os/system/ThreadPool.h
		src/utils/os/system/ThreadPoolWorkerInterface.h
		src/utils/os/system/Mutex.h
		src/utils/datetime/HighResolutionTimer.h
		src/utils/math/Vectors.h
		src/utils/math/Vector2i.h
		src/utils/math/Vector2f.h
		src/utils/math/Vector3f.h
		src/utils/math/Vector4i.h
		src/utils/math/Vector4f.h
		src/utils/math/Misc.h
		src/utils/math/Transform4x4f.h
		src/utils/Stringize.h
	    src/utils/FileUtil.h

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
)

set(CORE_SOURCES
		src/AudioManager.cpp
		src/GuiComponent.cpp
		src/HelpStyle.cpp
		src/HttpReq.cpp
		src/ImageIO.cpp
		src/input/InputDevice.cpp
		src/input/InputEvent.cpp
		src/input/InputCompactEvent.cpp
		src/input/InputManager.cpp
		src/input/InputStack.cpp
		src/utils/Log.cpp
		src/Music.cpp
		src/platform.cpp
		src/Renderer.cpp
		src/Settings.cpp
		src/Sound.cpp
		src/RootFolders.cpp
		src/MenuThemeData.cpp
		src/VideoEngine.cpp
		src/Window.cpp
		src/utils/sdl2/SyncTimer.cpp
		src/utils/sdl2/SyncronousEventService.cpp
		src/utils/sdl2/SyncronousEvent.cpp
		src/themes/ThemeData.cpp
		src/themes/ThemeElement.cpp
		src/themes/ThemeExtras.cpp

		# Utils
		src/utils/os/system/Thread.cpp
		src/utils/os/system/Mutex.cpp
		src/utils/os/fs/Path.cpp
		src/utils/os/fs/StringMapFile.cpp
		src/utils/StringUtil.cpp
		src/utils/math/Vector2i.cpp
		src/utils/math/Vector2f.cpp
		src/utils/math/Vector3f.cpp
		src/utils/math/Vector4i.cpp
		src/utils/math/Vector4f.cpp
		src/utils/math/Misc.cpp
		src/utils/math/Transform4x4f.cpp
	    src/utils/FileUtil.cpp

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
)

set(EMBEDDED_ASSET_SOURCES
		${emulationstation-all_SOURCE_DIR}/data/ResourceUtil.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/splash_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/window_icon_256_png.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/button_png.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/button_filled_png.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/textinput_ninepatch_png.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/textinput_ninepatch_active_png.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/frame_png.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/scroll_gradient_png.cpp

		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_a_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_b_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_x_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_y_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_l_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_r_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_l2_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_r2_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_l3_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_r3_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_start_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_select_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__button_hotkey_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__dpad_up_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__dpad_down_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__dpad_left_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__dpad_right_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__dpad_updown_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__dpad_leftright_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__dpad_all_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__joystick_left_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/help__joystick_up_svg.cpp

		${emulationstation-all_SOURCE_DIR}/data/converted/ubuntu_condensed_ttf.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/fontawesome_webfont_ttf.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/arrow_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/option_arrow_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/checkbox_checked_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/checkbox_unchecked_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/star_filled_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/star_unfilled_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/on_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/off_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/fav_add_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/fav_remove_svg.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/slider_knob_svg.cpp

		${emulationstation-all_SOURCE_DIR}/data/converted/busy_0_png.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/busy_1_png.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/busy_2_png.cpp
		${emulationstation-all_SOURCE_DIR}/data/converted/busy_3_png.cpp
)

list(APPEND CORE_SOURCES ${EMBEDDED_ASSET_SOURCES})

set(FFMPEGLIBS avcodec avformat swresample avutil swscale avdevice)

include_directories(${COMMON_INCLUDE_DIRS})
add_library(es-core STATIC ${CORE_SOURCES} ${CORE_HEADERS} src/RecalboxConf.cpp src/RecalboxConf.h)
target_link_libraries(es-core ${COMMON_LIBRARIES} ${FFMPEGLIBS})

