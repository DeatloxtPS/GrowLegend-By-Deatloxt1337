#pragma once
#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <chrono>
using namespace std::chrono;
using namespace std;
#pragma warning(disable : 4996)
#pragma warning(disable : 4430)

inline long long GetCurrentTimeInternalSeconds() {
	return (duration_cast<seconds>(system_clock::now().time_since_epoch())).count();
}

inline void SendConsoleText(const string text) {
	const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int k = 15;
	SetConsoleTextAttribute(hConsole, k);
	cout << "[Console]: " << text << endl;
}

// color to C++ color # by Leom#0001
inline void ColoredText(const string text, const string color)
{
	int colordecode = 0;
	if (color == "red") // error color
	{
		colordecode = 12;
	}
	else if (color == "yellow") // warning color
	{
		colordecode = 14;
	}
	const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colordecode);
	cout << text << endl;
}

inline void Console_Handler() {
	HANDLE hInput = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	DWORD prev_mode;
	GetConsoleMode(hInput, &prev_mode);
	SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));
}
