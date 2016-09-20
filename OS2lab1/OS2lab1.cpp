// OS2lab1.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "OS2lab1.h"
#include <vector>
#include <gdiplus.h>

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
	PAINTSTRUCT ps;
	HDC hdc;
	////////////////////////////////////////////
	//variable scope
	POINT t; 
	static std::vector<POINT> pos; 
	static bool status; 
	PAINTSTRUCT pt;
	////////////////////////////////////////////

	RECT rcClient;                 // client area rectangle 
	POINT ptClientUL;              // client upper left corner 
	POINT ptClientLR;              // client lower right corner 
	static POINTS ptsBegin;        // beginning point 
	static POINTS ptsEnd;          // new endpoint 
	static POINTS ptsPrevEnd;      // previous endpoint 
	static BOOL fPrevLine = FALSE; // previous line flag 

	switch (message)
	{
	case WM_LBUTTONDOWN:

		// Capture mouse input. 

		SetCapture(hWnd);

		// Retrieve the screen coordinates of the client area, 
		// and convert them into client coordinates. 

		GetClientRect(hWnd, &rcClient);
		ptClientUL.x = rcClient.left;
		ptClientUL.y = rcClient.top;

		// Add one to the right and bottom sides, because the 
		// coordinates retrieved by GetClientRect do not 
		// include the far left and lowermost pixels. 

		ptClientLR.x = rcClient.right + 1;
		ptClientLR.y = rcClient.bottom + 1;
		ClientToScreen(hWnd, &ptClientUL);
		ClientToScreen(hWnd, &ptClientLR);

		// Copy the client coordinates of the client area 
		// to the rcClient structure. Confine the mouse cursor 
		// to the client area by passing the rcClient structure 
		// to the ClipCursor function. 

		SetRect(&rcClient, ptClientUL.x, ptClientUL.y,
			ptClientLR.x, ptClientLR.y);
		ClipCursor(&rcClient);//cursor constrains

		// Convert the cursor coordinates into a POINTS 
		// structure, which defines the beginning point of the 
		// line drawn during a WM_MOUSEMOVE message. 

		ptsBegin = MAKEPOINTS(lParam);
		return 0;

	case WM_MOUSEMOVE:

		// When moving the mouse, the user must hold down 
		// the left mouse button to draw lines. 

		if (wParam & MK_LBUTTON)
		{

			// Retrieve a device context (DC) for the client area. 

			hdc = GetDC(hWnd);

			// The following function ensures that pixels of 
			// the previously drawn line are set to white and 
			// those of the new line are set to black. 

			SetROP2(hdc, R2_NOTXORPEN);

			// If a line was drawn during an earlier WM_MOUSEMOVE 
			// message, draw over it. This erases the line by 
			// setting the color of its pixels to white. 

			if (fPrevLine)
			{
				MoveToEx(hdc, ptsBegin.x, ptsBegin.y,
					(LPPOINT)NULL);
				LineTo(hdc, ptsPrevEnd.x, ptsPrevEnd.y);
			}

			// Convert the current cursor coordinates to a 
			// POINTS structure, and then draw a new line. 

			ptsEnd = MAKEPOINTS(lParam);
			MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT)NULL);
			LineTo(hdc, ptsEnd.x, ptsEnd.y);

			// Set the previous line flag, save the ending 
			// point of the new line, and then release the DC. 

			fPrevLine = TRUE;
			ptsPrevEnd = ptsEnd;
			ReleaseDC(hWnd, hdc);
		}
		break;


	case WM_LBUTTONUP:

		// The user has finished drawing the line. Reset the 
		// previous line flag, release the mouse cursor, and 
		// release the mouse capture. 

		fPrevLine = FALSE;
		ClipCursor(NULL);
		ReleaseCapture();
		return 0;


	//case WM_LBUTTONDOWN:
	//{
		//int iPosX = LOWORD(lParam);
		//int iPosY = HIWORD(lParam);
		//wchar_t waCoord[20];
		//wsprintf(waCoord,_T("(%i, %i)"),iPosX,iPosY);
		//::MessageBox(hWnd,waCoord, _T("LBM Click"),MB_OK);
	//}
	//case WM_LBUTTONDOWN: 
		//status = true;
		//break;

	//case WM_LBUTTONUP: 
		//status = false;
		//t.x = -1; pos.push_back(t);
		//break;

	//case WM_MOUSEMOVE: 
		//GetCursorPos(&t);
		//ScreenToClient(hWnd, &t);
		//(status) ? pos.push_back(t) : NULL;
		//(status) ? InvalidateRect(hWnd, NULL, true) : NULL;
		//break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_LINE:
			bDrawLine = !bDrawLine;
			InvalidateRect(hWnd,0,TRUE);
			break;
		case IDM_ELLIPSE:
			bDrawEllipse = !bDrawEllipse;
			InvalidateRect(hWnd, 0, TRUE);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		// TODO: добавьте любой код отрисовки...
		hdc = BeginPaint(hWnd, &ps);
	
		//(!pos.size()) ? NULL : MoveToEx(hdc, pos[0].x, pos[0].y, NULL);
		//for (size_t i = 0; i<pos.size(); ++i)
		//	(pos[i].x == -1 && i<pos.size() - 1) ? MoveToEx(hdc, pos[i + 1].x, pos[i + 1].y, NULL) : LineTo(hdc, pos[i].x, pos[i].y);

		
		
		/*HPEN hPenOld;//used for storage
		if (bDrawLine)//if line is selected
		{
			//Draw a red line
			HPEN hLinePen;//used for storage
			COLORREF qLineColor;
			qLineColor = RGB(255, 0, 0);
			hLinePen = CreatePen(PS_SOLID, 7, qLineColor);//solid/dashed/dotted; width
			hPenOld = (HPEN)SelectObject(hdc, hLinePen);//storing lod pen

			MoveToEx(hdc, 100, 100, NULL);
			LineTo(hdc, 500, 250);

			SelectObject(hdc, hPenOld);
			DeleteObject(hLinePen);
		}
		if (bDrawEllipse)//if ellipse is selected
		{
			//Draw a blue ellipse
			HPEN hEllipsePen;
			COLORREF qEllipseColor;
			qEllipseColor = RGB(0, 0, 255);
			hEllipsePen = CreatePen(PS_SOLID, 3, qEllipseColor);
			hPenOld = (HPEN)SelectObject(hdc, hEllipsePen);

			Arc(hdc, 100, 100, 500, 250, 0, 0, 0, 0);

			SelectObject(hdc, hPenOld);
			DeleteObject(hEllipsePen);
		}
		*/

		EndPaint(hWnd, &ps);
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
