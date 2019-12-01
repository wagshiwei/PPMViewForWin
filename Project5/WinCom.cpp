
#include <windows.h>
#include <vector>
#include <map>
#include <functional>

#define MAX_LOADSTRING 100

// �˴���ģ���а����ĺ�����ǰ������:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//void RefreshView(HWND hWnd, HDC hdc);
// ȫ�ֱ���:
static HINSTANCE hInst;                                // ��ǰʵ��
static WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
static WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

static std::map<int, std::function<int(HWND, WPARAM, LPARAM)>> cmdFuncs;
static std::function<void(HWND hWnd, HDC hdc)> RefreshView;

void AddMsgFunc(std::function<void(HWND hWnd, HDC hdc)> f) {
	RefreshView = f;
}

void AddMsgFunc(int msg,std::function<int(HWND, WPARAM, LPARAM)> f) {
	cmdFuncs[msg] = f;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: �ڴ˴����ô��롣

	// ��ʼ��ȫ���ַ���
	//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
	wsprintfW(szWindowClass, L"_12345");
	wsprintfW(szWindowClass, L"title");
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

	MSG msg;

	// ����Ϣѭ��:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;// LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
	wcex.hCursor = NULL;// LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = NULL;// (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
	int cxScreen, cyScreen;
	cxScreen = GetSystemMetrics(SM_CXSCREEN);
	cyScreen = GetSystemMetrics(SM_CYSCREEN);
	int w = 720/10*5;
	int h = 1080/10*5;
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		cxScreen/2-w/2, cyScreen/2 - h/2, w, h, nullptr, nullptr, hInstance, nullptr);
	//SetTimer(hWnd, 1, 60, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	//CreateEGLContext(hWnd, &eglDisplay, &eglContext, &eglSurface);
	DragAcceptFiles(hWnd, true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


static HDC hdcBuffer=0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//HPEN newPen;       // ���ڴ����»���   
	//HPEN *pOldPen;     // ���ڴ�žɻ���   
	//HBRUSH newBrush;   // ���ڴ����»�ˢ   
	//HBRUSH *pOldBrush; // ���ڴ�žɻ�ˢ   
	RECT rect;
	HDC hdc;
	HBITMAP hBitMap;
	HGDIOBJ hOldBitMap;
	int W;
	int H;
	GetClientRect(hWnd, &rect);

	for (std::map<int, std::function<int(HWND, WPARAM, LPARAM)>>::iterator itor = cmdFuncs.begin();  itor !=cmdFuncs.end(); itor++) {
		int k =itor->first;
		if (message == k) {
			itor->second(hWnd,wParam, lParam);
		}
	}

	switch (message)
	{
	case WM_CREATE:
		W = LOWORD(lParam); // width of client area
		H = HIWORD(lParam); // height of client area
		hdc = GetDC(hWnd); //��ȡ�豸

		hdcBuffer = CreateCompatibleDC(hdc); //���豸����һ���ڴ�ռ�

		hBitMap = CreateCompatibleBitmap(hdc, W, H); //����һ��cxClient, cyClient��С������ӦDC�豸������λͼ

		ReleaseDC(hWnd, hdc);

		SelectObject(hdcBuffer, hBitMap); //��λͼ����ΪhdcBuffer�Ļ�ˢ

		if (RefreshView!=nullptr) {
			RefreshView(hWnd, hdcBuffer);
		}
		//��������������������������������
		//	��Ȩ����������ΪCSDN������Swell_Fish����ԭ�����£���ѭ CC 4.0 BY - SA ��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
		//	ԭ�����ӣ�https ://blog.csdn.net/polley88/article/details/49719299
		return  DefWindowProc(hWnd, message, wParam, lParam);

	case WM_ERASEBKGND:
			return 1;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
			/*case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;*/
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_KEYDOWN:
		
		break;
	case WM_SIZE:
		W = LOWORD(lParam); // width of client area
		H = HIWORD(lParam); // height of client area

		hdc = GetDC(hWnd); //��ȡ�豸
		hBitMap = CreateCompatibleBitmap(hdc, W, H); //����һ��cxClient, cyClient��С������ӦDC�豸������λͼ
		ReleaseDC(hWnd, hdc);

		hOldBitMap = SelectObject(hdcBuffer, hBitMap); //��λͼ����ΪhdcBuffer�Ļ�ˢ
		DeleteObject(hOldBitMap);
		/*if (eglSurface) {
			glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		}*/
		InvalidateRect(hWnd, NULL, false);
		
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_TIMER:
		if (hdcBuffer!=0) {
			//PatBlt(hdcBuffer, 0, 0, rect.right, rect.bottom, WHITENESS);
			//RefreshView(hWnd, hdcBuffer);
		}
		InvalidateRect(hWnd, NULL, false);
		//PostMessage(hWnd, WM_PAINT, 0, 0);
		//return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
		PatBlt(hdcBuffer, 0, 0, rect.right, rect.bottom, WHITENESS);
		//Rectangle(hdc, 0, 0, rect.right, rect.bottom);
		if (RefreshView != nullptr) {
			RefreshView(hWnd, hdcBuffer);
		}
		SetStretchBltMode(hdc, COLORONCOLOR);
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBuffer, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);


	}
	break;
	
	case WM_MOUSEWHEEL:

		break;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

