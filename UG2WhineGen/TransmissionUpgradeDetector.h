#pragma once

#include <windows.h>
#include <cstdint>
#include <cstddef>

// Need for Speed Underground 2 v1.2 NTSC (SPEED2.EXE suportado pelo ropi).
// Referencias: NFSU2Unlimiter / InGameFunctions.h e NitrousGauge.h;
// NFSU2-Savegame-Editor / technical.md e perf-parts.ts.
namespace UG2Transmission
{
    constexpr uintptr_t RaceParametersAddress = 0x0089E7A0;
    constexpr uintptr_t GetDriverInfoAddress = 0x005ED960;
    constexpr uintptr_t IsPerformancePartInstalledAddress = 0x00599610;
    constexpr int TransmissionExtremePartId = 11; // TR_6_SPEED_TRANSMISSION, pacote 3.
    constexpr int TransmissionUniqueCategoryIndex = 1; // WEIGHT_REDUCTION=0, TRANSMISSION=1.
    constexpr std::ptrdiff_t PhysicsSpecificationOffsetInRideInfo = 0x2F0;
    constexpr std::ptrdiff_t UniqueFlagsOffsetInPhysicsSpecification = 0x28;

    enum State : LONG { Unavailable = -1, StockOrLower = 0, Level3 = 1, Unique = 2 };

    inline LONG Detect()
    {
        using GetDriverInfoFn = DWORD* (__thiscall*)(DWORD*, int);
        using IsPerfPartInstalledFn = bool (__thiscall*)(BYTE*, int);

        // Os ponteiros sao do executavel do jogo, nao da DLL. Nao consultar fora da partida.
        __try
        {
            auto getDriverInfo = reinterpret_cast<GetDriverInfoFn>(GetDriverInfoAddress);
            auto isPerfPartInstalled = reinterpret_cast<IsPerfPartInstalledFn>(IsPerformancePartInstalledAddress);
            DWORD* driverInfo = getDriverInfo(reinterpret_cast<DWORD*>(RaceParametersAddress), 0);
            if (!driverInfo) { return Unavailable; }

            // NFSU2Unlimiter: DriverInfo + 4 (DWORDs) eh o RideInfo do jogador.
            // RideInfo + 188 (DWORDs) eh PhysicsUpgradeSpecification (0x2F0 bytes).
            BYTE* rideInfo = reinterpret_cast<BYTE*>(driverInfo + 4);
            BYTE* physicsSpec = rideInfo + PhysicsSpecificationOffsetInRideInfo;
            LONG state = StockOrLower;

            if (isPerfPartInstalled(physicsSpec, TransmissionExtremePartId)) { state |= Level3; }
            const DWORD* uniqueFlag = reinterpret_cast<const DWORD*>(physicsSpec + UniqueFlagsOffsetInPhysicsSpecification + TransmissionUniqueCategoryIndex * sizeof(DWORD));
            if (*uniqueFlag != 0) { state |= Unique; }
            return state;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return Unavailable;
        }
    }

    inline bool IsEligible(LONG state, bool onlyUpgraded, bool allowUnique)
    {
        if (!onlyUpgraded) { return true; }
        if (state == Unavailable) { return false; }
        return allowUnique && (state & Unique) != 0;
    }
}
