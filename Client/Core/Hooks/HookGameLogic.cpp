// Game logic hooks
// Author(s):       iFarbod <ifarbod@outlook.com>
//                  LINK/2012 <dma2012@gmail.com>
//
// Copyright (c) 2015-2017 Project CtNorth
//
// Distributed under the MIT license (See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT)

#include "Precompiled.hpp"
#include <Hooking/HookingUtils.hpp>
#include <Hooking/HookFunction.hpp>

using namespace Util;

static HookFunction hookFunction([]()
{
    // No DirectPlay dependency - Better compatibility for Windows 8+
    MemPatch<u8>(0x74754A, 0xB8);
    MemPatch<u32>(0x74754B, 0x900);

    // Disable CGameLogic::Update
    MakeRET(0x442AD0);

    // No FxMemoryPool_c::Optimize (causes heap corruption/memory leak)
    MakeNOP(0x5C25D3, 5);

    // Disable CCheat::DoCheats
    MakeRET(0x439AF0);
    // Disable CCheat::ToggleCheat
    MakeRET(0x438370);

    // Disable the game's replay system (recording & playing - see CReplay stuff)
    // F1 = Play the last 30 second of gameplay
    // F2 = Save the last 30 second of gameplay to Hard Disk
    // F3 = Play a replay from Hard Disk (replay.rep)
    // F1/F3 = Finish playback (if a replay is playing)
    // CReplay::FinishPlayback
    MakeRET(0x45F050);
    // CReplay::TriggerPlayback
    MakeRET(0x4600F0);
    // CReplay::Update
    MakeRET(0x460500);
    // PlayReplayFromHD
    MakeRET(0x460390);

    // No "JCK_HLP" message
    MakeJMP(0x63E8DF);

    // Disable CPlayerInfo::MakePlayerSafe
    MakeRET(0x56E870, 8);

    // Disable ValidateVersion
    // Contains a stupid check for 'grandtheftauto3' string in peds.col
    MakeRET(0x5BA060);

    // Disable CLoadingScreen::LoadSplashes
    MakeRET(0x5900B0);

    // Disable CShopping::LoadStats
    MakeRET(0x49B6A0);

    // Disable CStats::LoadActionReactionStats
    MakeRET(0x5599B0);

    // Disable CStats::LoadStatUpdateConditions
    MakeRET(0x559860);

    // Disable CStats::IncrementStat
    MakeRET(0x55C180);

    // Disable CStats::DecrementStat
    MakeRET(0x559FA0);

    // Disable CStats::DisplayScriptStatUpdateMessage
    MakeRET(0x55B980);

    // Disable CStats::CheckForStatsMessage
    MakeRET(0x559760);

    // Stop CTaskSimpleCarDrive::ProcessPed from exiting passengers with CTaskComplexSequence
    MakeNOP(0x644C18);
    MemPatch<u8>(0x644C19, 0xE9);

    // Disable CFileLoader::LoadPickup
    MakeRET(0x5B47B0);

    // Disable Interior_c::AddPickups
    MakeRET(0x591F90);

    // Make CEventDamage::IsSameEventForAI return false
    MemPatch<u8>(0x4C01F0, 0xB0);
    MemPatch<u8>(0x4C01F1, 0x00);
    MakeNOP(0x4C01F2, 3);

    // Disable CGridRef::Init
    MakeRET(0x71D4E0);

    // Don't change velocity when colliding with peds in a vehicle
    MemPatch<u32>(0x5F12CA, 0x901CC483);
    MakeNOP(0x5F12CA + 4, 1);

    // Patch ped vehicles damage when flipped
    MemPatch<u16>(0x6A776B, 0xD8DD);
    MakeNOP(0x6A776D, 4);

    // Patch player vehicle damage when flipped
    MemPatch<u16>(0x570E7F, 0xD8DD);
    MakeNOP(0x570E81, 4);

    // No vehicle rewards
    MakeNOP(0x6D16D6);
    MakeJMP(0x6D16D6 + 1, 0x6D17D5);
    MakeNOP(0x6D1999);
    MakeJMP(0x6D1999 + 1, 0x6D1A36);

    // Change money counter format to mobile's one
    CopyStr(0x866C94, "$%d");
    CopyStr(0x866C8C, "$-%d");

    // Increase Streaming_rwObjectInstancesList limit (disables flicker)
    MemPatch<s32>(0x5B8E55, 7500 * 0xC);
    MemPatch<s32>(0x5B8EB0, 7500 * 0xC);

    // SetWindowText
    MemPatch(0x619608, MOD_NAME);
});
