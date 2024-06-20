#include "MyImage.h"
#include "DSpriteManager.h"
#include <time.h>
#include <stdlib.h>

Graphics* g_BackBuffer = nullptr;
Graphics* g_MainBuffer = nullptr;
Bitmap* g_Bitmap = nullptr;
Pen* g_pPen = nullptr;

typedef struct _tgBackground
{
	Rect rect;
	int     interval;
	int     yOffset;
}BACKGROUND, *LPBACKGROUND;

typedef struct _tgScrollObject
{
	Rect rect;
	BOOL isDraw;
	int     yOffset;
}SCROLLOBJECT, *LPSCROLLOBJECT;

typedef struct _tgAniInfo
{
	Rect aniRect[3];
	int     aniTime[3];
	int     interval;
}ANIINFO, *LPANIINFO;

BACKGROUND g_background;
int     g_aniDirection = 0;
int     g_aniIndex = 2;
bool g_isMouseCapture = false;


SCROLLOBJECT  g_ScrollObjects[2];

ANIINFO g_Character;
CMyImage g_imgCharacter;

DWORD g_interval = 0;

CMyImage g_myImage;

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
const float GRAVITY = 9.8;
const float JUMP_INITIAL_SPEED = 15.0;
int characterX = 210; // ĳ������ �ʱ� X ��ǥ
int characterY = 650; // ĳ������ �ʱ� Y ��ǥ
const int CHARACTER_MOVE_SPEED = 5;

void UpdateGame(DWORD tick);
//void OnUpdate(HWND hWnd, DWORD tick);
void CreateBuffer(HWND hWnd, HDC hDC);
void ReleaseBuffer(HWND hWnd, HDC hDC);
void RenderGame(HWND hwnd);

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

	HWND hwnd = CreateWindow("myGame", "Game Window",
		WS_THICKFRAME | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX,
		100, 100, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		return 0;
	}

	HDC hDC = GetDC(hwnd);

	CreateBuffer(hwnd, hDC);

	g_pPen = new Pen(Color(255, 0, 0), 1.0f);

	g_myImage.Load("./Data/Image/background.png");

	int frame, type, left, top, width, height, interval;
	FILE* fp = fopen("img_ani.txt", "rt");
	fscanf(fp, "%d %d", &frame, &type);

	fscanf(fp, "%d %d %d %d %d", &left, &top, &width, &height, &interval);
	g_background.rect.X = left;
	g_background.rect.Y = top;
	g_background.rect.Width = width;
	g_background.rect.Height = height;
	g_background.interval = interval;
	g_background.yOffset = 0;

	fscanf(fp, "%d %d", &frame, &type);
	for (int i = 0; i < frame; i++)
	{
		fscanf(fp, "%d %d %d %d %d", &left, &top, &width, &height, &interval);
		g_ScrollObjects[i].rect.X = left;
		g_ScrollObjects[i].rect.Y = top;
		g_ScrollObjects[i].rect.Width = width;
		g_ScrollObjects[i].rect.Height = height;
		g_ScrollObjects[i].yOffset = -height;
		g_ScrollObjects[i].isDraw = FALSE;
	}

	g_imgCharacter.Load("./Data/Image/Character.png");

	fscanf(fp, "%d %d", &frame, &type);
	for (int i = 0; i < frame; i++)
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

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	srand(time(NULL));

	MSG msg;
	DWORD tick = GetTickCount();
	while (1)
	{
		//������ �޼����� ������� �޼����� ó���Ѵ�.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //�޼����� ���� ��� ���� ������ �����Ѵ�.
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
	g_BackBuffer = Graphics::FromImage(g_Bitmap);  // HDC���� �������� �ʰ�, Bitmap���� ����
	g_BackBuffer->SetPageUnit(Gdiplus::Unit::UnitPixel);

	g_MainBuffer = Graphics::FromHDC(hDC);  // HDC���� ����
	g_MainBuffer->SetPageUnit(Gdiplus::Unit::UnitPixel);
}

void ReleaseBuffer(HWND hWnd, HDC hDC)
{
	ReleaseDC(hWnd, hDC);

	delete g_Bitmap;
	delete g_BackBuffer;
	delete g_MainBuffer;
}

void RenderGame(HWND hwnd)
{
	// ��� �׸���
	int height = g_background.rect.Height - g_background.yOffset;

	g_myImage.Draw(g_BackBuffer, 0, g_background.yOffset,
		g_background.rect.X,
		g_background.rect.Y,
		g_background.rect.Width,
		height);

	if (g_background.yOffset > 0)
	{
		g_myImage.Draw(g_BackBuffer, 0, 0,
			g_background.rect.X,
			g_background.rect.Y + height,
			g_background.rect.Width,
			g_background.yOffset);
	}
// ��ũ�� ������Ʈ �׸���
	g_myImage.Draw(g_BackBuffer, 0, g_ScrollObjects[0].yOffset,
		g_ScrollObjects[0].rect.X,
		g_ScrollObjects[0].rect.Y,
		g_ScrollObjects[0].rect.Width,
		g_ScrollObjects[0].rect.Height);

	// ĳ���� �׸���
	g_imgCharacter.Draw(g_BackBuffer, characterX, characterY,
		g_Character.aniRect[0].X,
		g_Character.aniRect[0].Y,
		g_Character.aniRect[0].Width,
		g_Character.aniRect[0].Height);

	
	// ȭ�� ����
	HDC hdc = GetDC(hwnd);
	g_MainBuffer->DrawImage(g_Bitmap, 0, 0);  // g_MainBuffer�� ����Ͽ� g_Bitmap�� ȭ�鿡 �׸���
	ReleaseDC(hwnd, hdc);
}

void UpdateGame(DWORD tick)
{
	// ��� ������Ʈ
	int height = g_background.rect.Height - g_background.yOffset;

	g_myImage.Draw(g_BackBuffer, 0, g_background.yOffset,
		g_background.rect.X,
		g_background.rect.Y,
		g_background.rect.Width,
		height);

	if (g_background.yOffset > 0)
	{
		g_myImage.Draw(g_BackBuffer, 0, 0,
			g_background.rect.X,
			g_background.rect.Y + height,
			g_background.rect.Width,
			g_background.yOffset);
	}

	int offset = g_background.yOffset + (g_background.interval * tick / 1000);
	int extra = offset - g_background.rect.Height;
	if (extra >= 0)
		g_background.yOffset = extra;
	else
		g_background.yOffset = offset;

	// ĳ���� �̵� �� ���� ����
	if (isJumping)
	{
		characterY -= jumpSpeed;
		jumpSpeed -= GRAVITY * (tick / 1000.0);

		if (characterY >= 650)
		{
			characterY = 650;
			isJumping = false;
			jumpSpeed = 0;
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

	// ĳ���� �׸���
	g_imgCharacter.Draw(g_BackBuffer, characterX, characterY,
		g_Character.aniRect[0].X,
		g_Character.aniRect[0].Y,
		g_Character.aniRect[0].Width,
		g_Character.aniRect[0].Height);

	// ��� ��ũ��: ĳ������ Y��ǥ�� ���� ��� �̵�
	g_background.yOffset = 650 - characterY;

	// ��ũ�� ������Ʈ �׸���
	g_myImage.Draw(g_BackBuffer, 0, g_ScrollObjects[0].yOffset,
		g_ScrollObjects[0].rect.X,
		g_ScrollObjects[0].rect.Y,
		g_ScrollObjects[0].rect.Width,
		g_ScrollObjects[0].rect.Height);

	g_ScrollObjects[0].yOffset += (g_background.interval * tick / 1000);

	if (g_ScrollObjects[0].yOffset > g_background.rect.Height)
	{
		g_ScrollObjects[0].yOffset = -g_ScrollObjects[0].rect.Height;
		g_ScrollObjects[0].isDraw = FALSE;
	}
}

/*
void OnUpdate(HWND hWnd, DWORD tick)
{
	if(hWnd == NULL)
		return;

	Color color(255, 255, 255);
	g_BackBuffer->Clear(color);

	int rnd = rand() % 100;
	if ( (rnd > 97) && (g_ScrollObjects[0].isDraw == FALSE) )
	{
		g_ScrollObjects[0].isDraw = TRUE;
	}

	// ���� ���� �� �ڵ�
	int height = g_background.rect.Height - g_background.yOffset;

	g_myImage.Draw(g_BackBuffer, 0, g_background.yOffset,
		g_background.rect.X,
		g_background.rect.Y,
		g_background.rect.Width,
		height);

	if (g_background.yOffset > 0)
	{
		g_myImage.Draw(g_BackBuffer, 0, 0,
			g_background.rect.X,
			g_background.rect.Y + height,
			g_background.rect.Width,
			g_background.yOffset);
	}

	int offset = g_background.yOffset + (g_background.interval * tick / 1000);
	int extra = offset - g_background.rect.Height;
	if (extra >= 0)
		g_background.yOffset = extra;
	else
		g_background.yOffset = offset;

	// ĳ���� �̵� �� ���� ����
	if (isJumping)
	{
		characterY -= jumpSpeed;
		jumpSpeed -= GRAVITY * (tick / 1000.0);

		if (characterY >= 650)
		{
			characterY = 650;
			isJumping = false;
			jumpSpeed = 0;
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

	// ĳ���� �׸���
	g_imgCharacter.Draw(g_BackBuffer, characterX, characterY,
		g_Character.aniRect[0].X,
		g_Character.aniRect[0].Y,
		g_Character.aniRect[0].Width,
		g_Character.aniRect[0].Height);

	// ��� ��ũ��: ĳ������ Y��ǥ�� ���� ��� �̵�
	g_background.yOffset = 650 - characterY;

	// ��ũ�� ������Ʈ �׸���
	g_myImage.Draw(g_BackBuffer, 0, g_ScrollObjects[0].yOffset,
		g_ScrollObjects[0].rect.X,
		g_ScrollObjects[0].rect.Y,
		g_ScrollObjects[0].rect.Width,
		g_ScrollObjects[0].rect.Height);

	g_ScrollObjects[0].yOffset += (g_background.interval * tick / 1000);

	if (g_ScrollObjects[0].yOffset > g_background.rect.Height)
	{
		g_ScrollObjects[0].yOffset = -g_ScrollObjects[0].rect.Height;
		g_ScrollObjects[0].isDraw = FALSE;
	}


}
*/