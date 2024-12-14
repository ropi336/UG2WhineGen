#include <windows.h>
#include <limits.h>
#include <stdlib.h>
#include "stdio.h"
#include <iostream>
#include <direct.h>
#include "..\includes\miniaudio.h"
#include "GlobalVariables.h"
#include "..\includes\injector\injector.hpp"
#include "..\includes\IniReader.h"

ma_result result;
ma_engine engine;
ma_sound sound, sound_two, sound_reverse, sound_on, sound_off;

bool checkFileExists(std::string filename, bool shouldError = false);
void coutCustom(std::string messageOne, std::string messageTwo);
void coutCustomResult(std::string messageOne, ma_result result);
std::wstring conWstring(std::string inString);

void SplitString(std::string stringIn)
{
	std::stringstream ss(stringIn);
	std::string tmp;
	while (std::getline(ss, tmp, ','))
	{
		carList.push_back(tmp);
	}
}

float mTz(float input)
{
	if (input < 0.0)
	{
		return 0.0;
	}
	
	return input;
}

void VolSettings()
{
	while (true)
	{
		CIniReader iniReader("UG2WhineGen.ini");
		IniMult = iniReader.ReadFloat("Settings", "Volume", -1.0);
		Sleep(1000);
	}
}

void InitData()
{
	char cwd[_MAX_PATH];
	std::string currentDir = _getcwd(cwd, sizeof(cwd));
	coutCustom("WorkingDirectory: ", currentDir);
	// Read values from .ini
	CIniReader iniReader("UG2WhineGen.ini");
	if (checkFileExists("scripts/UG2WhineGen.ini", true) == false)
	{
		iniReader.WriteFloat("Settings", "Volume", 1.0);
		iniReader.WriteBoolean("Settings", "EnableWhine", true);
		iniReader.WriteString("Settings", "CarList", "ALL");
		coutCustom("Couldn't find file", "Oh no :(");
	}
	else
	{
		CarInput = iniReader.ReadString("Settings", "CarList", "DEFAULTFAIL");
		coutCustom("Reading car input: ", CarInput);
		coutCustom("Anything: ", "Anything2");
		if (iniReader.ReadFloat("Settings", "Volume", -1.0) == -1.00)
		{
			MessageBoxA(NULL, "Failed to read Volume setting. Ensure .ini file is formatted correctly. Delete the file to regenerate it.", "UG2WhineGen", MB_ICONERROR);
		}
		if (CarInput == "DEFAULTFAIL")
		{
			MessageBoxA(NULL, "Failed to read CarList setting. Ensure .ini file is formatted correctly. Delete the file to regenerate it.", "UG2WhineGen", MB_ICONERROR);
		}
	}

	if (CarInput == std::string("ALL"))
	{
		gWhineAll = true;
		gWhineEnabled = true;
	}
	else
	{
		//Load carlist into array vector
		SplitString(CarInput);
	}
}

bool ValidateCar()
{
	if (gWhineAll == true)
	{
		gWhineEnabled = true;
		return true;
	}
	gCarNameChar = &gCarName[0];
	for (auto it = carList.begin(); it != carList.end(); ++it)
	{
		if (gCarNameChar == (*it))
		{
			gWhineEnabled = true;
			return true;
		}
	}
	return gWhineEnabled;
}

void initSnd(void)
{
	checkFileExists("scripts/snd/car_whine.wav", true);
	checkFileExists("scripts/snd/car_whine_2.wav", true);
	checkFileExists("scripts/snd/car_whine_reverse.wav", true);
	checkFileExists("scripts/snd/snd_off.wav", true);
	checkFileExists("scripts/snd/snd_on.wav", true);
	result = ma_engine_init(NULL, &engine);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("sound engine initiliazed: ", result);
	}
	else
	{
		coutCustomResult("failed to init snd engine: ", result);
	}

	result = ma_sound_init_from_file(&engine, "scripts/snd/car_whine.wav", NULL, NULL, NULL, &sound);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound one has init with code: ", result);
	}
	else
	{
		coutCustomResult("Sound one failed to init with code : ", result);
	}
	ma_sound_set_looping(&sound, true);
	ma_sound_set_volume(&sound, 0.00f);
	ma_sound_set_pitch(&sound, 0.3f);

	result = ma_sound_init_from_file(&engine, "scripts/snd/car_whine_2.wav", NULL, NULL, NULL, &sound_two);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound two has init with code: ", result);
	}
	else
	{
		coutCustomResult("Sound two failed to init with code: ", result);
	}
	ma_sound_set_looping(&sound_two, true);
	ma_sound_set_volume(&sound_two, 0.00f);
	ma_sound_set_pitch(&sound_two, 0.3f);

	result = ma_sound_init_from_file(&engine, "scripts/snd/car_whine_reverse.wav", NULL, NULL, NULL, &sound_reverse);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound reverse has init with code: ", result);
	}
	else
	{
		coutCustomResult("Sound reverse failed to init with code: ", result);
	}
	ma_sound_set_looping(&sound_reverse, true);
	ma_sound_set_volume(&sound_reverse, 0.00f);
	ma_sound_set_pitch(&sound_reverse, 1.0f);

	result = ma_sound_init_from_file(&engine, "scripts/snd/snd_off.wav", NULL, NULL, NULL, &sound_off);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound offswitch has init with code: ", result);
	}
	else
	{
		coutCustomResult("Sound offswitch failed to init with code: ", result);
	}
	ma_sound_set_looping(&sound_off, false);
	ma_sound_set_volume(&sound_off, 0.05f);

	result = ma_sound_init_from_file(&engine, "scripts/snd/snd_on.wav", NULL, NULL, NULL, &sound_on);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound onswitch has init with code: ", result);
	}
	else
	{
		coutCustomResult("Sound onswitch failed to init with code: ", result);
	}
	ma_sound_set_looping(&sound_on, false);
	ma_sound_set_volume(&sound_on, 0.05f);

	result = ma_sound_start(&sound);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound one has started with code: ", result);
	}
	else
	{
		coutCustomResult("Sound one failed to started with code: ", result);
	}

	result = ma_sound_start(&sound_two);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound two has started with code: ", result);
	}
	else
	{
		coutCustomResult("Sound two failed to started with code: ", result);
	}

	result = ma_sound_start(&sound_reverse);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound reverse has started with code: ", result);
	}
	else
	{
		coutCustomResult("Sound reverse failed to started with code: ", result);
	}
}

bool playSnd(void)
{
	if (!ValidateCar())
	{
		coutCustomResult("Car/gearbox invalid, returning false", result);
		return false;
	}
	ma_sound_set_volume(&sound, 0.00f);
	ma_sound_set_volume(&sound_two, 0.00f);
	ma_sound_set_volume(&sound_reverse, 0.00f);

	result = ma_sound_start(&sound);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound one started with code: ", result);
	}
	else
	{
		coutCustomResult("Failed to start sound one with code: ", result);
	}

	result = ma_sound_start(&sound_two);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound two started with code: ", result);
	}
	else
	{
		coutCustomResult("Failed to start sound two with code: ", result);
	}

	result = ma_sound_start(&sound_reverse);
	if (result == MA_SUCCESS)
	{
		coutCustomResult("Sound two started with code: ", result);
	}
	else
	{
		coutCustomResult("Failed to start sound two with code: ", result);
	}
	return true;
}

float mapToRangeExp(float x, float rangemin, float rangemax, float speedlimit) {
	static float modifier;

	if (x < 0) {
		return rangemin; // Handling negative inputs if needed
	}

	// Calculate the scaled value
	float y = rangemin + (x / speedlimit) * (rangemax - rangemin);
	double normalizedSpeed = x / 70.0;
	modifier = 0.5 * sqrt(normalizedSpeed);

	if (gSpeedLast > gSpeed && gPitchModifier > gPitchModTarget)
	{
		gPitchModifier -= 0.00100;
	}

	if (gSpeedLast < gSpeed && gPitchModifier < gPitchModTarget)
	{
		gPitchModifier += 0.00100;
	}

	gSpeedLast = gSpeed;

	if (modifier > 0.6)
	{
		modifier = 0.6;
	}

	return y + modifier + gPitchModifier;
}

float mapToRange(float x, float rangemin, float rangemax, float speedlimit, bool isMute = false) {
	static float modifier;

	if (x < 0) {
		return rangemin; // Handling negative inputs if needed
	}

	// Calculate the scaled value
	float y = rangemin + (x / speedlimit) * (rangemax - rangemin);

	return y;
}

float mapToRangeInverse(float x, float rangemin, float rangemax, float speedlimit, bool isMute = false) {
	if (isMute)
	{
		return 0.0f;
	}

	if (x < 0)
	{
		return rangemin; // Handling negative inputs if needed
	}

	// Calculate the scaled value
	float y = rangemax - (x / speedlimit) * (rangemax - rangemin);

	// Ensure it doesn't exceed the upper limit
	if (y > rangemax)
	{
		return rangemax;
	}

	// Ensure it doesn't exceed the lower limit
	if (y < rangemin)
	{
		return rangemin;
	}

	return y;
}

void fade_volume_to_zero(ma_sound* pSound, ma_sound* pSoundTwo, ma_uint64 duration_in_milliseconds, float initVolume, float initVolume2)
{
	ma_uint64 current_time = 0;
	float initial_volume = initVolume;
	float initial_volume2 = initVolume2;
	float target_volume = 0.0f;  // Target volume is 0

	// Time step in milliseconds (can be adjusted for more precision).
	ma_uint64 time_step = 10; // Adjust volume every 10 milliseconds.

	// Loop to gradually decrease the volume over the specified duration.
	while (current_time < duration_in_milliseconds) {
		// Calculate the volume at this time step.
		float current_volume = initial_volume - ((initial_volume - target_volume) * (float)current_time / (float)duration_in_milliseconds);
		float current_volume2 = initial_volume2 - ((initial_volume2 - target_volume) * (float)current_time / (float)duration_in_milliseconds);

		// Set the new volume for the sound.
		ma_sound_set_volume(pSound, current_volume);
		ma_sound_set_volume(pSoundTwo, current_volume2);

		// Wait for the next time step (sleep or delay function).
		Sleep(time_step);  // Sleeps for `time_step` milliseconds.

		// Increment the current time by the time step.
		current_time += time_step;
	}

	// Finally, set the volume to exactly 0 to ensure it fully fades out.
	ma_sound_set_volume(pSound, 0.0f);
	ma_sound_set_volume(pSoundTwo, 0.0f);
}

void toggleSound()
{
	CIniReader iniReader("UG2WhineGen.ini");
	if (GetAsyncKeyState(VK_F1) & 0x8000) // If high-order bit is set, F1 is down
	{
		if (!f1WasPressed)
		{ // If it wasn't already pressed
			f1WasPressed = true;
			if (gGlobalEnable == true)
			{
				gGlobalEnable = false;
				ma_sound_start(&sound_off);
			}
			else
			{
				gGlobalEnable = true;
				ma_sound_start(&sound_on);
			}
			iniReader.WriteBoolean("Settings", "EnableWhine", gGlobalEnable);
		}
	}
	else
	{
		f1WasPressed = false; // Reset the flag when F1 is not pressed
	}
}

void setCursorPosition(int x, int y)
{
	COORD position = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(gConsoleHandle, position);
}

void coutCustom(std::string messageOne, std::string messageTwo) {
	// Move the cursor to the top left corner

	std::vector<std::wstring> curVars;
	curVars.push_back(conWstring(messageOne) + conWstring(messageTwo));
	setCursorPosition(0, gConLine);

	// Write each line in the buffer to the console
	for (const std::wstring& line : curVars) {
		DWORD charsWritten;
		WriteConsoleW(gConsoleHandle, line.c_str(), line.size(), &charsWritten, nullptr);
	}
	gConLine++;
}

void coutCustomResult(std::string messageOne, ma_result result) {
	// Move the cursor to the top left corner

	std::vector<std::wstring> curVars;
	std::string messageTwo = std::string(ma_result_description(result));
	//std::string messageTwo = "test";
	curVars.push_back(conWstring(messageOne) + conWstring(messageTwo));
	setCursorPosition(0, gConLine);

	// Write each line in the buffer to the console
	for (const std::wstring& line : curVars) {
		DWORD charsWritten;
		WriteConsoleW(gConsoleHandle, line.c_str(), line.size(), &charsWritten, nullptr);
	}
	gConLine++;
}

void renderToConsole(const std::vector<std::wstring>& buffer)
{
	int y = 0; // Track the current line number

	for (const std::wstring& line : buffer) {
		size_t start = 0;
		size_t pos = line.find(L'X'); // Find the first occurrence of 'X'

		while (pos != std::wstring::npos) {
			// Write the segment up to 'X'
			std::wstring segment = line.substr(start, pos - start);
			DWORD charsWritten;
			setCursorPosition(0, y); // Move to the start of the current line
			WriteConsoleW(gConsoleHandle, segment.c_str(), segment.size(), &charsWritten, nullptr);

			// Move to the next line
			y++;
			start = pos + 1;               // Skip over 'X'
			pos = line.find(L'X', start);  // Find the next 'X' if it exists
		}

		// Write any remaining text after the last 'X'
		std::wstring remaining = line.substr(start);
		setCursorPosition(0, y);
		DWORD charsWritten;
		WriteConsoleW(gConsoleHandle, remaining.c_str(), remaining.size(), &charsWritten, nullptr);

		y++; // Move to the next line after finishing this line
	}
}

std::wstring conWstring(std::string inString)
{
	std::wstring wstr(inString.begin(), inString.end());
	return wstr;
}

bool checkFileExists(std::string filename, bool shouldError)
{
	std::ifstream file(filename);
	if (file)
	{
		return true;
	}
	else
	{
		if (shouldError)
		{
			std::string message = filename + " is missing. Please ensure you've installed it in the correct location.";
			LPCSTR cstr = message.c_str();
			MessageBoxA(NULL, cstr, "UG2WhineGen", MB_ICONERROR);
		}
	}

	return false;
}

bool WriteToConsole(LPVOID)
{
	while (true)
	{
		std::vector<std::wstring> curVars;
		curVars.push_back(L"Speed: " + std::to_wstring(gSpeed) + L"X");
		curVars.push_back(L"Gear: " + std::to_wstring(gGear) + L"X");
		curVars.push_back(L"CarName: " + conWstring(gCarName) + L"X");
		curVars.push_back(L"Clutch: " + std::to_wstring(gShiftValue) + L"X");
		curVars.push_back(L"RPM: " + std::to_wstring(gEngineRPM) + L"X");
		curVars.push_back(L"gGlobalEnable: " + std::to_wstring(gGlobalEnable) + L" IsStopped: " + std::to_wstring(IsStopped) +
			L" IsOnFocus: " + std::to_wstring(IsOnFocus) + L" TheGameFlowManager: " + std::to_wstring(TheGameFlowManager) +
			L" sndCurrentLvl: " + std::to_wstring(sndCurrentLvl) + L" gPhoneCall: " + std::to_wstring(gPhoneCall) +
			L" IniMult: " + std::to_wstring(IniMult) + L" gConLine: " + std::to_wstring(gConLine) + L" gWhineEnabled: " +
			std::to_wstring(gWhineEnabled) + L" CarInput: " + conWstring(CarInput) + L" gCutscene: " + std::to_wstring(gCutscene)
		
		);
		renderToConsole(curVars);
		Sleep(16.6666);
	}
}