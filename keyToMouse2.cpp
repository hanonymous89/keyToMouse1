#include <windows.h>
#include <functional>
#include <thread>
#include <chrono>
#include <string>
#include <string_view>
#include <iterator>
#include <fstream>
#include <array>
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
	class Button {
	private:
		bool is_on;
	public:
		inline auto get() const{
			return is_on;
		}
		inline auto changeover() {
			return is_on = !is_on;
		}
	};
	class KeyToMouseMove{
	private:
		int key,mKey;
		int pos=0;
	public:
		inline KeyToMouseMove(decltype(key) key,decltype(mKey) mKey) :key(key),mKey(mKey) {

		}
		inline auto get() {
			return pos;
		}
		inline bool tryEvent() {
			pos = 0;
			if (GetAsyncKeyState(key)) {
				--pos;
				return true;
			}
			else if (GetAsyncKeyState(mKey)) {
				++pos;
				return true;
			}
			return false;
		}
	};
	class KeyToMouseEventPairInterface {
	private :
		const int key;
		const std::pair<std::pair<DWORD,DWORD>, std::pair<DWORD,DWORD> > events;
	public:
		inline KeyToMouseEventPairInterface(decltype(key) key, decltype(events) events) :key(key),events(events) {}
		inline  auto doEvent(const bool first,const bool x)const {
			if ((GetAsyncKeyState(key) == 0) ^ x) {
				const auto data = first ? events.first : events.second;
				ano::mouseEvent(data.first,data.second);
				return true;
			}
			return false;
		}
		inline  virtual void tryEvent() = 0;
	};
	class KeyToMouseEventSpace:public KeyToMouseEventPairInterface{
	public:
		KeyToMouseEventSpace(int key, std::pair<DWORD,DWORD> noMod,std::pair<DWORD,DWORD> mod) :KeyToMouseEventPairInterface(key,{ noMod, mod}) {}
		inline void tryEvent() override{
			KeyToMouseEventPairInterface::doEvent(/*!*/GetAsyncKeyState(VK_SPACE),true);
		}
	};
	class KeyToMouseEventSwitch:Button,public KeyToMouseEventPairInterface{
	private:
	public:
		KeyToMouseEventSwitch(int key, int on, int off) :KeyToMouseEventPairInterface(key, { {on,0},{off,0} }) {

		}
		inline void tryEvent()override {
			if ((!get() && KeyToMouseEventPairInterface::doEvent(true,true)) || (get() && KeyToMouseEventPairInterface::doEvent(false,false))) {
				changeover();  
			}
		}
	};
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmsShow) {
	enum DATA {
		SPEED,
		BOOST
	};
	constexpr auto DEFAULT_BOOST = 1, DEFAULT_SPEED = 0;
	const auto data = ano::split(ano::File("data.txt").read().getContent(), "\n");
	const auto interval = DATA::SPEED < data.size() ? ano::stoull(data[DATA::SPEED], DEFAULT_SPEED) : DEFAULT_SPEED;//boost=default
	const decltype(interval) boost = (interval / (DATA::BOOST < data.size() ? ano::stod(data[DATA::BOOST], DEFAULT_BOOST) : DEFAULT_BOOST));
	ano::KeyToMouseMove horizon(0x4A, 0x4C);
	ano::KeyToMouseMove vertical(0x49,0x4B );
	ano::KeyToMouseEventSpace scrollLeftUp('G', { MOUSEEVENTF_HWHEEL,-WHEEL_DELTA }, { MOUSEEVENTF_WHEEL,-WHEEL_DELTA });
	ano::KeyToMouseEventSpace scrollRightDown('H', { MOUSEEVENTF_HWHEEL,WHEEL_DELTA }, { MOUSEEVENTF_WHEEL,WHEEL_DELTA });
	ano::KeyToMouseEventSwitch lButton('S', MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP);
	ano::KeyToMouseEventSwitch rButton('F', MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP);
	ano::KeyToMouseEventSwitch mButton('D', MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP);
	std::array < ano::KeyToMouseEventPairInterface*, 5/*btn length*/ > btns{
		&scrollLeftUp,
		&scrollRightDown,
		&lButton,
		&rButton,
		&mButton
	};
	while (!GetAsyncKeyState('M')&& !GetAsyncKeyState(VK_CONTROL)) {//multiThread
		if (horizon.tryEvent() | vertical.tryEvent()) {
			ano::setCursorRelativePos(horizon.get(), vertical.get());
		}
		for (auto &btn : btns) {
			btn->tryEvent();
		}
		std::this_thread::sleep_for(std::chrono::nanoseconds((GetAsyncKeyState(VK_SHIFT)) ? boost : interval));
	}
	return 0;
}