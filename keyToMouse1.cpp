#include <windows.h>
#include <fstream>
#include <string>
namespace ano {
	inline auto registerBase(LPCWSTR name,WNDPROC wndProc) noexcept(true) {
		WNDCLASS winc;
		/*WNDCLASS a{
		0,0,0,
		};*/
		winc.cbClsExtra = winc.cbWndExtra = 0;
		winc.hIcon = LoadIcon(nullptr,IDI_APPLICATION);
		winc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		winc.hInstance = GetModuleHandle(0);
		winc.lpszMenuName = nullptr;
		winc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		winc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		winc.lpszClassName = name;
		winc.lpfnWndProc = wndProc;
		return RegisterClass(&winc);
	}
	inline auto getCursorPos() {
		POINT pos;
		GetCursorPos(&pos);
		return pos;
	}
	inline auto setCursorRelativePos(int x,int y) noexcept(true) {
		auto pos = getCursorPos();
		SetCursorPos(pos.x + x, pos.y + y);
		return pos;
	}
	inline auto mouseEvent(DWORD flag,DWORD data=0) {
		auto pos = ano::getCursorPos();
		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi.dx = pos.x;
		input.mi.dy = pos.y;
		input.mi.time = input.mi.dwExtraInfo= 0;
		//input.mi.dwExtraInfo = GetMessageExtraInfo();
		input.mi.dwFlags =flag;
		input.mi.mouseData = data;
		return  SendInput(1, &input, sizeof(input));
	}
}
LRESULT CALLBACK WndProc(HWND hwnd,UINT msg ,WPARAM wp,LPARAM lp) {
	constexpr auto REGISTER_ID = 4649;
	constexpr auto CORSOR_MOVE_SPEED = 10;
	static int xSpeed=0,ySpeed=0;
	switch (msg) {
	case WM_DESTROY:
		UnregisterHotKey(hwnd,REGISTER_ID);
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		if (!RegisterHotKey(hwnd, REGISTER_ID, MOD_CONTROL, 'M')) {
			PostQuitMessage(0);
		}
		break;
	case WM_HOTKEY:
		SetForegroundWindow(hwnd);//Don't use SetActiveWindow function!!!
		break;
	case WM_KEYDOWN:
		if (wp == VK_SPACE) {
			ano::mouseEvent(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP);
			break;
		}
		else if (wp== 'O') {
			ano::mouseEvent(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP);
			break;
		}
		else if (wp == 'M') {
			ano::mouseEvent(MOUSEEVENTF_MIDDLEDOWN|MOUSEEVENTF_MIDDLEUP);
			break;
		}
		else if (wp == 'N') {
			ano::mouseEvent(MOUSEEVENTF_WHEEL, -WHEEL_DELTA);
		}
		else if (wp == 'H') {
			ano::mouseEvent(MOUSEEVENTF_WHEEL, WHEEL_DELTA);
		}
		else if (wp == 'U') {
			ano::mouseEvent(MOUSEEVENTF_HWHEEL, -WHEEL_DELTA);
		}
		else if (wp == 'P') {
			ano::mouseEvent(MOUSEEVENTF_HWHEEL, WHEEL_DELTA);
		}
		switch (wp) {
		case 'J':
			xSpeed = -CORSOR_MOVE_SPEED;
			break;
		case 'L':
			xSpeed = CORSOR_MOVE_SPEED;
			break;
		case 'I':
			ySpeed = -CORSOR_MOVE_SPEED; 
			break;
		case 'K':
			ySpeed = CORSOR_MOVE_SPEED; 
			break;
		default:
			return DefWindowProc(hwnd,msg,wp,lp);
			break;
		}
		ano::setCursorRelativePos(xSpeed, ySpeed);
		break;
	case WM_KEYUP:
		if (wp == 'J' || wp == 'L') {
			xSpeed = 0;
		}
		else if (wp =='I' || wp =='K') {
			ySpeed = 0;
		}
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmdShow) {
	MSG msg;
	ano::registerBase(TEXT("keyToMouse"), WndProc);
	CreateWindow(TEXT("keyToMouse"), TEXT("KeyToMouse"), WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}