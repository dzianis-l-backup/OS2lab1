// OS2lab1.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "OS2lab1.h"
#include <vector>
#include <gdiplus.h>
#include <CommDlg.h>
#include <string>
#include <stdio.h>
#include <windows.h>
#include <ctime>

#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;
using namespace std;

#define MAX_LOADSTRING 100
bool bDrawLine = false;
bool bDrawEllipse = false;

// Глобальные переменные:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// text of header string
TCHAR szWindowClass[MAX_LOADSTRING];			// the name of window main class

///
///User's global variables
///
bool isPen;
bool isLine;
bool isEllipse;
bool isRectangle;
static int f=0;


int xLBDPos;
int yLBDPos;
int id;
bool isPainting;
static HBITMAP hCompatibleBitmap, hBitmap;
static HDC hCompatibleDC = 0;
static HDC hBitmapDC;//
RECT rect;//client rectangle
static HDC hdc,hdc1,hdc2;//hdc - drawing in memory, hdc1 - for metafile, hdc2 - for drawing on real display
static HBRUSH hBrush;
static HPEN hPen;
TEXTMETRIC tm;//text settings
static int cxChar, cyChar;

static OPENFILENAME ofn;
static char fullpath[256], filename[256], dir[256];
HENHMETAFILE hEnhMtf;
ENHMETAHEADER emh;

static double scale;
static int xBegin = 0, yBegin = 0, fl = 0;
RECT rect1;

static int width = 0;
static CHOOSECOLOR cc1, cc2;//1-st - for color pen, 2-nd - for color fill
static COLORREF  crCustColor[16];
static bool b, bPoly, bText, flag, bPolyline;
static int xT, yT;
static string text;
int zDelta;
static int xx1, yy1;
HDC hdcMem;
HBITMAP hbmMem;
HANDLE hOld;
static bool bScaled = FALSE;

//prototypes list
void create(HWND h);
void Save(HWND h);
void Open(HWND h);
void setWidth(int w);
void colPen(HWND h);
void colFill(HWND h);
void New(HWND h);
void Id(HWND h, int i, int s, int x, int y, bool pol, bool line);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OS2LAB1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OS2LAB1));

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OS2LAB1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OS2LAB1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	
	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

/////////////////////////////////////////////
//methods here
VOID OnPaint(HDC hdc)//line drawn
{
	Graphics graphics(hdc);
	Pen      pen(Color(255, 0, 0, 255));
	graphics.DrawLine(&pen, 0, 0, 200, 100);
}



//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	- закрасить главное окно
//  WM_DESTROY	- ввести сообщение о выходе и вернуться.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;		// paint data for Begin/EndPaint  
	// handle to device context (DC)  
	////////////////////////////////////////////	
	///
	///temporary variables
	///
	int static x0, y0, x1, y1, x2, y2, oldMixMode;

	switch (message)
	{

	case WM_CREATE:
		create(hWnd);
		break;
	
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		//variables scope

		
		bText = false;
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		//ellipse rect roundRect line

		case IDM_PEN:
			Id(hWnd, IDM_PEN, 1, 0, 0, false, false);
			break;
		case IDM_LINE:
			Id(hWnd, IDM_LINE, 1, 0, 0, false, false);
			break;
		case IDM_ELLIPSE:
			Id(hWnd, IDM_ELLIPSE, 1, 0, 0, false, false);
			break;
		case IDM_RECTANGLE:
			Id(hWnd, IDM_RECTANGLE, 1, 0, 0, false, false);
			break;
		case IDM_POLYLINE:
			Id(hWnd, IDM_POLYLINE, 1, 0, yBegin, false, bPolyline);
			break;
		case IDM_POLYGON:
			Id(hWnd, IDM_POLYGON, 1, 0, 0, bPoly, false);
			break;
		case IDM_TEXT:
			Id(hWnd, IDM_TEXT, 1, 0, 0, false, false);
			break;
		case IDM_ZOOM:
			id = IDM_ZOOM;
			bScaled = TRUE;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case IDM_PAN:
			id = IDM_PAN;
			break;

		case IDM_OPEN:
			Open(hWnd);
			break;
		case IDM_SAVE:
			Save(hWnd);
			break;


		case IDM_1:
			setWidth(1);
			break;
		case IDM_2:
			setWidth(2);
			break;
		case IDM_3:
			setWidth(3);
			break;
		case IDM_4:
			setWidth(4);
			break;
		case IDM_5:
			setWidth(5);
			break;
		case IDM_6:
			setWidth(6);
			break;
		case IDM_7:
			setWidth(7);
			break;
		case IDM_8:
			setWidth(8);
			break;
		case IDM_9:
			setWidth(9);
			break;
		case IDM_10:
			setWidth(10);
			break;


		case IDM_COLORPEN:
			colPen(hWnd);
			break;
		case IDM_COLORFILL:
			colFill(hWnd);
			break;
		case IDM_COLORNONE:
			DeleteObject(hBrush);
			hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			DeleteObject(SelectObject(hCompatibleDC, hBrush));
			DeleteObject(SelectObject(hBitmapDC, hBrush));
			DeleteObject(SelectObject(hdc1, hBrush));
			break;

		case IDM_NEW:
			New(hWnd);
			break;
			
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;	

	case WM_PAINT:
		// TODO: добавьте любой код отрисовки...
		hdc2 = BeginPaint(hWnd, &ps);
		switch (id)
		{
		case IDM_ZOOM:
		case IDM_PAN:
		{
			GetClientRect(hWnd, &rect);
			hdcMem = CreateCompatibleDC(hdc);
			hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			hOld = SelectObject(hdcMem, hbmMem);
			FillRect(hdcMem, &rect, WHITE_BRUSH);
			StretchBlt(hdcMem, 0, 0, (int)(rect.right*scale), (int)(rect.bottom*scale),
				hBitmapDC, xBegin, yBegin, rect.right, rect.bottom, SRCCOPY);
			SelectObject(hdcMem, (HBRUSH)GetStockObject(NULL_BRUSH));
			SelectObject(hdcMem, (HPEN)GetStockObject(BLACK_PEN));
			Rectangle(hdcMem, 0, 0, (int)(rect.right*scale), (int)(rect.bottom*scale));
			BitBlt(hdc2, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
			SelectObject(hdcMem, hOld);
			DeleteObject(hbmMem);
			DeleteDC(hdcMem);
			break;
		}
			default:
			{
				GetClientRect(hWnd, &rect);
				if (f == 0)
				{
					BitBlt(hdc2, 0, 0, rect.right, rect.bottom, hBitmapDC, 0, 0, SRCCOPY);
				}
				if (f == 1)
				{
					BitBlt(hdc2, 0, 0, rect.right, rect.bottom, hBitmapDC, 0, 0, SRCCOPY);
					f = 0;
				}
				if (f == 2)
				{
					BitBlt(hdc2, 0, 0, rect.right, rect.bottom, hCompatibleDC, 0, 0, SRCCOPY);
					f = 0;
				}
				break;
			}
		}
		EndPaint(hWnd, &ps);
		break;



	//case WM_SIZE:

	case WM_LBUTTONDOWN:
		if (id == IDM_ZOOM)
			id = IDM_PAN;
		switch (id){
			case IDM_PEN:
			case IDM_ELLIPSE:
			case IDM_LINE:
			case IDM_RECTANGLE:
				x1 = x2 = (short)LOWORD(lParam);
				y1 = y2 = (short)HIWORD(lParam);
				
				
				break;

			case IDM_POLYLINE:
			case IDM_POLYGON:
				if (bPolyline == true)
					bPolyline = false;
				if (bPoly == false)
				{
					x0 = x1 = x2 = (short)LOWORD(lParam);
					y0 = y1 = y2 = (short)HIWORD(lParam);
					bPoly = true;
				}
				
				break;
			case IDM_TEXT:
				bText = true;
				break;
			case IDM_PAN:
				x1 = x2 = (short)((short)LOWORD(lParam) / scale);
				y1 = y2 = (short)((short)HIWORD(lParam) / scale);
				break;
		}		
		SetCapture(hWnd);
		//isPainting = true;
		b = true;
		break;

	case WM_MOUSEMOVE:
		GetClientRect(hWnd, &rect);		
		if (b && (bPoly == false) && bText == false)
		{
			hCompatibleBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);//bitmap created
			DeleteObject(SelectObject(hCompatibleDC, hCompatibleBitmap));//created when created
			BitBlt(hCompatibleDC, 0, 0, rect.right, rect.bottom, hBitmapDC, 0, 0, SRCCOPY);
			switch (id)
			{
				case IDM_PEN:
					x2 = LOWORD(lParam);
					y2 = HIWORD(lParam);
					MoveToEx(hBitmapDC, x1, y1, NULL);
					LineTo(hBitmapDC, x2, y2);
					
					MoveToEx(hdc1, x1, y1, NULL);
					LineTo(hdc1, x2, y2);

					MoveToEx(hCompatibleDC, x1, y1, NULL);
					LineTo(hCompatibleDC, x2, y2);

					
					
					x1 = x2;
					y1 = y2;
					break;
				case IDM_LINE:
					x2 = (short)LOWORD(lParam);
					y2 = (short)HIWORD(lParam);
					MoveToEx(hCompatibleDC, x1, y1, NULL);
					LineTo(hCompatibleDC, x2, y2);
					break;
				case IDM_ELLIPSE:
					x2 = (short)LOWORD(lParam);
					y2 = (short)HIWORD(lParam);
					Ellipse(hCompatibleDC, x1, y1, x2, y2);
					break;
				case IDM_RECTANGLE:
					x2 = (short)LOWORD(lParam);
					y2 = (short)HIWORD(lParam);
					Rectangle(hCompatibleDC, x1, y1, x2, y2);
					break;
				case IDM_PAN:
					x1 = (short)((short)LOWORD(lParam) / scale);
					y1 = (short)((short)HIWORD(lParam) / scale);
					xBegin += (x2 - x1);
					yBegin += (y2 - y1);
					x2 = x1;
					y2 = y1;
					break;
				
				
				
			}
			f = 2;
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);


		}
		if (b&&bPoly&&bText == false)
		{
			hCompatibleBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			DeleteObject(SelectObject(hCompatibleDC, hCompatibleBitmap));
			BitBlt(hCompatibleDC, 0, 0, rect.right, rect.bottom, hBitmapDC, 0, 0, SRCCOPY);
			x2 = (short)LOWORD(lParam);
			y2 = (short)HIWORD(lParam);
			MoveToEx(hCompatibleDC, x1, y1, NULL);
			LineTo(hCompatibleDC, x2, y2);
			f = 2;
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
		}
		break;


		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		if (bText)
		{
			xT = (short)LOWORD(lParam);
			yT = (short)HIWORD(lParam);
			text.clear();
			b = false;
			break;
		}
		GetClientRect(hWnd, &rect);
		hCompatibleBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		DeleteObject(SelectObject(hCompatibleDC, hCompatibleBitmap));
		BitBlt(hCompatibleDC, 0, 0, rect.right, rect.bottom, hBitmapDC, 0, 0, SRCCOPY);
		if (b && (bPoly == false))
		{
			x2 = (short)LOWORD(lParam);
			y2 = (short)HIWORD(lParam);
			switch (id)
			{
			case IDM_PEN:
				
			case IDM_LINE:
				MoveToEx(hdc1, x1, y1, NULL);
				LineTo(hdc1, x2, y2);
				MoveToEx(hBitmapDC, x1, y1, NULL);
				LineTo(hBitmapDC, x2, y2);
				break;
			case IDM_ELLIPSE:
				Ellipse(hdc1, x1, y1, x2, y2);
				Ellipse(hBitmapDC, x1, y1, x2, y2);
				break;
			case IDM_RECTANGLE:
				Rectangle(hdc1, x1, y1, x2, y2);
				Rectangle(hBitmapDC, x1, y1, x2, y2);
				break;
			}
			f = 1;
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
			b = false;
		}
		if (b&&bPoly)
		{
			MoveToEx(hdc1, x1, y1, NULL);
			MoveToEx(hBitmapDC, x1, y1, NULL);
			x2 = (short)LOWORD(lParam);
			y2 = (short)HIWORD(lParam);
			LineTo(hdc1, x2, y2);
			LineTo(hBitmapDC, x2, y2);
			x1 = x2;
			y1 = y2;
			b = false;
			f = 1;
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
		}
		break;

	case WM_RBUTTONUP:
		if (bPoly)
		{
			x2 = (short)LOWORD(lParam);
			y2 = (short)HIWORD(lParam);
			MoveToEx(hdc1, x1, y1, NULL);
			LineTo(hdc1, x2, y2);
			MoveToEx(hBitmapDC, x1, y1, NULL);
			LineTo(hBitmapDC, x2, y2);
			if (id == IDM_POLYGON)
			{
				LineTo(hdc1, x0, y0);
				LineTo(hBitmapDC, x0, y0);
			}
			bPoly = false;
			bPolyline = false;
			f = 1;
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
		}


	case WM_MOUSEWHEEL:
		bText = false;
		if (id != IDM_ZOOM&&id != IDM_PAN)
		{
			scale = 1;
			id = IDM_PAN;
		}
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if ((zDelta>0) && (scale<3))
			scale = scale + 0.03;
		if ((zDelta<0) && (scale>0.3))
			scale = scale - 0.03;
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
		break;


	case  WM_CHAR:
		if (bText)
		{
			char c = (char)wParam;
			if (c == VK_RETURN)
				bText = false;
			else
				text += c;
			TextOut(hBitmapDC, xT, yT,text.c_str(), strlen(text.c_str()));
			TextOut(hdc1, xT, yT, text.c_str(), strlen(text.c_str()));
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
		}
		break;




	case WM_DESTROY:
		
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void TurrnOffBoolean(bool &p1, bool &p2, bool &p3, bool &p4)
{
	p1 = false;
	p2 = false;
	p3 = false;
	p4 = false;
}

void assignId(int identifier)
{//what figure is turned on
	id = identifier;
}

void create(HWND h)
{
	hdc = GetDC(h);

	GetClientRect(h, &rect);
    hdc1 = CreateEnhMetaFile(NULL, NULL, NULL, NULL);//for metainformation
	///

	hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//initial brush	
	SelectObject(hdc, hBrush);//apply brush to hdc
	hPen = (HPEN)GetStockObject(BLACK_PEN);//initial black pen
	SelectObject(hdc1, hBrush);//for enfanced metafile the same settings
	SelectObject(hdc1, hPen);//for enfanced metafile the same settings

	width = 0;
	hCompatibleDC = CreateCompatibleDC(hdc);//to apply a bitmap and write there
	hCompatibleBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);//bitmap to draw
	hBitmapDC = CreateCompatibleDC(hdc);//second dc to draw, second buffer
	hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);//

	//we should set the buffers' color,not monochromatic
	DeleteObject(SelectObject(hCompatibleDC, hCompatibleBitmap));//free old bitmap
	DeleteObject(SelectObject(hCompatibleDC, (HBRUSH)WHITE_BRUSH));//white background
	PatBlt(hCompatibleDC, 0, 0, rect.right, rect.bottom, PATCOPY);
	DeleteObject(SelectObject(hBitmapDC, hBitmap));
	DeleteObject(SelectObject(hBitmapDC, (HBRUSH)WHITE_BRUSH));
	PatBlt(hBitmapDC, 0, 0, rect.right, rect.bottom, PATCOPY);

	DeleteObject(SelectObject(hCompatibleDC, hPen));//apply pen for drawing
	DeleteObject(SelectObject(hCompatibleDC, hBrush));//apply null brush instead of white
	//the same for the second buffer
	DeleteObject(SelectObject(hBitmapDC, hPen));
	DeleteObject(SelectObject(hBitmapDC, hBrush));

	SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
	GetTextMetrics(hdc, &tm);
	cxChar = tm.tmAveCharWidth;
	cyChar = tm.tmHeight;
	
}

void Save(HWND h)
{
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = h;
	ofn.hInstance = hInst;
	ofn.lpstrFilter="Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = fullpath;
	ofn.nMaxFile = sizeof(fullpath);
	ofn.lpstrFileTitle = filename;
	ofn.nMaxFileTitle = sizeof(filename);
	ofn.lpstrInitialDir = dir;
	ofn.lpstrTitle = "Save file as...";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER;
	if (GetSaveFileName(&ofn))
	{
		hEnhMtf = CloseEnhMetaFile(hdc1);
		CopyEnhMetaFile(hEnhMtf, fullpath);
		hdc1 = CreateEnhMetaFile(NULL, NULL, NULL, NULL);
		GetEnhMetaFileHeader(hEnhMtf, sizeof(ENHMETAHEADER), &emh);
		SetRect(&rect, emh.rclBounds.left, emh.rclBounds.top, emh.rclBounds.right, emh.rclBounds.bottom);
		DeleteObject(SelectObject(hdc1, hBrush));
		DeleteObject(SelectObject(hdc1, hPen));
		PlayEnhMetaFile(hdc1, hEnhMtf, &rect);
		CloseEnhMetaFile((HDC)hEnhMtf);
		DeleteEnhMetaFile(hEnhMtf);
	}
}

void Open(HWND h)
{
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = h;
	ofn.hInstance = hInst;
	ofn.lpstrFilter = "Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = fullpath;
	ofn.nMaxFile = sizeof(fullpath);
	ofn.lpstrFileTitle = filename;
	ofn.nMaxFileTitle = sizeof(filename);
	ofn.lpstrInitialDir = dir;
	ofn.lpstrTitle = "Open file...";
	ofn.Flags = OFN_EXPLORER | OFN_CREATEPROMPT | OFN_ALLOWMULTISELECT;
	if (GetOpenFileName(&ofn))
	{
		scale = 1;
		xBegin = 0;
		yBegin = 0;
		hdc1 = CreateEnhMetaFile(NULL, NULL, NULL, NULL);
		hEnhMtf = GetEnhMetaFile(fullpath);
		GetEnhMetaFileHeader(hEnhMtf, sizeof(ENHMETAHEADER), &emh);
		SetRect(&rect, emh.rclBounds.left, emh.rclBounds.top, emh.rclBounds.right, emh.rclBounds.bottom);
		DeleteObject(SelectObject(hdc1, hBrush));
		DeleteObject(SelectObject(hdc1, hPen));
		PlayEnhMetaFile(hdc1, hEnhMtf, &rect);

		GetClientRect(h, &rect1);
		DeleteDC(hBitmapDC);
		hBitmapDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, rect1.right, rect1.bottom);
		DeleteObject(SelectObject(hBitmapDC, hBitmap));
		DeleteObject(SelectObject(hBitmapDC, (HBRUSH)WHITE_BRUSH));
		PatBlt(hBitmapDC, 0, 0, rect1.right, rect1.bottom, PATCOPY);
		DeleteObject(SelectObject(hBitmapDC, hPen));
		DeleteObject(SelectObject(hBitmapDC, hBrush));
		PlayEnhMetaFile(hBitmapDC, hEnhMtf, &rect);

		InvalidateRect(h, NULL, TRUE);
		UpdateWindow(h);
		DeleteEnhMetaFile(hEnhMtf);
	}
}

void setWidth(int w)
{
	width = w;
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, width, cc1.rgbResult);
	DeleteObject(SelectObject(hCompatibleDC, hPen));
	DeleteObject(SelectObject(hBitmapDC, hPen));
	DeleteObject(SelectObject(hdc1, hPen));
}

void colPen(HWND h)
{
	cc1.lStructSize = sizeof(CHOOSECOLOR);
	cc1.hInstance = NULL;
	cc1.hwndOwner = h;
	cc1.lpCustColors = crCustColor;
	cc1.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc1.lCustData = 0L;
	cc1.lpfnHook = NULL;
	cc1.rgbResult = RGB(0x80, 0x80, 0x80);
	cc1.lpTemplateName = NULL;
	if (ChooseColor(&cc1))
	{
		DeleteObject(hPen);
		hPen = CreatePen(PS_SOLID, width, cc1.rgbResult);
		DeleteObject(SelectObject(hCompatibleDC, hPen));
		DeleteObject(SelectObject(hBitmapDC, hPen));
		DeleteObject(SelectObject(hdc1, hPen));
	}
}

void colFill(HWND h)
{
	cc2.lStructSize = sizeof(CHOOSECOLOR);
	cc2.hInstance = NULL;
	cc2.hwndOwner = h;
	cc2.lpCustColors = crCustColor;
	cc2.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc2.lCustData = 0L;
	cc2.lpfnHook = NULL;
	cc2.rgbResult = RGB(0x80, 0x80, 0x80);
	cc2.lpTemplateName = NULL;
	if (ChooseColor(&cc2))
	{
		DeleteObject(hBrush);
		hBrush = CreateSolidBrush(cc2.rgbResult);
		DeleteObject(SelectObject(hCompatibleDC, hBrush));
		DeleteObject(SelectObject(hBitmapDC, hBrush));
		DeleteObject(SelectObject(hdc1, hBrush));
	}
}

void New(HWND h)
{
	GetClientRect(h, &rect);
	hdc1 = CreateEnhMetaFile(NULL, NULL, NULL, NULL);
	///
	DeleteObject(SelectObject(hdc1, hBrush));
	DeleteObject(SelectObject(hdc1, hPen));

	width = 0;
	DeleteObject(hCompatibleDC);
	DeleteObject(hBitmapDC);
	hCompatibleDC = CreateCompatibleDC(hdc);
	hCompatibleBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	DeleteObject(SelectObject(hCompatibleDC, hCompatibleBitmap));
	DeleteObject(SelectObject(hCompatibleDC, (HBRUSH)WHITE_BRUSH));
	PatBlt(hCompatibleDC, 0, 0, rect.right, rect.bottom, PATCOPY);
	DeleteObject(SelectObject(hCompatibleDC, hPen));
	DeleteObject(SelectObject(hCompatibleDC, hBrush));
	hBitmapDC = CreateCompatibleDC(hdc);
	hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	DeleteObject(SelectObject(hBitmapDC, hBitmap));
	DeleteObject(SelectObject(hBitmapDC, (HBRUSH)WHITE_BRUSH));
	PatBlt(hBitmapDC, 0, 0, rect.right, rect.bottom, PATCOPY);
	DeleteObject(SelectObject(hBitmapDC, hPen));
	DeleteObject(SelectObject(hBitmapDC, hBrush));
	InvalidateRect(h, NULL, TRUE);
	UpdateWindow(h);
}

void Id(HWND h, int i, int s, int x, int y, bool pol, bool line)
{
	id = i;
	scale = s;
	xBegin = x;
	yBegin = y;
	xx1 = x;
	yy1 = y;
	bPoly = pol;
	bPolyline = line;
	InvalidateRect(h, NULL, FALSE);
}
