#include "stdafx.h"
#include <xtl.h>
#include <string>

#include "audioPlayer.h"
#include "component.h"
#include "context.h"
#include "drawing.h"
#include "inputManager.h"
#include "xboxinternals.h"
#include "meshUtility.h"
#include "utils.h"
#include "resources.h"
#include "stringUtility.h"
#include "theme.h"
#include "ssfn.h"
#include "ftpServer.h"
#include "driveManager.h"
#include "network.h"

#include <xgraphics.h>

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

typedef struct {
    DWORD dwWidth;
    DWORD dwHeight;
    BOOL  fProgressive;
    BOOL  fWideScreen;
	DWORD dwFreq;
} DISPLAY_MODE;

DISPLAY_MODE displayModes[] =
{
    //{   720,    480,    TRUE,   TRUE,  60 },         // 720x480 progressive 16x9
    //{   720,    480,    TRUE,   FALSE, 60 },         // 720x480 progressive 4x3
    //{   720,    480,    FALSE,  TRUE,  50 },         // 720x480 interlaced 16x9 50Hz
    //{   720,    480,    FALSE,  FALSE, 50 },         // 720x480 interlaced 4x3  50Hz
    //{   720,    480,    FALSE,  TRUE,  60 },         // 720x480 interlaced 16x9
    //{   720,    480,    FALSE,  FALSE, 60 },         // 720x480 interlaced 4x3


	// Width  Height Progressive Widescreen

	// HDTV Progressive Modes
    {  1280,    720,    TRUE,   TRUE,  60 },         // 1280x720 progressive 16x9

	// EDTV Progressive Modes
    {   720,    480,    TRUE,   TRUE,  60 },         // 720x480 progressive 16x9
    {   640,    480,    TRUE,   TRUE,  60 },         // 640x480 progressive 16x9
    {   720,    480,    TRUE,   FALSE, 60 },         // 720x480 progressive 4x3
    {   640,    480,    TRUE,   FALSE, 60 },         // 640x480 progressive 4x3

	// HDTV Interlaced Modes
	//    {  1920,   1080,    FALSE,  TRUE,  60 },         // 1920x1080 interlaced 16x9

	// SDTV PAL-50 Interlaced Modes
    {   720,    480,    FALSE,  TRUE,  50 },         // 720x480 interlaced 16x9 50Hz
    {   640,    480,    FALSE,  TRUE,  50 },         // 640x480 interlaced 16x9 50Hz
    {   720,    480,    FALSE,  FALSE, 50 },         // 720x480 interlaced 4x3  50Hz
    {   640,    480,    FALSE,  FALSE, 50 },         // 640x480 interlaced 4x3  50Hz

	// SDTV NTSC / PAL-60 Interlaced Modes
    {   720,    480,    FALSE,  TRUE,  60 },         // 720x480 interlaced 16x9
    {   640,    480,    FALSE,  TRUE,  60 },         // 640x480 interlaced 16x9
    {   720,    480,    FALSE,  FALSE, 60 },         // 720x480 interlaced 4x3
    {   640,    480,    FALSE,  FALSE, 60 },         // 640x480 interlaced 4x3
};

#define NUM_MODES (sizeof(displayModes) / sizeof(displayModes[0]))

bool supportsMode(DISPLAY_MODE mode, DWORD dwVideoStandard, DWORD dwVideoFlags)
{
    if (mode.dwFreq == 60 && !(dwVideoFlags & XC_VIDEO_FLAGS_PAL_60Hz) && (dwVideoStandard == XC_VIDEO_STANDARD_PAL_I))
	{
		return false;
	}    
    if (mode.dwFreq == 50 && (dwVideoStandard != XC_VIDEO_STANDARD_PAL_I))
	{
		return false;
	}
    if (mode.dwHeight == 480 && mode.fWideScreen && !(dwVideoFlags & XC_VIDEO_FLAGS_WIDESCREEN ))
	{
		return false;
	}
    if (mode.dwHeight == 480 && mode.fProgressive && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_480p))
	{
		return false;
	}
    if (mode.dwHeight == 720 && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_720p))
	{
		return false;
	}
    if (mode.dwHeight == 1080 && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_1080i))
	{
		return false;
	}
    return true;
}

bool createDevice()
{
	uint32_t videoFlags = XGetVideoFlags();
	uint32_t videoStandard = XGetVideoStandard();
	uint32_t currentMode;
    for (currentMode = 0; currentMode < NUM_MODES-1; currentMode++)
    {
		if (supportsMode(displayModes[currentMode], videoStandard, videoFlags)) 
		{
			break;
		}
    } 

	LPDIRECT3D8 d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if(d3d == NULL)
	{
		utils::debugPrint("Failed to create d3d\n");
        return false;
	}

	context::setBufferWidth(720);
	context::setBufferHeight(480);
	//context::setBufferWidth(displayModes[currentMode].dwWidth);
	//context::setBufferHeight(displayModes[currentMode].dwHeight);

	D3DPRESENT_PARAMETERS params; 
    ZeroMemory(&params, sizeof(params));
	params.BackBufferWidth = displayModes[currentMode].dwWidth;
    params.BackBufferHeight = displayModes[currentMode].dwHeight;
	params.Flags = displayModes[currentMode].fProgressive ? D3DPRESENTFLAG_PROGRESSIVE : D3DPRESENTFLAG_INTERLACED;
    params.Flags |= displayModes[currentMode].fWideScreen ? D3DPRESENTFLAG_WIDESCREEN : 0;
    params.FullScreen_RefreshRateInHz = displayModes[currentMode].dwFreq;
	params.BackBufferFormat = D3DFMT_X8R8G8B8;
    params.BackBufferCount = 1;
    params.EnableAutoDepthStencil = FALSE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	LPDIRECT3DDEVICE8 d3dDevice;
    if (FAILED(d3d->CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &d3dDevice)))
	{
		utils::debugPrint("Failed to create device\n");
        return false;
	}
	context::setD3dDevice(d3dDevice);
    
    context::getD3dDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER);
	context::getD3dDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	context::getD3dDevice()->SetVertexShader(D3DFVF_CUSTOMVERTEX);
	context::getD3dDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	context::getD3dDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	context::getD3dDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	context::getD3dDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	context::getD3dDevice()->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	context::getD3dDevice()->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	context::getD3dDevice()->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	context::getD3dDevice()->BeginScene();
	context::getD3dDevice()->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L);
	context::getD3dDevice()->EndScene();
	context::getD3dDevice()->Present(NULL, NULL, NULL, NULL);

	return true;
}

void render_sphere(float angle, utils::dataContainer* sphereMesh)
{
    const float deg_to_rad = 0.01745329252f;

    D3DXMATRIX matWorld;
    D3DXMatrixRotationY(&matWorld, -angle * deg_to_rad);

    D3DXVECTOR3 cameraPosition(0.0f, 0.1f, 8.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 7.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);   

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &cameraPosition, &target, &up);

    D3DXMATRIX matProjection;
    D3DXMatrixIdentity(&matProjection);
    D3DXMatrixPerspectiveFovLH(&matProjection, 45 * deg_to_rad, (float)context::getBufferWidth() / (float)context::getBufferHeight(), 0.1f, 100.0f);
   
    context::getD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld);
    context::getD3dDevice()->SetTransform(D3DTS_VIEW, &matView);
    context::getD3dDevice()->SetTransform(D3DTS_PROJECTION, &matProjection);

    context::getD3dDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff004000);
    context::getD3dDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    context::getD3dDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID); 

    image* imageToDraw = drawing::getImage("base");
    context::getD3dDevice()->SetTexture(0, imageToDraw->texture);
    context::getD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (sphereMesh->size / sizeof(meshUtility::vertex)) / 3, sphereMesh->data, sizeof(meshUtility::vertex));
}

void update_scene()
{
	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		utils::ReturnToDashboard();
		return;
	}

	network::init();
}

void render_scene()
{
    // Setup Ortho Camera 

    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity(&matIdentity);

    D3DXMATRIX matProjection;
	D3DXMatrixOrthoOffCenterLH(&matProjection, 0, (float)context::getBufferWidth(), 0, (float)context::getBufferHeight(), 1.0f, 800.0f);

    context::getD3dDevice()->SetTransform(D3DTS_VIEW, &matIdentity);
    context::getD3dDevice()->SetTransform(D3DTS_WORLD, &matIdentity);
    context::getD3dDevice()->SetTransform(D3DTS_PROJECTION, &matProjection);
    context::getD3dDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    context::getD3dDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    // Render Menu

	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);

    int32_t yPos = (context::getBufferHeight() - (3 * 40) - 10) / 2;
    yPos += theme::getCenterOffset();

    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "Welcome to UIX Lite FTPd. Powered by PrometheOS.", theme::getFooterTextColor(), horizAlignmentCenter, 193, yPos, 322);

    yPos += 40;

    component::button(true, false, "Return to Dashboard", 193, yPos, 322, 30);

	char* currentIp = context::getCurrentIp();
    char* ip = stringUtility::formatString("IP: %s - Username: xbox - Password: xbox", currentIp);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), ip, theme::getFooterTextColor(), horizAlignmentCenter, 193, theme::getFooterY(), 322);
	free(ip);
	free(currentIp);
}

void refreshInfo()
{
	if (network::isReady() == true)
	{
		XNADDR addr;
		memset(&addr, 0, sizeof(addr));
		DWORD dwState = XNetGetTitleXnAddr(&addr);
		if (dwState != XNET_GET_XNADDR_PENDING)
		{
			char* ipAddress = (XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE) ? stringUtility::formatString("%i.%i.%i.%i", 
				addr.ina.S_un.S_un_b.s_b1,
				addr.ina.S_un.S_un_b.s_b2,
				addr.ina.S_un.S_un_b.s_b3,
				addr.ina.S_un.S_un_b.s_b4) : strdup("0.0.0.0");
			char* currentIp = context::getCurrentIp();
			if (stringUtility::equals(ipAddress, currentIp, false) == false)
			{
				context::setCurrentIp(ipAddress);
			}
			free(ipAddress);
			free(currentIp);
		}
	}
}

void __cdecl main()
{
	utils::debugPrint("Welcome to PrometheOS Launcher\n");

	bool deviceCreated = createDevice();

	context::setNetworkInitialized(false);

	driveManager::mountDrive("HDD0-C");
	driveManager::mountDrive("HDD0-E");
	
	context::setImageMap(new pointerMap<image*>(true));
	theme::loadSkin();

	if (deviceCreated == false)
	{
		network::init();
	}

	drawing::loadFont(&font_sfn[0]);

	bitmapFont* fontSmall = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 18, 18, 0, 256);
	context::setBitmapFontSmall(fontSmall);
	bitmapFont* fontMedium = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 25, 25, 0, 256);
	context::setBitmapFontMedium(fontMedium);
	bitmapFont* fontLarge = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 32, 32, 0, 512);
	context::setBitmapFontLarge(fontLarge);

	drawing::renderRoundedRect("panel-fill", 24, 24, 6, 0xffffffff, 0x00000000, 0);
	drawing::renderRoundedRect("panel-stroke", 24, 24, 6, 0x01010100, 0xffffffff, 2);

    utils::dataContainer* sphereMesh = meshUtility::createSphere();
    drawing::loadImage((char*)base_jpg, sizeof(base_jpg), "base");

    audioPlayer::init();
    audioPlayer::play();
    
    float angle = 0;
    while (TRUE)
    {
		inputManager::processController();
        refreshInfo();

		drawing::clearBackground((uint32_t)0);

        // Render Sphere Background

        render_sphere(angle, sphereMesh); 

        // Process Input / Render Menu

		update_scene();
		render_scene();

        // Present Rendered Results

		context::getD3dDevice()->EndScene();
		context::getD3dDevice()->Present(NULL, NULL, NULL, NULL);

        angle += 0.05f;
        if (angle > 360.0f)
        {
            angle -= 360.f;
        }
    }
}