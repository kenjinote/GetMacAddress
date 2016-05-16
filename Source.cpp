#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib,"netapi32") 

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

struct ASTAT
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[30];
} Adapter;

void GetMacAddress(HWND hList)
{
	LANA_ENUM lenum;
	TCHAR szMACAddress[64];
	NCB Ncb = { 0 };
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR*)&lenum;
	Ncb.ncb_length = sizeof(lenum);
	if (Netbios(&Ncb) == 0)
	{
		for (int i = 0; i<lenum.length; ++i)
		{
			ZeroMemory(&Ncb, sizeof(Ncb));
			Ncb.ncb_command = NCBRESET;
			Ncb.ncb_lana_num = lenum.lana[i];
			if (Netbios(&Ncb) != 0)continue;
			ZeroMemory(&Ncb, sizeof(Ncb));
			Ncb.ncb_command = NCBASTAT;
			Ncb.ncb_lana_num = lenum.lana[i];
			lstrcpyA((LPSTR)(Ncb.ncb_callname), "*");
			Ncb.ncb_buffer = (UCHAR*)&Adapter;
			Ncb.ncb_length = sizeof(Adapter);
			if (Netbios(&Ncb) != 0)continue;
			wsprintf(szMACAddress, TEXT("%02X-%02X-%02X-%02X-%02X-%02X"),
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]);
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)szMACAddress);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HWND hList;
	switch (msg)
	{
	case WM_CREATE:
		hList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("Macアドレスを取得"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		PostMessage(hWnd, WM_COMMAND, IDOK, 0);
		break;
	case WM_SIZE:
		MoveWindow(hList, 10, 10, 256, 128, TRUE);
		MoveWindow(hButton, 10, 148, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			SendMessage(hList, LB_RESETCONTENT, 0, 0);
			GetMacAddress(hList);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Netbios 関数を使用してMacアドレスを取得する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
