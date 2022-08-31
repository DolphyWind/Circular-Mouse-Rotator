#pragma once
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <sstream>
using namespace std;
const int bits_per_pixel = 24;

template <typename T>
string toString(T arg)
{
    stringstream ss;
    ss << arg;
    return ss.str();
}

HBITMAP CaptureScreen()
{
    // get the device context of the screen
    HDC hScreenDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
    // and a device context to put it in
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    int width = GetDeviceCaps(hScreenDC, HORZRES);
    int height = GetDeviceCaps(hScreenDC, VERTRES);

    // maybe worth checking these are positive values
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

    // get a new bitmap
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    CURSORINFO cursor = { sizeof(cursor) };
    ::GetCursorInfo(&cursor);
    if (cursor.flags == CURSOR_SHOWING) {
        RECT rcWnd;
        ::GetWindowRect(GetDesktopWindow(), &rcWnd);
        ICONINFOEXW info = { sizeof(info) };
        ::GetIconInfoExW(cursor.hCursor, &info);
        const int x = cursor.ptScreenPos.x - rcWnd.left - rcWnd.left - info.xHotspot;
        const int y = cursor.ptScreenPos.y - rcWnd.top - rcWnd.top - info.yHotspot;
        BITMAP bmpCursor = { 0 };
        ::GetObject(info.hbmColor, sizeof(bmpCursor), &bmpCursor);
        ::DrawIconEx(hMemoryDC, x, y, cursor.hCursor, bmpCursor.bmWidth, bmpCursor.bmHeight,
            0, NULL, DI_NORMAL);
    }

    hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);

    // clean up
    DeleteDC(hMemoryDC);
    DeleteDC(hScreenDC);
    return hBitmap;
}

bool SFMLLoadHBitmapAsImage(HBITMAP hBitmap, sf::Image* pPicture)
{
    //Create a DC to get hBitmap information
    HDC hDC = GetDC(::GetDesktopWindow());

    //Create BITMAPINFO variable, set size 
    BITMAPINFO MyBMInfo = { 0 };
    MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

    //Get the BITMAPINFO structure from the bitmap
    if (0 == GetDIBits(hDC, hBitmap, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS))
    {
        // error handling
        return false;
    }

    //Create the bitmap pixel array each element is [b,g,r]
    BYTE* lpPixels = new BYTE[MyBMInfo.bmiHeader.biSizeImage];

    //Setting up the structure of the buffer to be received
    MyBMInfo.bmiHeader.biCompression = BI_RGB;  // No-compression

   //Now get the actual data from the picture
    if (0 == GetDIBits(hDC, hBitmap, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)lpPixels, &MyBMInfo, DIB_RGB_COLORS))
    {
        // error handling
        return false;
    }

    //Now create an array of SFML pixels we want to fill
    sf::Uint8* lpPixelWithAlpha = new sf::Uint8[MyBMInfo.bmiHeader.biSizeImage +
        (MyBMInfo.bmiHeader.biSizeImage / 3) / 3]; //Add room for alpha

  //Loop through each pixel, with steps of four RGBA!
    for (int x = 0; x < MyBMInfo.bmiHeader.biSizeImage; x += 4)
    {
        lpPixelWithAlpha[x] = lpPixels[x + 2];    //lpPixels = r
        lpPixelWithAlpha[x + 1] = lpPixels[x + 1];    //lpPixels = g
        lpPixelWithAlpha[x + 2] = lpPixels[x];    //lpPixels = b
        lpPixelWithAlpha[x + 3] = 255;        //Nada alpha (just to adjust if you like)
    }

    //Remove old DIBsection
    delete[] lpPixels;

    //Load picture, now with correct pixels and alpha channel
    pPicture->create(MyBMInfo.bmiHeader.biWidth, MyBMInfo.bmiHeader.biHeight, lpPixelWithAlpha);
    pPicture->flipVertically();

    //Remove the pixels with alphachannel
    delete[] lpPixelWithAlpha;

    //Release the DC
    ReleaseDC(::GetDesktopWindow(), hDC);

    //Notify ok!
    return true;
}

template <typename T>
unsigned int setBetween(T num, int min, int max)
{
    if (num < min) num = min;
    else if (num > max) num = max;
    return num;
}