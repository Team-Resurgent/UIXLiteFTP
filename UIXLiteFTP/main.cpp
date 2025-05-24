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
#include "xbeParser.h"
#include "xunzip.h"
#include "IniUtility.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

char* StatusMsg = NULL;
bool msgAutoClear = false;
bool indexed = false;
DWORD StatusSetTime = 0;
DWORD menuNavDelay = 0;
HANDLE hThread = NULL;
void updateStatusMsg(char* msg,bool autoClear);
void clearStatusMsg();
void DownloadCallback(char* StatusMsg);
void refreshicons();
bool isBusy(HANDLE* hThreadPtr);

typedef struct MenuEntry {
    const char* label;
    void (*action)();
    struct MenuEntry* subMenu;
    int subMenuSize;
} MenuEntry;

char* appendNumber(const char* original, int number);
void enterSubMenu(MenuEntry* subMenu, int size);
void goBack();

MenuEntry* currentMenu = NULL;
int currentMenuSize = 0;
int mSelectedControl = 0;

#define MAX_MENU_DEPTH 5
MenuEntry* menuStack[MAX_MENU_DEPTH];
int menuSizeStack[MAX_MENU_DEPTH];
int menuDepth = 0;

char* appendNumber(const char* original, int number) {
    // Buffer to hold the integer as a string
    char numberStr[20];
    std::sprintf(numberStr, "%d", number);

    // Calculate total length
    size_t originalLen = std::strlen(original);
    size_t numberLen = std::strlen(numberStr);
    size_t totalLen = originalLen + numberLen;

    // Allocate memory for the new string
    char* result = new char[totalLen + 1]; // +1 for null terminator

    // Copy original and append number
    std::strcpy(result, original);
    std::strcat(result, numberStr);

    return result;
}

void DownloadCallback(void* data) {
	char* msg = strdup(reinterpret_cast<char*>(data));
	bool autoClear = strstr(msg,"Downloaded") == NULL;
	updateStatusMsg(msg,autoClear);
}

bool isBusy(HANDLE* hThreadPtr){
    DWORD exitCode;
    if (hThreadPtr == NULL) return false;
    if (GetExitCodeThread(*hThreadPtr, &exitCode)){
        if (exitCode == STILL_ACTIVE) {
            return true;
        } else {
		    CloseHandle(*hThreadPtr);
            *hThreadPtr = NULL;
            return false;
        }
    } else {
        return false;
    }
}
void enterSubMenu(MenuEntry* subMenu, int size) {
    if (menuDepth < MAX_MENU_DEPTH) {
        menuStack[menuDepth] = currentMenu;
        menuSizeStack[menuDepth] = currentMenuSize;
        menuDepth++;
        currentMenu = subMenu;
        currentMenuSize = size;
        mSelectedControl = 0;
    }
}

void action_downloadUIX() {
    if(isBusy(&hThread)) return;
    updateStatusMsg(strdup("Downloading UIX Lite..."),false);
	hThread = socketUtility::downloadFile("milenko.org", "/uix-lite/uix-lite-latest.zip", "HDD0-E:\\uix-lite-latest.zip", DownloadCallback);
}

void action_downloadFonts() {
    if(isBusy(&hThread)) return;
    updateStatusMsg(strdup("Downloading Fonts..."),false);
    hThread = socketUtility::downloadFile("milenko.org", "/uix-lite/uix-lite-fonts.zip", "HDD0-E:\\uix-lite-fonts.zip", DownloadCallback);
}

void action_downloadAudio() {
    if(isBusy(&hThread)) return;
    updateStatusMsg(strdup("Downloading Audio..."),false);
	hThread = socketUtility::downloadFile("milenko.org", "/uix-lite/uix-lite-audio.zip", "HDD0-E:\\uix-lite-audio.zip", DownloadCallback);
}

void action_refreshIcons();
void action_installFromZip();
void action_installFonts();
void action_installAudio();
void action_exit() { utils::ReturnToDashboard(); }

MenuEntry submenu_downloads[] = {
    { "Download UIX Lite", action_downloadUIX, NULL, 0 },
    { "Download Fonts", action_downloadFonts, NULL, 0 },
    { "Download Audio", action_downloadAudio, NULL, 0 },
};

MenuEntry submenu_installs[] = {
    { "Install UIX Lite", action_installFromZip, NULL, 0 },
    { "Install Fonts", action_installFonts, NULL, 0 },
    { "Install Audio", action_installAudio, NULL, 0 },
};
MenuEntry submenu_manageUIX[] = {
	{ "Refresh Icons", action_refreshIcons, NULL, 0 },
    { "Downloads", NULL, submenu_downloads, sizeof(submenu_downloads) / sizeof(MenuEntry) },
    { "Install", NULL, submenu_installs, sizeof(submenu_installs) / sizeof(MenuEntry) },
};

MenuEntry mainMenu[] = {
    { "Manage UIX Lite", NULL, submenu_manageUIX, sizeof(submenu_manageUIX)/sizeof(MenuEntry) },
    { "Return to Dashboard", action_exit, NULL, 0 }
};

// Quick implementation of CrunchBite's xunzip library
void installFromZip();
void installFromZip(){
    bool success = xExtractZip("HDD0-E:\\uix-lite-latest.zip", "HDD0-C:\\", true, true);
    updateStatusMsg(strdup(success ? "UIX installed to C:\\" : "Install failed."),true);
}
void action_installFromZip() {
    if (isBusy(&hThread)) return; 
    updateStatusMsg(strdup("Installing UIX..."),false);
    hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)installFromZip, 0, 0, NULL);
}
void installFonts();
void installFonts(){
    bool success = xExtractZip("HDD0-E:\\uix-lite-fonts.zip", "HDD0-C:\\", true, true);
    updateStatusMsg(strdup(success ? "Fonts installed to C:\\" : "Font install failed."),true);
}
void action_installFonts() {
    if(isBusy(&hThread)) return;
    updateStatusMsg(strdup("Installing Fonts..."),false);
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)installFonts, 0, 0, NULL);
}

void installAudio();
void installAudio(){
    bool success = xExtractZip("HDD0-E:\\uix-lite-audio.zip", "HDD0-C:\\Audio\\", true, true);
    updateStatusMsg(strdup(success ? "Audio installed to C:\\" : "Audio install failed."),true);
}
void action_installAudio() {
    if(isBusy(&hThread)) return;
    updateStatusMsg(strdup("Installing Audio..."),false);
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)installAudio, 0, 0, NULL);
}

void scanForDefaultXBE(const char* basePath) {
    WIN32_FIND_DATAA findFileData;
	char searchPath[256];
	sprintf(searchPath, "%s\\*",basePath);

    HANDLE hFind = FindFirstFileA(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) return;
    
	do {
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
            continue;

		char fullPath[256];
		sprintf(fullPath, "%s\\%s", basePath, findFileData.cFileName);

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Check if default.xbe exists in this subdirectory
            bool Exists;
			char xbePath[MAX_PATH];

            sprintf(xbePath, "%s\\default.xbe", fullPath);
			fileSystem::fileExists(xbePath,Exists);

            if (Exists) {
                updateStatusMsg(strdup(xbePath),false);
                XBEParser parser;
                parser.LoadXBE(xbePath);
                uint32_t titleId = 0;
                char* titleName = NULL;
				if (parser.GetTitleName(titleName) && parser.GetTitleID(titleId)) {
                    char TitleId[9];
                    sprintf(TitleId, "%08x", titleId);
                    
                    // Update Iconss.ini
                    if (!IniUtility::SetValue("HDD0-C:\\UIX Configs\\Icons.ini","default",findFileData.cFileName,TitleId)) {
                        updateStatusMsg(strdup("Failed to write Icons.ini"),true);
                        return;
                    }

                    // Update TitleNames.ini
                    char* existingTitle = IniUtility::GetValue("HDD0-C:\\UIX Configs\\TitleNames.ini", "default", findFileData.cFileName);
                    if (strlen(existingTitle) == 0) {
                        IniUtility::SetValue("HDD0-C:\\UIX Configs\\TitleNames.ini","default",findFileData.cFileName,titleName);
                    }
                    free(existingTitle);
                } else {
                    updateStatusMsg(strdup("Failed to get TitleID and TitleName."),true);
                }
                free(titleName);
			}
        }
    } while (FindNextFileA(hFind, &findFileData));

    FindClose(hFind);
}
void refreshIcons(){
    IniUtility::DeleteSection("HDD0-C:\\UIX Configs\\Icons.ini","default");
    pointerVector<char*>* Locations = new pointerVector<char*>(true);
	int MaxLauncherItems = stringUtility::toInt(IniUtility::GetValue("HDD0-C:\\UIX Configs\\config.ini","LauncherMenu","MaxLauncherMenuItems"));
	if (MaxLauncherItems == 0) MaxLauncherItems = 8;
	for (int i = 0; i < MaxLauncherItems; i++)
	{
		char* value = IniUtility::GetValue("HDD0-C:\\UIX Configs\\config.ini","LauncherMenu",appendNumber("Path",i));
		if (*value != '\0') {
			pointerVector<char*>* Paths = stringUtility::split(value,";",true);
			for (size_t p = 0; p < Paths->count(); p++){
				Locations->add(strdup(Paths->get(p)));
			}
            delete Paths;
		}
        free(value);
	}
    
	pointerVector<char*>* drives = driveManager::getMountedDrives();
    updateStatusMsg(strdup("Refreshing icons..."),false);
	for (size_t d = 0; d < drives->count(); d++)
	{
		//Skip the C:,X:,Y:,and Z: partitions
		if (strstr(drives->get(d), "C") ||
			strstr(drives->get(d), "X") ||
			strstr(drives->get(d), "Y") ||
			strstr(drives->get(d), "Z")
			) continue;
		for (size_t L = 0; L < Locations->count(); L++)
		{
			char searchPath[256];
			sprintf(searchPath, "%s:\\%s", drives->get(d),Locations->get(L));
			scanForDefaultXBE(searchPath);
		}
	}
    delete Locations; 
    delete drives;
	indexed = true;
    updateStatusMsg(strdup("Icons refreshed!"), true);
}

void action_refreshIcons() {
    if (isBusy(&hThread)) return;
	updateStatusMsg(strdup(indexed ? "Refreshing icons..." : "Indexing the extended partitions..."),false);
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)refreshIcons, 0, 0, NULL);
}

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

void update_scene() {
    if (inputManager::buttonPressed(ButtonA)) {
		if (GetTickCount() - menuNavDelay < 100) return; else menuNavDelay=GetTickCount();
		MenuEntry* entry = &currentMenu[mSelectedControl];
        if (entry->subMenu != NULL) {
            enterSubMenu(entry->subMenu, entry->subMenuSize);
        } else if (entry->action != NULL) {
            entry->action();
            return;
        }
    }

    if (inputManager::buttonPressed(ButtonB)) {
		if (GetTickCount() - menuNavDelay < 100) return; else menuNavDelay=GetTickCount();
        if (menuDepth > 0) {
			menuDepth--;
			currentMenu = menuStack[menuDepth];
			currentMenuSize = menuSizeStack[menuDepth];
			mSelectedControl = 0;
		}
	}

    if (inputManager::buttonPressed(ButtonDpadDown)) {
        if (GetTickCount() - menuNavDelay < 100) return; else menuNavDelay=GetTickCount();
        mSelectedControl = (mSelectedControl + 1) % currentMenuSize;
    }

    if (inputManager::buttonPressed(ButtonDpadUp)) {
        if (GetTickCount() - menuNavDelay < 100) return; else menuNavDelay=GetTickCount();
        mSelectedControl = (mSelectedControl - 1 + currentMenuSize) % currentMenuSize;
    }

    isBusy(&hThread);
    clearStatusMsg();
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

    // Render Menu Panel
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);

    int menuItemCount = currentMenuSize;
	int32_t yPos = (context::getBufferHeight() - (menuItemCount * 40) - 10) / 2;
    yPos += theme::getCenterOffset();

    drawing::drawBitmapStringAligned(
        context::getBitmapFontSmall(),
        "Welcome to UIX Lite FTPd. Powered by PrometheOS.",
        theme::getFooterTextColor(),
        horizAlignmentCenter,
        193,
        yPos,
        322
    );

    yPos += 40;

    for (int i = 0; i < menuItemCount; ++i) {
		component::button(mSelectedControl == i, false, currentMenu[i].label, 193, yPos, 322, 30);
		yPos += 40;
	}

	// Footer Info (IP + Status)
	char* currentIp = context::getCurrentIp();
	char* ip = stringUtility::formatString("IP: %s - Username: xbox - Password: xbox", currentIp);

	if (StatusMsg) {
		drawing::drawBitmapStringAligned(
			context::getBitmapFontSmall(),
			StatusMsg,
			theme::getFooterTextColor(),
			horizAlignmentCenter,
			193,
			theme::getFooterY() - 30,
			322
		);
	}

	drawing::drawBitmapStringAligned(
		context::getBitmapFontSmall(),
		ip,
		theme::getFooterTextColor(),
		horizAlignmentCenter,
		193,
		theme::getFooterY(),
		322
	);

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
void updateStatusMsg(char * msg, bool autoClear) {
	if (StatusMsg) free(StatusMsg);
	msgAutoClear = autoClear;
	StatusSetTime = GetTickCount();
	StatusMsg = msg;
}

void clearStatusMsg() {
	if (StatusMsg) {
		if (GetTickCount() - StatusSetTime > 4000 && msgAutoClear) {
			free(StatusMsg);
			StatusMsg = NULL;
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

    currentMenu = mainMenu;
	currentMenuSize = sizeof(mainMenu) / sizeof(MenuEntry);

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