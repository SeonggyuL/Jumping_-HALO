#include "MyImage.h"
#include<iostream>
#include <gdiplus.h>

using namespace Gdiplus;

CMyImage::CMyImage(void)
    : m_pBitmap(nullptr), m_pImage(nullptr), m_width(0), m_height(0)
{
}

CMyImage::~CMyImage(void)
{
    if (m_pBitmap)
    {
        delete m_pBitmap;
        m_pBitmap = nullptr;
    }

    if (m_pImage)
    {
        delete m_pImage;
        m_pImage = nullptr;
    }
}

void CMyImage::Load(const char* filePath)
{
    if (m_pBitmap)
    {
        delete m_pBitmap;
        m_pBitmap = nullptr;
    }

    WCHAR file[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, filePath, -1, file, MAX_PATH);

    m_pBitmap = Bitmap::FromFile(file);
    if (m_pBitmap && m_pBitmap->GetLastStatus() == Ok)
    {
        m_width = m_pBitmap->GetWidth();
        m_height = m_pBitmap->GetHeight();
    }
    else
    {
        printf("Failed to load image: %s\n", filePath);
        m_pBitmap = nullptr;
    }
}

void CMyImage::Draw(Graphics* g, int x, int y)
{
    if (m_pBitmap)
    {
        g->DrawImage(m_pBitmap, x, y);
    }
}

void CMyImage::DrawCenter(Graphics* g, int x, int y, int xCenter, int yCenter)
{
    if (m_pBitmap)
    {
        int drawX = x - xCenter;
        int drawY = y - yCenter;
        g->DrawImage(m_pBitmap, drawX, drawY);
    }
}

void CMyImage::Draw(Graphics* g, int x, int y, int width, int height)
{
    if (m_pBitmap)
    {
        g->DrawImage(m_pBitmap, x, y, width, height);
    }
}

void CMyImage::Draw()
{
    // Empty implementation
}

void CMyImage::Draw(Graphics* g, int dstX, int dstY, int srcX, int srcY, int width, int height)
{
    if (m_pBitmap)
    {
        // dstRect: (dstX, dstY, width, height)
        // srcRect: (srcX, srcY, width, height)
        Rect destRect(dstX, dstY, width, height);
        g->DrawImage(m_pBitmap, destRect, srcX, srcY, width, height, UnitPixel);
    }
}

int CMyImage::GetWidth() const
{
    if (m_pBitmap)
    {
        return m_pBitmap->GetWidth();
    }
    return 0;
}

int CMyImage::GetHeight() const
{
    if (m_pBitmap)
    {
        return m_pBitmap->GetHeight();
    }
    return 0;
}
