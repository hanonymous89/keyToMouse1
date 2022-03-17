#include <windows.h>
#include <string>
namespace ano {
	inline auto getCursorPos() {
		POINT pos;
		GetCursorPos(&pos);
		return pos;
	}
	inline auto setCursorRelativePos(int x, int y) noexcept(true) {
		auto pos = getCursorPos();
		SetCursorPos(pos.x + x, pos.y + y);
		return pos;
	}
	inline auto mouseEvent(DWORD flag, DWORD data) {
		auto pos = ano::getCursorPos();
		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi.dx = pos.x;
		input.mi.dy = pos.y;
		input.mi.time = input.mi.dwExtraInfo = 0;
		input.mi.dwFlags = flag;
		input.mi.mouseData = data;
		return  SendInput(1, &input, sizeof(input));
	}
}
HHOOK hook;
bool active;
LRESULT CALLBACK hookProc(int nCode,WPARAM wp,LPARAM lp) { 
	static int horizon=0,vertical=0;
	//static bool shift = false;
	const auto speed = wp == WM_KEYDOWN ? 10: 0;
	if (!active||nCode < 0  ) {
		return CallNextHookEx(hook,nCode,wp,lp);
	}
	switch (((KBDLLHOOKSTRUCT*)lp)->vkCode) {
	/*case VK_SHIFT:ghでスクロールを完結する場合
		shift = !shift;
		break;
	*/
	case 'G':
		ano::mouseEvent(MOUSEEVENTF_HWHEEL, -WHEEL_DELTA);
		break;
	case 'H':
		ano::mouseEvent(MOUSEEVENTF_HWHEEL, WHEEL_DELTA);
		break;
	case 'M':
		ano::mouseEvent(MOUSEEVENTF_WHEEL, -WHEEL_DELTA);
		break;
	case 'E':
		ano::mouseEvent(MOUSEEVENTF_WHEEL, WHEEL_DELTA);
		break;
	case 'C':
		active = false;
		break;
	case 'Q':
		PostQuitMessage(0);
		break;
	case 'S':
		ano::mouseEvent(speed?MOUSEEVENTF_LEFTDOWN: MOUSEEVENTF_LEFTUP,0);//こっちにshiftでscrollでもいい
		break;
	case 'F':
		ano::mouseEvent(speed ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP, 0);
		break;
	case 'D':
		ano::mouseEvent(speed ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP, 0);
		break;
	case 'J':
		horizon = -speed;
		break;
	case 'L':
		horizon = speed;
		break;
	case 'I':
		vertical=-speed;
		break;
	case 'K':
		vertical= speed;
		break;
	default:
		if(!vertical&&!horizon)
			return CallNextHookEx(hook, nCode, wp, lp);
	}
	ano::setCursorRelativePos(horizon, vertical);
	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmdShow) {
	constexpr auto REGISTER_HOT_KEY_ID = 4649;
	if (!RegisterHotKey(nullptr, REGISTER_HOT_KEY_ID,MOD_CONTROL,'M') || (hook = SetWindowsHookEx(WH_KEYBOARD_LL, hookProc, hInstance, 0)) == nullptr)return 0;
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (msg.message == WM_HOTKEY) {
			//active = !active;//起動と終了
			active = true;//setwindowhookex//必要な時だけ
		}
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(hook);
	UnregisterHotKey(nullptr, REGISTER_HOT_KEY_ID);
	return 0;
}
