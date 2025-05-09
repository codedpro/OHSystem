/**
* Copyright [2013-2014] [OHsystem]
*
* We spent a lot of time writing this code, so show some respect:
* - Do not remove this copyright notice anywhere (bot, website etc.)
* - We do not provide support to those who removed copyright notice
*
* OHSystem is free software: You can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* You can contact the developers on: admin@ohsystem.net
* or join us directly here: http://forum.ohsystem.net/
*
* Visit us also on http://ohsystem.net/ and keep track always of the latest
* features and changes.
*
*
* This is modified from GHOST++: http://ghostplusplus.googlecode.com/
*/

#ifndef REPLAY_H
#define REPLAY_H

#include "gameslot.h"

//
// CReplay
//

class CIncomingAction;

class CReplay : public CPacked
{
public:
    enum BlockID {
        REPLAY_LEAVEGAME		= 0x17,
        REPLAY_FIRSTSTARTBLOCK	= 0x1A,
        REPLAY_SECONDSTARTBLOCK	= 0x1B,
        REPLAY_THIRDSTARTBLOCK	= 0x1C,
        REPLAY_TIMESLOT2		= 0x1E,		// corresponds to W3GS_INCOMING_ACTION2
        REPLAY_TIMESLOT			= 0x1F,		// corresponds to W3GS_INCOMING_ACTION
        REPLAY_CHATMESSAGE		= 0x20,
        REPLAY_CHECKSUM			= 0x22,		// corresponds to W3GS_OUTGOING_KEEPALIVE
        REPLAY_DESYNC			= 0x23
    };

private:
    unsigned char m_HostPID;
    string m_HostName;
    string m_GameName;
    string m_StatString;
    uint32_t m_PlayerCount;
    uint32_t m_MapGameType;
    vector<PIDPlayer> m_Players;
    vector<CGameSlot> m_Slots;
    uint32_t m_RandomSeed;
    unsigned char m_SelectMode;				// also known as the "layout style" elsewhere in this project
    unsigned char m_StartSpotCount;
    queue<BYTEARRAY> m_LoadingBlocks;
    queue<BYTEARRAY> m_Blocks;
    queue<uint32_t> m_CheckSums;
    string m_CompiledBlocks;

public:
    CReplay( );
    virtual ~CReplay( );

    unsigned char GetHostPID( )				{
        return m_HostPID;
    }
    string GetHostName( )					{
        return m_HostName;
    }
    string GetGameName( )					{
        return m_GameName;
    }
    string GetStatString( )					{
        return m_StatString;
    }
    uint32_t GetPlayerCount( )				{
        return m_PlayerCount;
    }
    uint32_t GetMapGameType( )				{
        return m_MapGameType;
    }
    vector<PIDPlayer> GetPlayers( )			{
        return m_Players;
    }
    vector<CGameSlot> GetSlots( )			{
        return m_Slots;
    }
    uint32_t GetRandomSeed( )				{
        return m_RandomSeed;
    }
    unsigned char GetSelectMode( )			{
        return m_SelectMode;
    }
    unsigned char GetStartSpotCount( )		{
        return m_StartSpotCount;
    }
    queue<BYTEARRAY> *GetLoadingBlocks( )	{
        return &m_LoadingBlocks;
    }
    queue<BYTEARRAY> *GetBlocks( )			{
        return &m_Blocks;
    }
    queue<uint32_t> *GetCheckSums( )		{
        return &m_CheckSums;
    }

    void AddPlayer( unsigned char nPID, string nName )		{
        m_Players.push_back( PIDPlayer( nPID, nName ) );
    }
    void SetSlots( vector<CGameSlot> nSlots )				{
        m_Slots = nSlots;
    }
    void SetRandomSeed( uint32_t nRandomSeed )				{
        m_RandomSeed = nRandomSeed;
    }
    void SetSelectMode( unsigned char nSelectMode )			{
        m_SelectMode = nSelectMode;
    }
    void SetStartSpotCount( unsigned char nStartSpotCount )	{
        m_StartSpotCount = nStartSpotCount;
    }
    void SetMapGameType( uint32_t nMapGameType )			{
        m_MapGameType = nMapGameType;
    }
    void SetHostPID( unsigned char nHostPID )				{
        m_HostPID = nHostPID;
    }
    void SetHostName( string nHostName )					{
        m_HostName = nHostName;
    }

    void AddLeaveGame( uint32_t reason, unsigned char PID, uint32_t result );
    void AddLeaveGameDuringLoading( uint32_t reason, unsigned char PID, uint32_t result );
    void AddTimeSlot2( queue<CIncomingAction *> actions );
    void AddTimeSlot( uint16_t timeIncrement, queue<CIncomingAction *> actions );
    void AddChatMessage( unsigned char PID, unsigned char flags, uint32_t chatMode, string message );
    void AddLoadingBlock( BYTEARRAY &loadingBlock );
    void BuildReplay( string gameName, string statString, uint32_t war3Version, uint16_t buildNumber );

    void ParseReplay( bool parseBlocks );

public:
	static void RegisterPythonClass( );
};

#endif
