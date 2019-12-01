
#include <windows.h>
#include <functional>

extern "C" {
	void *load_ppm(char *filename, int *w, int *h);
}

void AddMsgFunc(std::function<void(HWND hWnd, HDC hdc)> f);

void AddMsgFunc(int msg, std::function<int(HWND , WPARAM, LPARAM)> f);



static void *pixels = NULL;
static int pixelw;
static int pixelh;
void out2rect(void *data, int w1, int h1, HDC &hdc, RECT  &rectw) {
	//is->video_st->codec->width;
	//is->video_st->codec->height;
	BITMAPINFO lpBmpInfo;
	lpBmpInfo.bmiHeader.biBitCount = 32;
	lpBmpInfo.bmiHeader.biClrImportant = 0;
	lpBmpInfo.bmiHeader.biClrUsed = 0;
	lpBmpInfo.bmiHeader.biCompression = BI_RGB;
	lpBmpInfo.bmiHeader.biPlanes = 1;
	lpBmpInfo.bmiHeader.biXPelsPerMeter = 0;
	lpBmpInfo.bmiHeader.biYPelsPerMeter = 0;
	lpBmpInfo.bmiHeader.biSize = sizeof(lpBmpInfo.bmiHeader);
	lpBmpInfo.bmiHeader.biWidth = w1;
	lpBmpInfo.bmiHeader.biHeight = h1;
	lpBmpInfo.bmiHeader.biSizeImage = w1 * h1 * 4;

	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchDIBits(hdc, 0, rectw.bottom, rectw.right - rectw.left, -(rectw.bottom - rectw.top), 0, 0, w1, h1,
		data, &lpBmpInfo, DIB_RGB_COLORS, SRCCOPY);
	
}

static char** win32_argv_utf8 = NULL;
static int win32_argc = 0;
static void prepare_app_arguments(int *argc_ptr, char ***argv_ptr)
{
	char *argstr_flat;
	wchar_t **argv_w;
	int i, buffsize = 0, offset = 0;

	if (win32_argv_utf8) {
		/**argc_ptr = win32_argc;
		*argv_ptr = win32_argv_utf8;*/
		//return;
	}

	win32_argc = 0;
	argv_w = CommandLineToArgvW(GetCommandLineW(), &win32_argc);
	//win32_argc = *argc_ptr;
	//argv_w = *argv_ptr;
	if (win32_argc <= 0)
		return;

	/* determine the UTF-8 buffer size (including NULL-termination symbols) */
	for (i = 0; i < win32_argc; i++)
		buffsize += WideCharToMultiByte(CP_UTF8, 0, argv_w[i], -1,
			NULL, 0, NULL, NULL);

	win32_argv_utf8 = (char**)malloc(sizeof(char *) * (win32_argc + 1) + buffsize);
	argstr_flat = (char *)win32_argv_utf8 + sizeof(char *) * (win32_argc + 1);
	if (!win32_argv_utf8) {
		LocalFree(argv_w);
		return;
	}

	for (i = 0; i < win32_argc; i++) {
		win32_argv_utf8[i] = &argstr_flat[offset];
		offset += WideCharToMultiByte(CP_UTF8, 0, argv_w[i], -1,
			&argstr_flat[offset],
			buffsize - offset, NULL, NULL);
	}
	win32_argv_utf8[i] = NULL;
	LocalFree(argv_w);

	*argc_ptr = win32_argc;
	*argv_ptr = win32_argv_utf8;
}

void RefreshView(HWND hWnd, HDC hdc) {
	RECT rect;
	GetClientRect(hWnd, &rect);

	if (pixels != NULL && pixelw > 0 && pixelh > 0) {
		out2rect(pixels, pixelw, pixelh, hdc, rect);
	}
}

int DragFileProcess(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	HDROP hdrop = (HDROP)wParam;
	char sDropFilePath[MAX_PATH + 1];
	int iDropFileNums = 0;
	iDropFileNums = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);//获取拖放文件个数
	if (iDropFileNums<=0) {
		DragFinish(hdrop);//释放文件名缓冲区 
		return 0;
	}
	for (int i = 0; i < iDropFileNums; i++)//分别获取拖放文件名(针对多个文件操作)
	{
		DragQueryFileA(hdrop, i, sDropFilePath, sizeof(sDropFilePath));
	}
	
	if (strlen(sDropFilePath) > 0) {
		void *p = pixels;
		pixels = load_ppm(sDropFilePath, &pixelw, &pixelh);
		if (p != NULL) {
			free(p);
			p = NULL;
		}
		InvalidateRect((HWND)hWnd, NULL, true);
	}
	//DragQueryFile(hdrop, 0, sDropFilePath, MAX_PATH);//分别获取拖放文件名(针对单个文件操作)    
	DragFinish(hdrop);//释放文件名缓冲区  

	return 0;
}

int main(int argc,char**argv) {
	HWND hwnd = ::GetConsoleWindow();
	HINSTANCE instance = GetModuleHandle(NULL);
	ShowWindow(hwnd, 0);
	AddMsgFunc(RefreshView);
	AddMsgFunc(WM_DROPFILES, DragFileProcess);
	/*int argc;
	char **argv;*/
	//prepare_app_arguments(&argc, &argv);

	if (argc > 1) {
		if (strlen(argv[1]) > 0) {
			void *p = pixels;
			pixels = load_ppm(argv[1], &pixelw, &pixelh);
			if (p != NULL) {
				free(p);
				p = NULL;
			}
		}
	}
	//free(argv);

	wWinMain(instance, NULL, NULL, 1);
	ShowWindow(hwnd, 1);
}