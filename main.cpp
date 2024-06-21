#include "MyImage.h"
#include "DSpriteManager.h"
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <string>

#define NUM_BLOCKS 10
#define NUM_BACKGROUNDS 3

Graphics* g_BackBuffer = nullptr;
Graphics* g_MainBuffer = nullptr;
Bitmap* g_Bitmap = nullptr;
Pen* g_pPen = nullptr;

CMyImage g_BackgroundImages[NUM_BACKGROUNDS]; // ���� ���� ����

typedef struct _tgBackground
{
    Rect rect;
    int interval;
    int yOffset;
} BACKGROUND, * LPBACKGROUND;

typedef struct _tgBlock
{
    Rect rect;
    bool isActive;
    CMyImage image;
} BLOCK, * LPBLOCK;

typedef struct _BlockData
{
    int left;
    int top;
    int width;
    int height;
    int interval;
    char imagePath[256];
} BlockData;

// ���� ������ �߰��մϴ�.
std::vector<std::vector<BlockData>> g_Stages;
int g_NumStages = 0;

BLOCK g_Blocks[NUM_BLOCKS];

typedef struct _tgAniInfo
{
    Rect aniRect[3];
    int aniTime[3];
    int interval;
} ANIINFO, * LPANIINFO;

BACKGROUND g_background;
int g_aniDirection = 0;
int g_aniIndex = 2;
bool g_isMouseCapture = false;

ANIINFO g_Character;
CMyImage g_imgCharacter;

DWORD g_interval = 0;

int g_BackgroundOffset = 0;
const int BACKGROUND_SCROLL_SPEED = 1; // ��ũ�� �ӵ��� �� ���߱� ���� ���� ������ ����
const int CHARACTER_FALL_SPEED = 5; // ĳ���Ͱ� �������� �ӵ�
int g_CurrentBackgroundIndex = 0;
bool isScrolling = true; // ��ũ�� ���¸� �����ϴ� ����

bool isFirstScroll = true; // ù ��ũ������ ���θ� �����ϴ� ����

#define MAX_FRAME 3
#define SCREEN_WIDTH 427
#define SCREEN_HEIGHT 750

struct VECTOR2
{
    float x;
    float y;
};

VECTOR2 g_vecDir = { 0.0f, 0.0f };

// ���� ����
bool isLeftPressed = false;
bool isRightPressed = false;
bool isUpPressed = false;
bool isJumping = false;
float jumpSpeed = 0;
const float GRAVITY = 9.8f;
const float JUMP_INITIAL_SPEED = 15.0f;
int characterX = 210; // ĳ������ �ʱ� X ��ǥ
int characterY = 650; // ĳ������ �ʱ� Y ��ǥ
const int CHARACTER_MOVE_SPEED = 5;

void UpdateGame(DWORD tick);
void CreateBuffer(HWND hWnd, HDC hDC);
void ReleaseBuffer(HWND hWnd, HDC hDC);
void RenderGame(HWND hwnd);
void InitializeBlocks(FILE* fp);
void InitializeStages(const char* filePath);
void RandomizeStage();
void InitializeBackgrounds();
void InitializeCharacterAnimation();
void UpdateBackgroundScroll();
void ResetToFirstStage();

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = NULL;
    wndclass.hCursor = NULL;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "myGame";

    if (RegisterClass(&wndclass) == 0)
    {
        return 0;
    }

    RECT rc = { 0, 0, 427, 750 };
    ::AdjustWindowRect(&rc,
        WS_THICKFRAME | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX,
        FALSE);

    HWND hwnd = CreateWindow("myGame", "Jump king",
        WS_THICKFRAME | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX,
        100, 100, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    HDC hDC = GetDC(hwnd);

    CreateBuffer(hwnd, hDC);

    g_pPen = new Pen(Color(255, 0, 0), 1.0f);

    InitializeBackgrounds();
    InitializeCharacterAnimation();
    InitializeStages("./Data/txt_data/stages.txt"); // �������� �ʱ�ȭ

    g_imgCharacter.Load("./Data/Image/Character.png");

    RandomizeStage(); // �ʱ� �������� ���� ���� �� ��� ��ġ

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    srand(time(NULL));

    MSG msg;
    DWORD tick = GetTickCount();
    while (1)
    {
        // ������ �޼����� ������� �޼����� ó���Ѵ�.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else // �޼����� ���� ��� ���� ������ �����Ѵ�.
        {
            DWORD curTick = GetTickCount();
            UpdateGame(curTick - tick); // UpdateGame �Լ� ȣ��
            tick = curTick;

            RenderGame(hwnd); // RenderGame �Լ� ȣ��

            // ���� ���� �ð� ���� (������ ����)
            Sleep(16); // �� 60 FPS
        }
    }

    delete g_pPen;
    ReleaseBuffer(hwnd, hDC);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_LEFT:
            isLeftPressed = true;
            break;
        case VK_RIGHT:
            isRightPressed = true;
            break;
        case VK_UP:
            isUpPressed = true;
            break;
        }
    }
    break;
    case WM_KEYUP:
    {
        switch (wParam)
        {
        case VK_LEFT:
            isLeftPressed = false;
            break;
        case VK_RIGHT:
            isRightPressed = false;
            break;
        case VK_UP:
            isUpPressed = false;
            break;
        }
    }
    break;
    case WM_CREATE:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void CreateBuffer(HWND hWnd, HDC hDC)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    RECT rc;
    BOOL res = GetClientRect(hWnd, &rc);

    g_Bitmap = new Bitmap(rc.right - rc.left, rc.bottom - rc.top);
    g_BackBuffer = Graphics::FromImage(g_Bitmap); // HDC���� �������� �ʰ�, Bitmap���� ����
    g_BackBuffer->SetPageUnit(Gdiplus::Unit::UnitPixel);

    g_MainBuffer = Graphics::FromHDC(hDC); // HDC���� ����
    g_MainBuffer->SetPageUnit(Gdiplus::Unit::UnitPixel);
}

void ReleaseBuffer(HWND hWnd, HDC hDC)
{
    ReleaseDC(hWnd, hDC);

    delete g_Bitmap;
    delete g_BackBuffer;
    delete g_MainBuffer;
}

void InitializeBlocks(FILE* fp)
{
    int frame, type, left, top, width, height, interval;
    char imagePath[256];

    fscanf(fp, "%d %d", &frame, &type);
    for (int i = 0; i < frame && i < NUM_BLOCKS; ++i)
    {
        fscanf(fp, "%d %d %d %d %d %s", &left, &top, &width, &height, &interval, imagePath);
        g_Blocks[i].rect.X = left;
        g_Blocks[i].rect.Y = top;
        g_Blocks[i].rect.Width = width;
        g_Blocks[i].rect.Height = height;
        g_Blocks[i].isActive = true;
        g_Blocks[i].image.Load(imagePath); // �� ����� �̹��� ��θ� ����Ͽ� �̹��� �ε�
    }
}

void InitializeStages(const char* filePath)
{
    FILE* fp = fopen(filePath, "rt");
    if (!fp)
    {
        printf("Failed to open stages file.\n");
        return;
    }

    fscanf(fp, "%d", &g_NumStages);
    g_Stages.resize(g_NumStages);

    for (int stage = 0; stage < g_NumStages; ++stage)
    {
        int frame, type;
        fscanf(fp, "%d %d", &frame, &type);

        for (int i = 0; i < frame; ++i)
        {
            BlockData block;
            fscanf(fp, "%d %d %d %d %d %s", &block.left, &block.top, &block.width, &block.height, &block.interval, block.imagePath);
            g_Stages[stage].push_back(block);
        }
    }

    fclose(fp);
}

void RandomizeStage()
{
    int randomStage = rand() % g_NumStages;
    std::vector<BlockData>& stage = g_Stages[randomStage];

    for (int i = 0; i < stage.size() && i < NUM_BLOCKS; ++i)
    {
        g_Blocks[i].rect.X = stage[i].left;
        g_Blocks[i].rect.Y = stage[i].top;
        g_Blocks[i].rect.Width = stage[i].width;
        g_Blocks[i].rect.Height = stage[i].height;
        g_Blocks[i].isActive = true;
        g_Blocks[i].image.Load(stage[i].imagePath); // �� ����� �̹��� ��θ� ����Ͽ� �̹��� �ε�
    }
}

void ResetToFirstStage()
{
    g_CurrentBackgroundIndex = 0;
    g_BackgroundOffset = 0;
    isScrolling = true;
    characterX = 210;
    characterY = 650;
    isJumping = false;
    jumpSpeed = 0;
    isFirstScroll = true;
    RandomizeStage(); // ó�� ���������� ���ư� ���� ���������� �����ϰ� ��ġ
}

void InitializeBackgrounds()
{
    std::ifstream infile("./Data/txt_data/backgrounds.txt");
    if (!infile.is_open())
    {
        printf("Failed to open backgrounds file.\n");
        return;
    }

    std::string line;
    int index = 0;

    while (std::getline(infile, line) && index < NUM_BACKGROUNDS)
    {
        g_BackgroundImages[index].Load(line.c_str()); // std::string�� const char*�� ��ȯ�Ͽ� Load �Լ��� ����
        index++;
    }
}

void InitializeCharacterAnimation()
{
    FILE* fp = fopen("./Data/txt_data/character_ani.txt", "rt");
    if (!fp)
    {
        printf("Failed to open character animation file.\n");
        return;
    }

    int frame, type, left, top, width, height, interval;
    fscanf(fp, "%d %d", &frame, &type);

    for (int i = 0; i < frame; ++i)
    {
        fscanf(fp, "%d %d %d %d %d", &left, &top, &width, &height, &interval);
        g_Character.aniRect[i].X = left;
        g_Character.aniRect[i].Y = top;
        g_Character.aniRect[i].Width = width;
        g_Character.aniRect[i].Height = height;
        g_Character.aniTime[i] = interval;
        g_Character.interval = 0;
    }

    fclose(fp);
}

void UpdateBackgroundScroll()
{
    if (isScrolling)
    {
        g_BackgroundOffset -= BACKGROUND_SCROLL_SPEED;
        if (g_BackgroundOffset <= -SCREEN_HEIGHT)
        {
            g_BackgroundOffset = 0;
            if (g_CurrentBackgroundIndex < NUM_BACKGROUNDS - 1)
            {
                g_CurrentBackgroundIndex++;
                RandomizeStage(); // ���������� �����ϰ� ����
            }
        }
    }
}

void RenderGame(HWND hwnd)
{
    // ȭ���� ������� ����ϴ�.
    g_BackBuffer->Clear(Color(255, 255, 255));

    // ��� �׸���
    int totalHeight = g_BackgroundOffset;
    for (int i = g_CurrentBackgroundIndex; i < NUM_BACKGROUNDS; ++i)
    {
        g_BackgroundImages[i].Draw(g_BackBuffer, 0, totalHeight, 0, 0, g_BackgroundImages[i].GetWidth(), g_BackgroundImages[i].GetHeight());
        totalHeight += g_BackgroundImages[i].GetHeight();
    }

    // ĳ���� �׸���
    g_imgCharacter.Draw(g_BackBuffer, characterX, characterY,
        g_Character.aniRect[0].X,
        g_Character.aniRect[0].Y,
        g_Character.aniRect[0].Width,
        g_Character.aniRect[0].Height);

    // ��� �׸���
    for (int i = 0; i < NUM_BLOCKS; ++i)
    {
        if (g_Blocks[i].isActive)
        {
            g_Blocks[i].image.Draw(g_BackBuffer, g_Blocks[i].rect.X, g_Blocks[i].rect.Y,
                g_Blocks[i].rect.Width, g_Blocks[i].rect.Height);
        }
    }

    // ȭ�� ����
    HDC hdc = GetDC(hwnd);
    g_MainBuffer->DrawImage(g_Bitmap, 0, 0); // g_MainBuffer�� ����Ͽ� g_Bitmap�� ȭ�鿡 �׸���
    ReleaseDC(hwnd, hdc);
}

void UpdateGame(DWORD tick)
{
    // ĳ���� �̵� �� ���� ����
    if (isJumping)
    {
        characterY -= jumpSpeed;
        jumpSpeed -= GRAVITY * (tick / 1000.0f);

        if (characterY >= 650)
        {
            characterY = 650;
            isJumping = false;
            jumpSpeed = 0;
        }

        // ĳ���Ͱ� ȭ�� �Ʒ��� ���������� Ȯ��
        if (characterY > SCREEN_HEIGHT)
        {
            ResetToFirstStage(); // ó�� ���������� �ǵ��ư���
        }
    }

    if (isLeftPressed)
    {
        characterX -= CHARACTER_MOVE_SPEED;
    }
    if (isRightPressed)
    {
        characterX += CHARACTER_MOVE_SPEED;
    }
    if (isUpPressed && !isJumping)
    {
        isJumping = true;
        jumpSpeed = JUMP_INITIAL_SPEED;
    }

    // ��� �浹 �� �ٴ� ����
    bool onBlock = false;
    for (int i = 0; i < NUM_BLOCKS; ++i)
    {
        if (g_Blocks[i].isActive)
        {
            // ��ϰ��� �浹�� Ȯ��
            if (characterX + g_Character.aniRect[0].Width > g_Blocks[i].rect.X &&
                characterX < g_Blocks[i].rect.X + g_Blocks[i].rect.Width &&
                characterY + g_Character.aniRect[0].Height > g_Blocks[i].rect.Y &&
                characterY + g_Character.aniRect[0].Height < g_Blocks[i].rect.Y + 20) // �ణ�� ������ �ΰ� ����
            {
                characterY = g_Blocks[i].rect.Y - g_Character.aniRect[0].Height;
                onBlock = true;
                isJumping = false;
                jumpSpeed = 0;
                break;
            }
        }
    }

    // ���� ĳ���Ͱ� � ��Ͽ��� �浹���� ������ ���� ���·� ����
    if (!onBlock && characterY < 650)
    {
        isJumping = true;
    }

    // ĳ���� Y ��ġ�� ���� ��� ��ũ��
    if (characterY < 200) // ĳ���Ͱ� ȭ���� ��ܿ� ������ ���
    {
        g_BackgroundOffset += BACKGROUND_SCROLL_SPEED;
        characterY += BACKGROUND_SCROLL_SPEED; // ĳ������ Y ��ġ�� �����Ͽ� ��� ȭ�鿡 �ְ� �մϴ�.
    }
    else if (characterY > SCREEN_HEIGHT - 200) // ĳ���Ͱ� ȭ���� �ϴܿ� ������ ���
    {
        g_BackgroundOffset -= BACKGROUND_SCROLL_SPEED;
        characterY -= BACKGROUND_SCROLL_SPEED; // ĳ������ Y ��ġ�� �����Ͽ� ��� ȭ�鿡 �ְ� �մϴ�.
    }

    // ��� ��ũ���� �Ѱ迡 �����ߴ��� Ȯ��
    if (g_BackgroundOffset >= SCREEN_HEIGHT)
    {
        g_BackgroundOffset = 0;
        if (g_CurrentBackgroundIndex < NUM_BACKGROUNDS - 1)
        {
            g_CurrentBackgroundIndex++;
            RandomizeStage(); // ���������� �����ϰ� ����
        }
    }

    // background1���� �� �̻� �Ʒ��� �������� �ʵ���
    if (g_CurrentBackgroundIndex == 0 && characterY > SCREEN_HEIGHT - g_Character.aniRect[0].Height)
    {
        characterY = SCREEN_HEIGHT - g_Character.aniRect[0].Height;
    }
}
