#include <windows.h>
#include <functional>
#include <thread>
#include <chrono>
#include <string>
#include <string_view>
#include <iterator>
#include <fstream>
#include <mutex>
#include <algorithm>
namespace ano {
	auto split(std::string str, std::string cut) {
		std::vector<std::string> data;
		for (auto pos = str.find(cut); pos != std::string::npos; str = str.substr(pos + cut.size()), pos = str.find(cut)) {
			data.push_back(str.substr(0, pos));
		}
		if (!str.empty())data.push_back(str);
		return data;
	}
	auto stoull(std::string str,unsigned long long def) {
		if (!str.empty()&&std::all_of(str.cbegin(), str.cend(), [](char c) {return std::isdigit(c); })) {
			return std::stoull(str);
		}
		return def;
	}
	auto stod(std::string str, double def) {
		if (!str.empty() && std::all_of(str.cbegin(), str.cend(), [](char c) {return std::isdigit(c)||c=='.'; })) {//all_of(,,isdigit)error!
			return std::stod("0"+str);
		}
		return def;
	}
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
	class File {
	private:
		std::string name,content;
	public:
		File(decltype(name) name):name(name){}
		auto & read() {
			std::fstream file(name);
			if (file.fail())return *this;
			content = std::string(std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>());
			return *this;
		}
		auto getContent()const {
			return content;
		}

	};
}
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmsShow) {
	enum DATA {
		SPEED,
		BOOST
	};
	constexpr auto DEFAULT_BOOST = 1,DEFAULT_SPEED=0;
	const auto data=ano::split(ano::File("data.txt").read().getContent(),"\n");
	const auto interval = DATA::SPEED < data.size() ? ano::stoull(data[DATA::SPEED], DEFAULT_SPEED) : DEFAULT_SPEED;//boost=default
	const decltype(interval) boost = (interval / (DATA::BOOST < data.size() ? ano::stod(data[DATA::BOOST], DEFAULT_BOOST) : DEFAULT_BOOST));
	bool lClk=false, rClk=false, mClk=false;
	while (!GetAsyncKeyState('M')&& !GetAsyncKeyState(VK_CONTROL)) {//multiThread
		constexpr auto SPEED = 1;
		std::this_thread::sleep_for(std::chrono::nanoseconds((GetAsyncKeyState(/*VK_SPACE*/VK_SHIFT) != 0) ? boost : interval));//interval/is_boost?boost:defboost
		int xSpeed=0, ySpeed= 0;
		if (GetAsyncKeyState(0x4A)) {
			xSpeed = -SPEED;
		}
		else if (GetAsyncKeyState(0x4C)) {
			xSpeed = SPEED;
		}
		if (GetAsyncKeyState(0x4B)) {
			ySpeed = SPEED;
		}
		else if (GetAsyncKeyState(0x49)) {
			ySpeed = -SPEED;
		}
		if (xSpeed || ySpeed) {
			ano::setCursorRelativePos(xSpeed, ySpeed);
		}
		if (GetAsyncKeyState('G')) {
			ano::mouseEvent((GetAsyncKeyState(VK_SPACE) != 0)?MOUSEEVENTF_WHEEL:MOUSEEVENTF_HWHEEL,-WHEEL_DELTA);
		}
		else if (GetAsyncKeyState('H') ) {
			ano::mouseEvent((GetAsyncKeyState(VK_SPACE) != 0) ? MOUSEEVENTF_WHEEL : MOUSEEVENTF_HWHEEL,WHEEL_DELTA);
		}
		if (!lClk && GetAsyncKeyState('S')) {
			lClk = true;
			ano::mouseEvent(MOUSEEVENTF_LEFTDOWN, 0);
		}
		else if (lClk && !GetAsyncKeyState('S')) {
			lClk = false;
			ano::mouseEvent(MOUSEEVENTF_LEFTUP, 0);
		}
		if (!rClk && GetAsyncKeyState('F')) {
			rClk = true;
			ano::mouseEvent(MOUSEEVENTF_RIGHTDOWN, 0);
		}
		else if (rClk && !GetAsyncKeyState('F')) {
			rClk = false;
			ano::mouseEvent(MOUSEEVENTF_RIGHTUP, 0);
		}
		if (!mClk && GetAsyncKeyState('D')) {
			mClk = true;
			ano::mouseEvent(MOUSEEVENTF_MIDDLEDOWN, 0);
		}
		else if (mClk && !GetAsyncKeyState('D')) {
			mClk = false;
			ano::mouseEvent(MOUSEEVENTF_MIDDLEUP, 0);
		}
	}
	return 0;
}