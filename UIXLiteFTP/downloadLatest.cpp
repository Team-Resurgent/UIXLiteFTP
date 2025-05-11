#include <xtl.h>
#include <xbox.h>
#include "utils.h"
#include "stringUtility.h"
#include "socketUtility.h"

bool downloadZip(const char* path, const char* outputFile)
{
    const char* hostname = "milenko.org";

    WSAEVENT hEvent = WSACreateEvent();
    XNDNS* pDns = NULL;

    if (XNetDnsLookup(hostname, hEvent, &pDns) != 0 || WaitForSingleObject(hEvent, 5000) != WAIT_OBJECT_0 || !pDns || pDns->iStatus != 0)
    {
        if (pDns) XNetDnsRelease(pDns);
        return false;
    }

    IN_ADDR addr = pDns->aina[0];
    XNetDnsRelease(pDns);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    SOCKADDR_IN server = {};
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    server.sin_addr = addr;

    if (connect(sock, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        closesocket(sock);
        return false;
    }

    char request[512];
    sprintf(request,
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n",
        path, hostname);

    send(sock, request, strlen(request), 0);

    char buffer[4096];
    int bytes = 0;
    bool headersParsed = false;
    int httpStatus = 0;

    FILE* file = fopen(outputFile, "wb");
    if (!file)
    {
        closesocket(sock);
        return false;
    }

    while ((bytes = recv(sock, buffer, sizeof(buffer), 0)) > 0)
    {
        if (!headersParsed)
        {
            char* headerEnd = strstr(buffer, "\r\n\r\n");
            if (headerEnd)
            {
                char version[16] = { 0 };
                if (sscanf(buffer, "HTTP/%15s %d", version, &httpStatus) != 2 || httpStatus != 200)
                {
                    fclose(file);
                    remove(outputFile);
                    closesocket(sock);
                    return false;
                }

                int headerLen = headerEnd - buffer + 4;
                fwrite(headerEnd + 4, 1, bytes - headerLen, file);
                headersParsed = true;
            }
        }
        else
        {
            fwrite(buffer, 1, bytes, file);
        }
    }

    fclose(file);
    closesocket(sock);
    return true;
}

bool downloadLatestUIXZip() {
    return downloadZip("/uix-lite/uix-lite-latest.zip", "HDD0-E:\\uix-lite-latest.zip");
}

bool downloadFontsZip() {
    return downloadZip("/uix-lite/uix-lite-fonts.zip", "HDD0-E:\\uix-lite-fonts.zip");
}

bool downloadAudioZip() {
    return downloadZip("/uix-lite/uix-lite-audio.zip", "HDD0-E:\\uix-lite-audio.zip");
}