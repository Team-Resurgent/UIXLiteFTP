#include "component.h"
#include "drawing.h"
#include "ssfn.h"
#include "theme.h"

#include <string>

void component::panel(uint32_t fill, uint32_t stroke, int x, int y, int width, int height)
{
	drawing::drawNinePatch("panel-fill", fill, x, y, width, height);
	drawing::drawNinePatch("panel-stroke", stroke, x, y, width, height);
}

void component::button(bool selected, bool active, const char* label, int x, int y, int width, int height)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

	if (active)
	{
		if (selected) 
		{
			panel(theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveHoverTextColor(), x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveTextColor(), x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
		}
	}
	else
	{
		if (selected) 
		{
			panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveHoverTextColor(), x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveTextColor(), x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
		}
	}
}

void component::splitButton(bool selected, bool active, const char* label1, int label1Width, const char* label2, int x, int y, int width, int height)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label2, &textWidth, &textHeight);

	if (active)
	{
		if (selected) 
		{
			panel(theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonActiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonActiveHoverTextColor(), x + 10 + label1Width, y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonActiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonActiveTextColor(), x + 10 + label1Width, y + ((height - textHeight) / 2) - 3);
		}
	}
	else
	{
		if (selected) 
		{
			panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonInactiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonInactiveHoverTextColor(), x + 10 + label1Width, y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonInactiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonInactiveTextColor(), x + 10 + label1Width, y + ((height - textHeight) / 2) - 3);
		}
	}
}


void component::fileButton(bool selected, bool active, bool isFile, const char* label, int x, int y, int width, int height)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

	if (active)
	{
		if (selected) 
		{
			panel(theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveHoverTextColor(), x + 30, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5", theme::getButtonActiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveTextColor(), x + 30, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5", theme::getButtonActiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
	}
	else
	{
		if (selected) 
		{
			panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveHoverTextColor(), x + 30, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5", theme::getButtonInactiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveTextColor(), x + 30, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5", theme::getButtonInactiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
	}
}

void component::text(const char* label, bool disabled, horizAlignment hAlign, int x, int y, int width, int height)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

	int xPos = x;
	if (hAlign == horizAlignmentCenter)
	{
		xPos = x + ((width - textWidth) / 2);
	}
	else if (hAlign == horizAlignmentRight)
	{
		xPos = x + (width - textWidth);
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), label, disabled ? theme::getTextDisabledColor() : theme::getTextColor(), xPos, y + ((height - textHeight) / 2) - 3);
}

void component::textBox(const char* label, bool selected, bool disabled, horizAlignment hAlign, int x, int y, int width, int height, bool vAlignTop, bool solidBg)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

	int xPos = x + 16;
	if (hAlign == horizAlignmentCenter)
	{
		xPos = x + ((width - textWidth) / 2);
	}
	else if (hAlign == horizAlignmentRight)
	{
		xPos = (x + 16) + ((width - 32) - textWidth);
	}

	int yPos = vAlignTop ? (y + 16 - 3) : (y + ((height - textHeight) / 2) - 3);
	if (selected == true)
	{
		uint32_t fillColor = theme::getTextPanelHoverFillColor();
		if(solidBg) fillColor |= 0xff000000;
		panel(fillColor, theme::getTextPanelHoverStrokeColor(), x, y, width, height);
		drawing::drawBitmapString(context::getBitmapFontSmall(), label, disabled ? theme::getTextDisabledColor() : theme::getTextPanelHoverTextColor(), xPos, yPos);
	}
	else
	{
		uint32_t fillColor = theme::getTextPanelHoverFillColor();
		if(solidBg) fillColor |= 0xff000000;
		panel(fillColor, theme::getTextPanelStrokeColor(), x, y, width, height);
		drawing::drawBitmapString(context::getBitmapFontSmall(), label, disabled ? theme::getTextDisabledColor() : theme::getTextPanelTextColor(), xPos, yPos);
	}
	
}