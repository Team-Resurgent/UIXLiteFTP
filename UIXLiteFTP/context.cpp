#include "context.h"
#include "inputManager.h"
#include "pointerMap.h"

namespace 
{
	char* mCurrentIp = NULL;
	bool mNetworkInitialized;
	LPDIRECT3DDEVICE8 mD3dDevice;
	int32_t mBufferWidth;
	int32_t mBufferHeight;
	int32_t mBufferPitch;
	int32_t* mBuffer;
	pointerMap<image*>* mImageMap = NULL;
	bitmapFont* mBitmapFontSmall = NULL;
	bitmapFont* mBitmapFontMedium = NULL;
	bitmapFont* mBitmapFontLarge = NULL;
}

void context::setCurrentIp(const char* ip)
{
	free(mCurrentIp);
	mCurrentIp = strdup(ip);
}

char* context::getCurrentIp()
{
	return mCurrentIp == NULL ? strdup("0.0.0.0") : strdup(mCurrentIp);
}

void context::setNetworkInitialized(bool value)
{
	mNetworkInitialized = value;
}

bool context::getNetworkInitialized()
{
	return mNetworkInitialized;
}

void context::setD3dDevice(LPDIRECT3DDEVICE8 d3dDevice)
{
	mD3dDevice = d3dDevice;
}

LPDIRECT3DDEVICE8 context::getD3dDevice()
{
	return mD3dDevice;
}

void context::setBufferWidth(int32_t bufferWidth)
{
	mBufferWidth = bufferWidth;
}

int32_t context::getBufferWidth()
{
	return mBufferWidth;
}

void context::setBufferHeight(int32_t bufferHeight)
{
	mBufferHeight = bufferHeight;
}

int32_t context::getBufferHeight()
{
	return mBufferHeight;
}

void context::setBufferPitch(int32_t bufferPitch)
{
	mBufferPitch = bufferPitch;
}

int32_t context::getBufferPitch()
{
	return mBufferPitch;
}

void context::setBuffer(int32_t* buffer)
{
	mBuffer = buffer;
}

int32_t* context::getBuffer()
{
	return mBuffer;
}

void context::setImageMap(pointerMap<image*>* imageMap)
{
	mImageMap = imageMap;
}

pointerMap<image*>* context::getImageMap()
{
	return mImageMap;
}

void context::setBitmapFontSmall(bitmapFont* font)
{
	mBitmapFontSmall = font;
}

bitmapFont* context::getBitmapFontSmall()
{
	return mBitmapFontSmall;
}

void context::setBitmapFontMedium(bitmapFont* font)
{
	mBitmapFontMedium = font;
}

bitmapFont* context::getBitmapFontMedium()
{
	return mBitmapFontMedium;
}

void context::setBitmapFontLarge(bitmapFont* font)
{
	mBitmapFontLarge = font;
}

bitmapFont* context::getBitmapFontLarge()
{
	return mBitmapFontLarge;
}