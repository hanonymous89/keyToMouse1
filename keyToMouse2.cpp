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
		if (!str.empty()&&std::all_of(str.cbegin(), str.cend(), [](char c) {return std::isdigit(c); })) {//all_of(,,isdigit)error!
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
	const auto interval = DATA::SPEED < data.size() ? ano::stoull(data[DATA::SPEED], DEFAULT_SPEED) : DEFAULT_SPEED;
	const decltype(interval) boost = (interval / (DATA::BOOST < data.size() ? ano::stod(data[DATA::BOOST], DEFAULT_BOOST) : DEFAULT_BOOST));
	while (!GetAsyncKeyState(VK_SPACE)) {
		constexpr auto SPEED = 1;
		std::this_thread::sleep_for(std::chrono::nanoseconds((GetAsyncKeyState(VK_SHIFT)!=0)?boost:interval));//interval/is_boost?boost:defboost
		int xSpeed=0, ySpeed= 0;
		if (GetAsyncKeyState(0x4A)!=0) {
			xSpeed = -SPEED;
		}
		else if (GetAsyncKeyState(0x4C) != 0) {
			xSpeed = SPEED;
		}
		if (GetAsyncKeyState(0x4B) != 0) {
			ySpeed = SPEED;
		}
		else if (GetAsyncKeyState(0x49) != 0) {
			ySpeed = -SPEED;
		}
		if (xSpeed || ySpeed) {
			ano::setCursorRelativePos(xSpeed, ySpeed);
		}
	}
	return 0;
}