#include "stdafx.h"
#include "stdio.h"
#include "Utils.h"
#include <string>
#include <algorithm>

#define MINIAUDIO_IMPLEMENTATION
#include "..\includes\miniaudio.h"

unsigned int* GetPtr(unsigned int* offsets, int count)
{
	auto ptr = (unsigned int*)offsets[0];
	for (int i = 1, size = count; i < size; i++)
	{
		if (ptr == NULL || *ptr == 0)
		{
			return 0;
		}

		ptr = (unsigned int*)(*ptr + offsets[i]);
	}

	return ptr;
}

float GetRPM()
{
	unsigned int offsets[] = { 0x008900AC, 4, 0x968, 0x58 };
	float* ptr = (float*)GetPtr(offsets, 4);
	if (ptr)
	{
		return *ptr;
	}

	return 0;
}

float GetSpeed()
{
	unsigned int offsets[] = { 0x008900AC, 4, 0x430 };
	float* ptr = (float*)GetPtr(offsets, 3);
	if (ptr)
	{
		return 3.59999 * (*ptr);
	}

	return 0;
}

int GetGear()
{
	//0 = reverse
	//1 = neutral
	//2 = 1 
	//3 = 2
	//4 = 3
	//5 = 4
	//6 = 5
	//7 = 6
	unsigned int offsets[] = { 0x008900AC, 4, 0x434 };
	int* ptr = (int*)GetPtr(offsets, 3);
	if (ptr)
	{
		return *ptr;
	}

	return 0;
}

bool GetShiftValue()
{
	DWORD address = gBase + 0x0042C648;
	unsigned int offsets[] = { address, 0x0, 0x1C, 0x5C8};
	int* ptr = (int*)GetPtr(offsets, 4);
	if (ptr)
	{
		if (*ptr > 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

bool IsPlayerBeingCalled()
{
	unsigned int offsets[] = { 0x008B406C, 0x14C };
	auto ptr = GetPtr(offsets, 2);
	if (ptr)
	{
		return *ptr;
	}

	return 0;
}

std::string GetCarName()
{
	unsigned int offsets[] = { 0x008900AC, 4, 0xC, 0 };
	auto ptr = (char*)GetPtr(offsets, 4);
	if (ptr)
	{
		return ptr;
	}

	return std::string();
}


bool gearJustChanged()
{
	if (gGearOld == 0)
	{
		gGearOld = gGear;
		return false;
	}

	if (skipGear == true)
	{
		gGearOld = gGear;
		skipGear = false;
		return false;
	}

	if (gGear < gGearOld)
	{
		gGearOld = gGear;
	}

	if (gGear > gGearOld)
	{
		gGearOld = gGear;
		return true;
	}

	return false;
}

void processGearbox(void)
{
	if (gGear == 0)
	{
		gPlaySnd = false;
	}
	else
	{
		gPlaySnd = true;
	}
}

void fadeSnd()
{
	if (sndTarget == 0.0)
	{
		sndCurrentLvl = 0.0000000;
		return;
	}

	static float difference;
	if (sndCurrentLvl != sndTarget && gDetectDiff)
	{
		if (sndCurrentLvl < sndTarget)
		{
			difference = (sndTarget - sndCurrentLvl) / 12.0f;
		}
		if (sndCurrentLvl > sndTarget)
		{
			difference = (sndCurrentLvl - sndTarget) / 12.0f;
		}
		gDetectDiff = false;
	}
	if (sndCurrentLvl == sndTarget)
	{
		gDetectDiff = true;
	}

	if (difference < 0.001)
	{
		sndCurrentLvl == sndTarget;
	}

	if (sndCurrentLvl != sndTarget)
	{
		if (sndCurrentLvl < sndTarget)
		{
			sndCurrentLvl += difference;
		}
		if (sndCurrentLvl > sndTarget)
		{
			sndCurrentLvl -= difference;
		}
	}
}

void processVol()
{
	if (IsPaused || gCrashCam)
	{
		sndTarget = 0.0f;
		return;
	}

	if (gPhoneCall)
	{
		sndTarget = gMasterVolSlider * gEngineVolSlider * 0.4;
	}
	else
	{
		sndTarget = gMasterVolSlider * gEngineVolSlider;
	}
}

void modulateSnd(void)
{
	processVol();
	if (gSpeed < 1.00)
	{
		ma_sound_set_pitch(&sound, 0.3f);
		ma_sound_set_pitch(&sound_two, 0.0f);
		ma_sound_set_pitch(&sound_reverse, 0.0f);
	}
	else
	{
		gPitchModTarget = mTz((gGear - 1) * 0.020f);
		if (gearJustChanged())
		{
			gClutchTime = 0;
			fade_volume_to_zero(&sound, &sound_two, 100, ma_sound_get_volume(&sound), ma_sound_get_volume(&sound_two));
			Sleep(200);
			gPitchModifier = gPitchModTarget;
			while (gShiftValue)
			{
				gClutchTime++;
				Sleep(16.6666);
				if (gClutchTime > 30)
				{
					break;
				}
			}
			Sleep(100);
		}
		ma_sound_set_pitch(&sound, mapToRangeExp(GetSpeed(), gPitchMin, gPitchMax, gKMmax));
		ma_sound_set_pitch(&sound_two, 1.00f);
		ma_sound_set_pitch(&sound_reverse, mapToRangeExp(GetSpeed(), gPitchMin, gPitchMax, gKMmax));
	}
	fadeSnd();
	ma_sound_set_volume(&sound, 0.030f * sndCurrentLvl * IniMult * gPlaySnd);
	ma_sound_set_volume(&sound_two, mapToRange(GetSpeed(), 0.00f, (0.015f * sndCurrentLvl * IniMult * gPlaySnd), 140));
	ma_sound_set_volume(&sound_reverse, gReverseVol * IniMult * sndCurrentLvl * !(gPlaySnd));
}

void Update()
{
	_asm pushad;
	TheGameFlowManager = *(DWORD*)_TheGameFlowManager;
	gEngineVolSlider = (*(float*)_EngineVolSlider);
	gMasterVolSlider = (*(float*)_MasterVolSlider);
	IsOnFocus = !(*(bool*)_IsLostFocus);
	IsPaused = (*(bool*)_IsPaused);
	gRacing = (*(bool*)_IsRacing);
	gEngineRPM = GetRPM();
	gGear = GetGear();
	gSpeed = GetSpeed();
	gCarName = GetCarName();
	gShiftValue = GetShiftValue();
	if (!gRacing)
	{
		gPhoneCall = IsPlayerBeingCalled();
	}
	else
	{
		gPhoneCall = (*(bool*)_IsCutscene);
	}
	_asm popad;
}

bool ProcessWhine(LPVOID)
{
	if (hasInit == false)
	{
		Sleep(2000);
		InitData();
		initSnd();
		hasInit = true;
	}

	while (true)
	{
		if (gGlobalEnable == false || IsStopped == false && (IsOnFocus == false || TheGameFlowManager < 6))
		{
			//std::cout << "Game paused/ unfocused or Whine is disabled, stopping all sounds." << std::endl;
			ma_sound_stop(&sound);
			ma_sound_stop(&sound_two);
			ma_sound_stop(&sound_reverse);
			IsStopped = true;
			gWhineEnabled = false;
			HasPlayed = false;
			TriedToPlay = false;
		}

		if (TheGameFlowManager == 6 && IsOnFocus)
		{
			toggleSound();
			if (gGlobalEnable == true)
			{
				IsStopped = false;
				if (TriedToPlay == false)
				{
					HasPlayed = playSnd();
					TriedToPlay = true;
				}
				else if (HasPlayed == true)
				{
					processGearbox();
					modulateSnd();
				}
			}
			else
			{
				skipGear = true;
			}
		}
		Sleep(16.6666);
	}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		gBase = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(gBase);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(gBase + dos->e_lfanew);

		if ((gBase + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - gBase)) == 0x75BCC7) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
		{
			//AllocConsole();
			//if (freopen("CONOUT$", "w", stdout) == NULL)
			//{
			//	MessageBoxA(NULL, "CONOUT failure", "Could not redirect stdout to the console.", MB_ICONERROR);
			//}
			//gConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&WriteToConsole, NULL, 0, NULL);
			injector::MakeCALL(0x004022C0, Update, true); // Hook the whine update func to the main loop
			injector::MakeRET(0x004022C0 + 5, 0, true);
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&ProcessWhine, NULL, 0, NULL);
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&VolSettings, NULL, 0, NULL);
			
		}
		else
		{
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.2 NTSC speed2.exe (4,57 MB (4.800.512 bytes)).", "UG2WhineGen", MB_ICONERROR);
			return FALSE;
		}
	}
	return TRUE;
}
