#pragma once

#include "xboxInternals.h"
#include "alignment.h"
#include "pointerVector.h"

#define THEME_BACKGROUND_OVERLAY_AS_UNDERLAY 0
#define THEME_BACKGROUND_FRAME_DELAY 0
#define THEME_BACKGROUND_FRAME_PINGPONG 0
#define THEME_BACKGROUND_COLOR 0x00000000
#define THEME_BACKGROUND_IMAGE_TINT 0xffffffff
#define THEME_BACKGROUND_OVERLAY_IMAGE_TINT 0xffffffff
#define THEME_PROMETHEOS_ALIGN 1
#define THEME_PROMETHEOS_Y 32
#define THEME_PROMETHEOS_COLOR 0x00ffcd00
#define THEME_INSTALLER_TINT 0xffffffff
#define THEME_TEXT_COLOR 0xFF7EB900
#define THEME_TEXT_DISABLED_COLOR 0xFF909090
#define THEME_HEADER_TEXT_COLOR 0xFF7EB900
#define THEME_FOOTER_TEXT_COLOR 0xFF7EB900

#define THEME_HEADER_ALIGN 1
#define THEME_HEADER_Y 420
#define THEME_CENTER_OFFSET 4
#define THEME_FOOTER_Y 420

#define THEME_JOY_BUTTON_A_COLOR 0xffffffff
#define THEME_JOY_BUTTON_B_COLOR 0xffffffff
#define THEME_JOY_BUTTON_X_COLOR 0xffffffff
#define THEME_JOY_BUTTON_Y_COLOR 0xffffffff
#define THEME_JOY_BUTTON_BLACK_COLOR 0xff666666
#define THEME_JOY_BUTTON_WHITE_COLOR 0xffffffff

#define THEME_BUTTON_ACTIVE_FILL_COLOR 0xff577A1B
#define THEME_BUTTON_ACTIVE_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_ACTIVE_TEXT_COLOR 0xff0E3205

#define THEME_BUTTON_ACTIVE_HOVER_FILL_COLOR 0xff577A1B
#define THEME_BUTTON_ACTIVE_HOVER_STROKE_COLOR 0xffAADC39
#define THEME_BUTTON_ACTIVE_HOVER_TEXT_COLOR 0xff0E3205

#define THEME_BUTTON_INACTIVE_FILL_COLOR 0xff0B2A07
#define THEME_BUTTON_INACTIVE_STROKE_COLOR 0xff517B2F
#define THEME_BUTTON_INACTIVE_TEXT_COLOR 0xff76BB2D

#define THEME_BUTTON_INACTIVE_HOVER_FILL_COLOR 0xff577A1B
#define THEME_BUTTON_INACTIVE_HOVER_STROKE_COLOR 0xffAADC39
#define THEME_BUTTON_INACTIVE_HOVER_TEXT_COLOR 0xff0E3205

#define THEME_PANEL_FILL_COLOR 0x00000000
#define THEME_PANEL_STROKE_COLOR 0x00141e26

#define THEME_TEXT_PANEL_FILL_COLOR 0xc8464646
#define THEME_TEXT_PANEL_STROKE_COLOR 0xffAADC39
#define THEME_TEXT_PANEL_TEXT_COLOR 0xff0E3205
#define THEME_TEXT_PANEL_HOVER_FILL_COLOR 0xff577A1B
#define THEME_TEXT_PANEL_HOVER_STROKE_COLOR 0xffAADC39
#define THEME_TEXT_PANEL_HOVER_TEXT_COLOR 0xff0E3205

class theme
{
public:

	typedef struct ThemeData {

		uint32_t BACKGROUND_OVERLAY_AS_UNDERLAY;
		uint32_t BACKGROUND_FRAME_DELAY;
		uint32_t BACKGROUND_FRAME_PINGPONG;
		uint32_t BACKGROUND_COLOR;
		uint32_t BACKGROUND_IMAGE_TINT;
		uint32_t BACKGROUND_OVERLAY_IMAGE_TINT;
		uint32_t PROMETHEOS_ALIGN;
		uint32_t PROMETHEOS_Y;
		uint32_t PROMETHEOS_COLOR;
		uint32_t INSTALLER_TINT;
		uint32_t TEXT_COLOR;
		uint32_t TEXT_DISABLED_COLOR;
		uint32_t HEADER_TEXT_COLOR;
		uint32_t FOOTER_TEXT_COLOR;

		uint32_t HEADER_ALIGN;
		uint32_t HEADER_Y;
		int32_t  CENTER_OFFSET;
		uint32_t FOOTER_Y;

		uint32_t JOY_BUTTON_A_COLOR;
		uint32_t JOY_BUTTON_B_COLOR;
		uint32_t JOY_BUTTON_X_COLOR;
		uint32_t JOY_BUTTON_Y_COLOR;
		uint32_t JOY_BUTTON_BLACK_COLOR;
		uint32_t JOY_BUTTON_WHITE_COLOR;

		uint32_t BUTTON_ACTIVE_FILL_COLOR;
		uint32_t BUTTON_ACTIVE_STROKE_COLOR;
		uint32_t BUTTON_ACTIVE_TEXT_COLOR;

		uint32_t BUTTON_ACTIVE_HOVER_FILL_COLOR;
		uint32_t BUTTON_ACTIVE_HOVER_STROKE_COLOR;
		uint32_t BUTTON_ACTIVE_HOVER_TEXT_COLOR;

		uint32_t BUTTON_INACTIVE_FILL_COLOR;
		uint32_t BUTTON_INACTIVE_STROKE_COLOR;
		uint32_t BUTTON_INACTIVE_TEXT_COLOR;

		uint32_t BUTTON_INACTIVE_HOVER_FILL_COLOR;
		uint32_t BUTTON_INACTIVE_HOVER_STROKE_COLOR;
		uint32_t BUTTON_INACTIVE_HOVER_TEXT_COLOR;

		uint32_t PANEL_FILL_COLOR;
		uint32_t PANEL_STROKE_COLOR;

		uint32_t TEXT_PANEL_FILL_COLOR;
		uint32_t TEXT_PANEL_STROKE_COLOR;
		uint32_t TEXT_PANEL_TEXT_COLOR;
		uint32_t TEXT_PANEL_HOVER_FILL_COLOR;
		uint32_t TEXT_PANEL_HOVER_STROKE_COLOR;
		uint32_t TEXT_PANEL_HOVER_TEXT_COLOR;

	} ThemeData;

	static uint32_t getBackgroundFrameCount();

	static bool getBackgroundOverlayAsUnderlay();
	static uint32_t getBackgroundFrameDelay();
	static bool getBackgroundFramePingPong();
	static uint32_t getBackgroundColor();
	static uint32_t getBackgroundImageTint();
	static uint32_t getBackgroundOverlayImageTint();
	static uint32_t getPrometheosColor();
	static horizAlignment getPrometheosAlign();
	static uint32_t getPrometheosY();
	static uint32_t getInstallerTint();
	static uint32_t getTextColor();
	static uint32_t getTextDisabledColor();
	static uint32_t getHeaderTextColor();
	static uint32_t getFooterTextColor();

	static horizAlignment getHeaderAlign();
	static uint32_t getHeaderY();
	static int32_t getCenterOffset();
	static uint32_t getFooterY();

	static uint32_t getJoyButtonAColor();
	static uint32_t getJoyButtonBColor();
	static uint32_t getJoyButtonXColor();
	static uint32_t getJoyButtonYColor();
	static uint32_t getJoyButtonBlackColor();
	static uint32_t getJoyButtonWhiteColor();

	static uint32_t getButtonToggleFillColor();
	static uint32_t getButtonToggleStrokeColor();

	static uint32_t getButtonActiveFillColor();
	static uint32_t getButtonActiveStrokeColor();
	static uint32_t getButtonActiveTextColor();
	static uint32_t getButtonActiveHoverFillColor();
	static uint32_t getButtonActiveHoverStrokeColor();
	static uint32_t getButtonActiveHoverTextColor();

	static uint32_t getButtonInactiveFillColor();
	static uint32_t getButtonInactiveStrokeColor();
	static uint32_t getButtonInactiveTextColor();
	static uint32_t getButtonInactiveHoverFillColor();
	static uint32_t getButtonInactiveHoverStrokeColor();
	static uint32_t getButtonInactiveHoverTextColor();

	static uint32_t getPanelFillColor();
	static uint32_t getPanelStrokeColor();

	static uint32_t getTextPanelFillColor();
	static uint32_t getTextPanelStrokeColor();
	static uint32_t getTextPanelTextColor();
	static uint32_t getTextPanelHoverFillColor();
	static uint32_t getTextPanelHoverStrokeColor();
	static uint32_t getTextPanelHoverTextColor();

	static void loadSkin();
};