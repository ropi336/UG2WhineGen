#pragma once

#include "stdio.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Global variables
#define _IsRacing 0x0089E7B0
#define _IsCutscene 0x0087619C //0082BCA0
#define _TheGameFlowManager 0x8654A4
#define _IsLostFocus 0x8709E0
#define _IsPaused 0x00874E58//0x008384B8
#define _EngineVolSlider 0x0083AA3C
#define _MasterVolSlider 0x0083AA28
#define _IsRacing 0x0082BC4A
uintptr_t gBase;

HANDLE gConsoleHandle;

bool gGlobalEnable = true;
bool HasPlayed = false, hasInit = false, skipGear = true, gPlaySnd = false, gPhoneCall = false,
gDetectDiff = true, gCrashCam = false, IsPaused, IsStopped, IsOnFocus, gRacing, gCutscene, TriedToPlay,
f1WasPressed;
DWORD TheGameFlowManager;
int gConLine = 13;

bool gShiftValue;
float gEngineRPM = 0.0;
float gPitchModifier = 0.0f, gPitchModTarget = 0.0f;
bool gWhineEnabled = true;
int gClutchTime = 0;
bool gWhineAll = false;
float gSpeed = 0.0f;
float gSpeedLast = 0.0f;
float gPitchMin = 0.3, gPitchMax = 1.0;
float gKMmax = 330.0;
float sndTarget = 1.0f, sndCurrentLvl = 1.0f;
float gEngineVolSlider = 1.0, gMasterVolSlider = 1.0, IniMult = 1.0;
float gReverseVol = 0.050f;
int gShiftDurationMS = 200;
int gGear, gGearOld = 1;
char* gCarNameChar;
std::string gCarName = "";
std::string CarInput;
std::vector<std::string> carList;