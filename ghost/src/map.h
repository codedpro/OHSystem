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

#ifndef MAP_H
#define MAP_H

#define MAPSPEED_SLOW			1
#define MAPSPEED_NORMAL			2
#define MAPSPEED_FAST			3

#define MAPVIS_HIDETERRAIN		1
#define MAPVIS_EXPLORED			2
#define MAPVIS_ALWAYSVISIBLE	3
#define MAPVIS_DEFAULT			4

#define MAPOBS_NONE				1
#define MAPOBS_ONDEFEAT			2
#define MAPOBS_ALLOWED			3
#define MAPOBS_REFEREES			4

#define MAPFLAG_TEAMSTOGETHER	1
#define MAPFLAG_FIXEDTEAMS		2
#define MAPFLAG_UNITSHARE		4
#define MAPFLAG_RANDOMHERO		8
#define MAPFLAG_RANDOMRACES		16

#define MAPOPT_HIDEMINIMAP				1 << 0
#define MAPOPT_MODIFYALLYPRIORITIES		1 << 1
#define MAPOPT_MELEE					1 << 2		// the bot cares about this one...
#define MAPOPT_REVEALTERRAIN			1 << 4
#define MAPOPT_FIXEDPLAYERSETTINGS		1 << 5		// and this one...
#define MAPOPT_CUSTOMFORCES				1 << 6		// and this one, the rest don't affect the bot's logic
#define MAPOPT_CUSTOMTECHTREE			1 << 7
#define MAPOPT_CUSTOMABILITIES			1 << 8
#define MAPOPT_CUSTOMUPGRADES			1 << 9
#define MAPOPT_WATERWAVESONCLIFFSHORES	1 << 11
#define MAPOPT_WATERWAVESONSLOPESHORES	1 << 12

#define MAPFILTER_MAKER_USER			1
#define MAPFILTER_MAKER_BLIZZARD		2

#define MAPFILTER_TYPE_MELEE			1
#define MAPFILTER_TYPE_SCENARIO			2

#define MAPFILTER_SIZE_SMALL			1
#define MAPFILTER_SIZE_MEDIUM			2
#define MAPFILTER_SIZE_LARGE			4

#define MAPFILTER_OBS_FULL				1
#define MAPFILTER_OBS_ONDEATH			2
#define MAPFILTER_OBS_NONE				4

#define MAPGAMETYPE_UNKNOWN0			1			// always set except for saved games?
// AuthenticatedMakerBlizzard = 1 << 3
// OfficialMeleeGame = 1 << 5
#define MAPGAMETYPE_SAVEDGAME			1 << 9
#define MAPGAMETYPE_PRIVATEGAME			1 << 11
#define MAPGAMETYPE_MAKERUSER			1 << 13
#define MAPGAMETYPE_MAKERBLIZZARD		1 << 14
#define MAPGAMETYPE_TYPEMELEE			1 << 15
#define MAPGAMETYPE_TYPESCENARIO		1 << 16
#define MAPGAMETYPE_SIZESMALL			1 << 17
#define MAPGAMETYPE_SIZEMEDIUM			1 << 18
#define MAPGAMETYPE_SIZELARGE			1 << 19
#define MAPGAMETYPE_OBSFULL				1 << 20
#define MAPGAMETYPE_OBSONDEATH			1 << 21
#define MAPGAMETYPE_OBSNONE				1 << 22

#include "gameslot.h"

//
// CMap
//

class CMap
{
public:
    CGHost *m_GHost;

private:
    bool m_Valid;
    string m_CFGFile;
    string m_MapPath;							// config value: map path
    BYTEARRAY m_MapSize;						// config value: map size (4 bytes)
    BYTEARRAY m_MapInfo;						// config value: map info (4 bytes) -> this is the real CRC
    BYTEARRAY m_MapCRC;							// config value: map crc (4 bytes) -> this is not the real CRC, it's the "xoro" value
    BYTEARRAY m_MapSHA1;						// config value: map sha1 (20 bytes)
    unsigned char m_MapSpeed;
    unsigned char m_MapVisibility;
    unsigned char m_MapObservers;
    unsigned char m_MapFlags;
    unsigned char m_MapFilterMaker;
    unsigned char m_MapFilterType;
    unsigned char m_MapFilterSize;
    unsigned char m_MapFilterObs;
    uint32_t m_MapOptions;
    BYTEARRAY m_MapWidth;						// config value: map width (2 bytes)
    BYTEARRAY m_MapHeight;						// config value: map height (2 bytes)
    string m_MapType;							// config value: map type (for stats class)
    string m_MapMatchMakingCategory;			// config value: map matchmaking category (for matchmaking)
    string m_MapStatsW3MMDCategory;				// config value: map stats w3mmd category (for saving w3mmd stats)
    string m_MapDefaultHCL;						// config value: map default HCL to use (this should really be specified elsewhere and not part of the map config)
    uint32_t m_MapDefaultPlayerScore;			// config value: map default player score (for matchmaking)
    string m_MapLocalPath;						// config value: map local path
    bool m_MapLoadInGame;
    string m_MapData;							// the map data itself, for sending the map to players
    uint32_t m_MapNumPlayers;
    uint32_t m_MapNumTeams;
    vector<CGameSlot> m_Slots;
    uint32_t m_Alias;
    string m_AliasType;
    string m_PossibleModesToHost;
    bool m_MapTradeAllowed;

public:
    CMap( CGHost *nGHost );
    CMap( CGHost *nGHost, CConfig *CFG, string nCFGFile );
    ~CMap( );

    bool GetValid( )						{
        return m_Valid;
    }
    string GetCFGFile( )					{
        return m_CFGFile;
    }
    string GetMapPath( )					{
        return m_MapPath;
    }
    BYTEARRAY GetMapSize( )					{
        return m_MapSize;
    }
    BYTEARRAY GetMapInfo( )					{
        return m_MapInfo;
    }
    BYTEARRAY GetMapCRC( )					{
        return m_MapCRC;
    }
    BYTEARRAY GetMapSHA1( )					{
        return m_MapSHA1;
    }
    unsigned char GetMapSpeed( )			{
        return m_MapSpeed;
    }
    unsigned char GetMapVisibility( )		{
        return m_MapVisibility;
    }
    unsigned char GetMapObservers( )		{
        return m_MapObservers;
    }
    unsigned char GetMapFlags( )			{
        return m_MapFlags;
    }
    BYTEARRAY GetMapGameFlags( );
    uint32_t GetMapGameType( );
    uint32_t GetMapOptions( )				{
        return m_MapOptions;
    }
    unsigned char GetMapLayoutStyle( );
    BYTEARRAY GetMapWidth( )				{
        return m_MapWidth;
    }
    BYTEARRAY GetMapHeight( )				{
        return m_MapHeight;
    }
    string GetMapType( )					{
        return m_MapType;
    }
    string GetMapMatchMakingCategory( )		{
        return m_MapMatchMakingCategory;
    }
    string GetMapStatsW3MMDCategory( )		{
        return m_MapStatsW3MMDCategory;
    }
    string GetMapDefaultHCL( )				{
        return m_MapDefaultHCL;
    }
    uint32_t GetMapDefaultPlayerScore( )	{
        return m_MapDefaultPlayerScore;
    }
    string GetMapLocalPath( )				{
        return m_MapLocalPath;
    }
    bool GetMapLoadInGame( )				{
        return m_MapLoadInGame;
    }
    string *GetMapData( )					{
        return &m_MapData;
    }
    uint32_t GetMapNumPlayers( )			{
        return m_MapNumPlayers;
    }
    uint32_t GetMapNumTeams( )				{
        return m_MapNumTeams;
    }
    vector<CGameSlot> GetSlots( )			{
        return m_Slots;
    }
    uint32_t GetAlias( )                            {
        return m_Alias;
    }
    string GetPossibleModesToVote( )                {
        return m_PossibleModesToHost;
    }
    bool GetMapTradeAllowed( )                      {
        return m_MapTradeAllowed;
    }

    void Load( CConfig *CFG, string nCFGFile );
    void CheckValid( );
    uint32_t XORRotateLeft( unsigned char *data, uint32_t length );

public:
	static void RegisterPythonClass( );
};

#endif
