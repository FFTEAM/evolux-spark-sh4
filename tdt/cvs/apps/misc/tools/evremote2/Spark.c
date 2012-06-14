/*
 * Spark.c
 *
 * (c) 2010 duckbox project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <termios.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>

#include "global.h"
#include "map.h"
#include "remotes.h"
#include "Spark.h"


#define	SPARK_RC05_PREDATA		"11EE" // Pingulux (green, RC2)
#define	SPARK_RC08_PREDATA		"44BB"
#define	SPARK_RC09_PREDATA		"9966" // Pingulux (red, RC1) and Amiko
#define	SPARK_RC12_PREDATA		"08F7"
#define	SPARK_RC13_PREDATA		"AA55"
#define	SPARK_DEFAULT_PREDATA	"A25D"

static tLongKeyPressSupport cLongKeyPressSupport = {
  10, 120,
};

/* Edision argus-spark RCU */
static tButton cButtonsEdisionSpark[] = {
    {"STANDBY"        , "25", KEY_POWER},
    {"MUTE"           , "85", KEY_MUTE},
    {"V.FORMAT"       , "ad", KEY_V},
    {"TV/SAT"         , "c5", KEY_AUX},

    {"0BUTTON"        , "57", KEY_0},
    {"1BUTTON"        , "b5", KEY_1},
    {"2BUTTON"        , "95", KEY_2},
    {"3BUTTON"        , "bd", KEY_3},
    {"4BUTTON"        , "f5", KEY_4},
    {"5BUTTON"        , "d5", KEY_5},
    {"6BUTTON"        , "fd", KEY_6},
    {"7BUTTON"        , "35", KEY_7},
    {"8BUTTON"        , "15", KEY_8},
    {"9BUTTON"        , "3d", KEY_9},

    {"BACK"           , "7f", KEY_BACK},
    {"INFO"           , "a7", KEY_INFO}, //THIS IS WRONG SHOULD BE KEY_INFO
    {"AUDIO"          , "35", KEY_AUDIO},

    {"VOL+"           , "C7", KEY_VOLUMEUP},
    {"VOL-"           , "DD", KEY_VOLUMEDOWN},
    {"PAGE+"          , "07", KEY_PAGEUP},
    {"PAGE-"          , "5F", KEY_PAGEDOWN},

    {"DOWN"           , "0f", KEY_DOWN},
    {"UP   "          , "27", KEY_UP},
    {"RIGHT"          , "af", KEY_RIGHT},
    {"LEFT"           , "6d", KEY_LEFT},
    {"OK/LIST"        , "2f", KEY_OK},
    {"MENU"           , "65", KEY_MENU},
    {"GUIDE"          , "8f", KEY_EPG},
    {"EXIT"           , "4d", KEY_HOME},
    {"FAV"            , "87", KEY_FAVORITES},

    {"RED"            , "7d", KEY_RED},
    {"GREEN"          , "ff", KEY_GREEN},
    {"YELLOW"         , "3f", KEY_YELLOW},
    {"BLUE"           , "bf", KEY_BLUE},

    {"REWIND"         , "17", KEY_REWIND},
    {"PAUSE"          , "37", KEY_PAUSE},
    {"PLAY"           , "b7", KEY_PLAY},
    {"FASTFORWARD"    , "97", KEY_FASTFORWARD},
    {"RECORD"         , "45", KEY_RECORD},
    {"STOP"           , "f7", KEY_STOP},
    {"SLOWMOTION"     , "5d", KEY_SLOW},
    {"ARCHIVE"        , "75", KEY_ARCHIVE},
    {"SAT"            , "1d", KEY_SAT},
    {"STEPBACK"       , "55", KEY_PREVIOUS},
    {"STEPFORWARD"    , "d7", KEY_NEXT},
    {"MARK"           , "8f", KEY_EPG},
    {"TV/RADIO"       , "77", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCH BUTTON
    {"USB"            , "95", KEY_CLOSE},
    {"TIMER"          , "8d", KEY_TIME},
    {""               , ""  , KEY_NULL},
};

/* Edision argus-sparkPlus RCU */
static tButton cButtonsEdisionSparkPlus[] = {
    {"POWER"          , "87", KEY_POWER},
    {"V.FORMAT"       , "0F", KEY_V},
    {"TV/SAT"         , "2F", KEY_AUX},
    {"TIME"           , "A5", KEY_TIME},
    {"MUTE"           , "C5", KEY_MUTE},
    {"0BUTTON"        , "37", KEY_0},
    {"1BUTTON"        , "A7", KEY_1},
    {"2BUTTON"        , "07", KEY_2},
    {"3BUTTON"        , "E5", KEY_3},
    {"4BUTTON"        , "97", KEY_4},
    {"5BUTTON"        , "27", KEY_5},
    {"6BUTTON"        , "D5", KEY_6},
    {"7BUTTON"        , "B7", KEY_7},
    {"8BUTTON"        , "17", KEY_8},
    {"9BUTTON"        , "F5", KEY_9},
    {"TV/RADIO"       , "CD", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCHB
    {"RECALL"         , "CF", KEY_BACK},
    {"CHANNEL+"       , "ED", KEY_UP},
    {"CHANNEL-"       , "DD", KEY_DOWN},
    {"PAGE+"          , "E7", KEY_PAGEUP},
    {"PAGE-"          , "7B", KEY_PAGEDOWN},
    {"FIND"           , "4D", KEY_FIND},
    {"FOLDER"         , "FB", KEY_ARCHIVE},
    {"VOL+"           , "AF", KEY_VOLUMEUP},
    {"VOL-"           , "9F", KEY_VOLUMEDOWN},
    {"MENU"           , "C7", KEY_MENU},
    {"INFO"           , "1F", KEY_INFO},
    {"UP"             , "3F", KEY_UP},
    {"DOWN"           , "85", KEY_DOWN},
    {"LEFT" 	      , "BF", KEY_LEFT},
    {"RIGHT"          , "FD", KEY_RIGHT},
    {"OK"             , "05", KEY_OK},
    {"EXIT"           , "F9", KEY_HOME},
    {"EPG"            , "45", KEY_EPG},
    {"FAV"            , "F7", KEY_FAVORITES},
    {"SAT"            , "BB", KEY_SAT},
    {"RED"            , "55", KEY_RED},
    {"GREEN"          , "95", KEY_GREEN},
    {"YELLOW"         , "15", KEY_YELLOW},
    {"BLUE"           , "D7", KEY_BLUE},
    {"REC"            , "67", KEY_RECORD},
    {"STOP"           , "77", KEY_STOP},
    {"PLAY"           , "7F", KEY_PLAY},
    {"PAUSE"          , "8D", KEY_PAUSE},
    {"FASTFORWARD"    , "35", KEY_FASTFORWARD},
    {"REWIND"         , "3D", KEY_REWIND},
    {"PREV"           , "6D", KEY_PREVIOUS},
    {"NEXT"           , "3B", KEY_NEXT},
    {"FAST"           , "E7", KEY_F},
    {"SLOW"           , "7B", KEY_SLOW},
    {"PLAY_MODE"      , "B5", KEY_P},
    {"USB"            , "35", KEY_CLOSE},
    {"Tms"            , "2F", KEY_T},
    {"F1"             , "65", KEY_F1},
    {"F2"             , "15", KEY_F1},
    {"F3"             , "D7", KEY_F1},
    {""               , ""  , KEY_NULL},
};

/* spark RC8 */
static tButton cButtonsSparkRc08[] = {
    {"POWER"          , "4D", KEY_POWER},
    {"MUTE"           , "DD", KEY_MUTE},
    {"TIME"           , "ED", KEY_TIME},
    {"V.FORMAT"       , "AD", KEY_V},
    {"F1"             , "0F", KEY_F1},
    {"TV/SAT"         , "37", KEY_AUX},
    {"USB"            , "0D", KEY_CLOSE},
    {"FIND"           , "35", KEY_FIND},

    {"0BUTTON"        , "27", KEY_0},
    {"1BUTTON"        , "7D", KEY_1},
    {"2BUTTON"        , "3F", KEY_2},
    {"3BUTTON"        , "BD", KEY_3},
    {"4BUTTON"        , "5D", KEY_4},
    {"5BUTTON"        , "1F", KEY_5},
    {"6BUTTON"        , "9D", KEY_6},
    {"7BUTTON"        , "55", KEY_7},
    {"8BUTTON"        , "17", KEY_8},
    {"9BUTTON"        , "95", KEY_9},

	{"TV/RADIO" 	  , "65", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCHB
    {"RECALL"         , "A5", KEY_BACK},

    {"MENU"           , "5F", KEY_MENU},
    {"INFO"           , "1D", KEY_INFO},

	{"UP"		      , "9F", KEY_UP},
	{"DOWN"		      , "AF", KEY_DOWN},
	{"LEFT" 	      , "3D", KEY_LEFT},
	{"RIGHT"		  , "7F", KEY_RIGHT},
	{"OK"		      , "BF", KEY_OK},
    {"EXIT"           , "2D", KEY_HOME},
    {"EPG"            , "25", KEY_EPG},

    {"VOL+"           , "8D", KEY_VOLUMEDOWN},
    {"VOL-"           , "B5", KEY_VOLUMEUP},
    {"REC"            , "EF", KEY_RECORD},
    {"PAGE+"          , "B7", KEY_CHANNELDOWN},
    {"PAGE-"          , "77", KEY_CHANNELUP},

	{"FOLDER"		  , "E5", KEY_ARCHIVE},

	{"STOP" 		  , "A7", KEY_STOP},
	{"PLAY" 		  , "75", KEY_PLAY},
	{"PAUSE"		  , "F5", KEY_PAUSE},
	{"FASTFORWARD"	  , "CD", KEY_FASTFORWARD},
	{"REWIND"		  , "D5", KEY_REWIND},
    {"PREV"           , "8F", KEY_PREVIOUS},
    {"NEXT"           , "57", KEY_NEXT},

    {"Tms"            , "4F", KEY_T},

    {"FAST"           , "97", KEY_F},
    {"SLOW"           , "15", KEY_SLOW},
    {"PLAY_MODE"      , "6F", KEY_P},
    {"WHITE"          , "67", KEY_W},

    {"RED"            , "05", KEY_RED},
    {"GREEN"          , "87", KEY_GREEN},
    {"YELLOW"         , "C5", KEY_YELLOW},
    {"BLUE"           , "47", KEY_BLUE},

	{"FAV"			  , "F7", KEY_FAVORITES},
    {"SAT"            , "2F", KEY_SAT},
    {"TTX"            , "DF", KEY_TITLE},
    {"AUDIO"          , "D7", KEY_SUBTITLE},
    {""               , ""  , KEY_NULL},
};
/* Amiko alien-spark RCU */
static tButton cButtonsSparkRc09[] = {
   {"POWER"          , "25", KEY_POWER},
    {"MUTE"           , "85", KEY_MUTE},
    {"TIME"           , "8D", KEY_TIME},
    {"V.FORMAT"       , "AD", KEY_V},
    {"TV/SAT"         , "A5", KEY_AUX},
    {"PICASA"         , "E5", KEY_SUBTITLE},
    {"SHOUTCAST"      , "ED", KEY_AUDIO},
    {"YOUTUBE"        , "CD", KEY_VIDEO},
    {"SPARK"          , "C5", KEY_S},
    {"0BUTTON"        , "57", KEY_0},
    {"1BUTTON"        , "B5", KEY_1},
    {"2BUTTON"        , "95", KEY_2},
    {"3BUTTON"        , "BD", KEY_3},
    {"4BUTTON"        , "F5", KEY_4},
    {"5BUTTON"        , "D5", KEY_5},
    {"6BUTTON"        , "FD", KEY_6},
    {"7BUTTON"        , "35", KEY_7},
    {"8BUTTON"        , "15", KEY_8},
    {"9BUTTON"        , "3D", KEY_9},
    {"TV/RADIO"       , "77", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCHB
    {"RECALL"         , "7F", KEY_BACK},
    {"SAT"            , "9D", KEY_SAT},
    {"FAV"     	      , "45", KEY_FAVORITES},
    {"VOL-"           , "C7", KEY_VOLUMEDOWN},
    {"VOL+"           , "DD", KEY_VOLUMEUP},
    {"PAGE-"          , "07", KEY_PAGEDOWN},
    {"PAGE+"          , "5F", KEY_PAGEUP},
    {"INFO"           , "1D", KEY_INFO},
    {"EPG"            , "87", KEY_EPG},
    {"MENU"           , "65", KEY_MENU},
    {"EXIT"           , "A7", KEY_HOME},
    {"UP"	      , "27", KEY_UP},
    {"DOWN"	      , "0F", KEY_DOWN},
    {"LEFT" 	      , "6D", KEY_LEFT},
    {"RIGHT"	      , "AF", KEY_RIGHT},
    {"OK"	      , "2F", KEY_OK},
    {"FIND"           , "4D", KEY_FIND},
    {"REC"            , "8F", KEY_RECORD},
    {"RED"            , "75", KEY_RED},
    {"GREEN"          , "F7", KEY_GREEN},
    {"YELLOW"         , "37", KEY_YELLOW},
    {"BLUE"           , "B7", KEY_BLUE},
    {"REWIND"	      , "55", KEY_REWIND},
    {"PLAY" 	      , "D7", KEY_PLAY},
    {"PAUSE"	      , "17", KEY_PAUSE},
    {"FASTFORWARD"    , "97", KEY_FASTFORWARD},
    {"FOLDER"	      , "5D", KEY_ARCHIVE},
    {"PLAY_MODE"      , "DF", KEY_P},
    {"USB"            , "1F", KEY_CLOSE},
    {"STOP" 	      , "9F", KEY_STOP},
    {"F1"             , "7D", KEY_HELP},
    {"F2"             , "FF", KEY_PREVIOUS},
    {"F3"             , "3F", KEY_NEXT},
    {"F4"             , "BF", KEY_TITLE},
    {""               , ""  , KEY_NULL},
};

static tButton cButtonsSparkRc12[] = {

    {"MUTE"           , "87", KEY_MUTE},
    {"POWER"          , "45", KEY_POWER},
    {"PLAY_MODE"      , "A7", KEY_P},
    {"V.FORMAT"       , "E5", KEY_V},
    {"TIME"           , "C5", KEY_TIME},
    {"USB"			  , "47", KEY_CLOSE},
	{"FOLDER"		  , "65", KEY_ARCHIVE},

    {"STOP"		 	, "25", KEY_STOP},
    {"PLAY"			, "3D", KEY_PLAY},
    {"PAUSE"		, "1D", KEY_PAUSE},
    {"FASTFORWARD"	, "C7", KEY_FASTFORWARD},
    {"REWIND" 		, "FD", KEY_REWIND},
    {"PREV"			, "BF", KEY_PREVIOUS},
    {"NEXT"			, "E7", KEY_NEXT},
    {"FAST" 		, "67", KEY_F},
    {"SLOW"   		, "9F", KEY_SLOW},

    {"MENU"         , "DD", KEY_MENU},
    {"Tms"			, "BD", KEY_T},
    {"INFO"         , "FF", KEY_INFO},

	{"UP"		   	, "5D", KEY_UP},
	{"DOWN"        	, "55", KEY_DOWN},
	{"LEFT"        	, "1F", KEY_LEFT},
	{"RIGHT"       	, "7F", KEY_RIGHT},
	{"OK"          	, "7D", KEY_OK},

	{"EXIT" 		  , "3F", KEY_HOME},

	{"REC"			  , "9D", KEY_RECORD},
    {"EPG"            , "5F", KEY_EPG},
    {"TV/SAT"		, "D5", KEY_AUX},
    {"RECALL"         , "DF", KEY_BACK},
    {"FIND"           , "95", KEY_FIND},
    {"VOL+"           , "17", KEY_VOLUMEDOWN},
    {"VOL-"           , "37", KEY_VOLUMEUP},

    {"SAT"            , "15", KEY_SAT},
	{"FAV"			  , "35", KEY_FAVORITES},
    {"PAGE+"          , "57", KEY_CHANNELDOWN},
    {"PAGE-"          , "D7", KEY_CHANNELUP},

	{"KEY_0"        , "0D", KEY_0},
	{"KEY_1"        , "F5", KEY_1},
	{"KEY_2"        , "B5", KEY_2},
	{"KEY_3"        , "F7", KEY_3},
	{"KEY_4"        , "CD", KEY_4},
	{"KEY_5"        , "AD", KEY_5},
	{"KEY_6"        , "77", KEY_6},
	{"KEY_7"        , "0F", KEY_7},
	{"KEY_8"        , "8D", KEY_8},
	{"KEY_9"        , "4F", KEY_9},

	{"TV/RADIO" 	  , "ED", KEY_TV2},

	{"RED"			, "2F", KEY_RED},
	{"GREEN"		, "6F", KEY_GREEN},
	{"YELLOW"		, "EF", KEY_YELLOW},
	{"BLUE" 		, "05", KEY_BLUE},
    {"WHITE"        , "2D", KEY_W},
    {""               , ""  , KEY_NULL},
};

/* spark Default */
static tButton cButtonsSparkDefault[] = {
    {"POWER"          , "87", KEY_POWER},
    {"V.FORMAT"       , "0F", KEY_V},
    {"TV/SAT"         , "2F", KEY_AUX},
    {"TIME"           , "65", KEY_TIME},
    {"MUTE"           , "A5", KEY_MUTE},
    {"0BUTTON"        , "37", KEY_0},
    {"1BUTTON"        , "A7", KEY_1},
    {"2BUTTON"        , "07", KEY_2},
    {"3BUTTON"        , "E5", KEY_3},
    {"4BUTTON"        , "97", KEY_4},
    {"5BUTTON"        , "27", KEY_5},
    {"6BUTTON"        , "D5", KEY_6},
    {"7BUTTON"        , "B7", KEY_7},
    {"8BUTTON"        , "17", KEY_8},
    {"9BUTTON"        , "F5", KEY_9},
	{"TV/RADIO" 	  , "CD", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCHB
    {"RECALL"         , "CF", KEY_BACK},
    {"PAGE+"          , "ED", KEY_PAGEDOWN},
    {"PAGE-"          , "DD", KEY_PAGEUP},
    {"FIND"           , "C5", KEY_FIND},
	{"FOLDER"		  , "67", KEY_ARCHIVE},
    {"VOL+"           , "AF", KEY_VOLUMEUP},
    {"VOL-"           , "9F", KEY_VOLUMEDOWN},
    {"MENU"           , "C7", KEY_MENU},
    {"INFO"           , "1F", KEY_INFO},
	{"UP"		      , "3F", KEY_UP},
	{"DOWN"		      , "85", KEY_DOWN},
	{"LEFT" 	      , "BF", KEY_LEFT},
	{"RIGHT"		  , "FD", KEY_RIGHT},
	{"OK"		      , "05", KEY_OK},
    {"EXIT"           , "F9", KEY_HOME},
    {"EPG"            , "45", KEY_EPG},
	{"FAV"			  , "3D", KEY_FAVORITES},
    {"SAT"            , "0D", KEY_SAT},
    {"RED"            , "6D", KEY_RED},
    {"GREEN"          , "8D", KEY_GREEN},
    {"YELLOW"         , "77", KEY_YELLOW},
    {"BLUE"           , "AD", KEY_BLUE},
    {"REC"            , "F7", KEY_RECORD},

    {"STOP"		 	, "BB", KEY_STOP},
    {"PLAY"			, "57", KEY_PLAY},
    {"PAUSE"		, "4D", KEY_PAUSE},
    {"FASTFORWARD"	, "35", KEY_FASTFORWARD},
    {"REWIND" 		, "7F", KEY_REWIND},
    {"PREV"			, "FB", KEY_PREVIOUS},
    {"NEXT"			, "3B", KEY_NEXT},
    {"FAST" 		, "E7", KEY_F},
    {"SLOW"   		, "7B", KEY_SLOW},

    {"PLAY_MODE"	, "B5", KEY_P},
    {"USB"			, "DF", KEY_CLOSE},
    {"Tms"			, "55", KEY_T},
    {"F1"             , "95", KEY_F1},
    {"F2"             , "15", KEY_F1},
    {"F3"             , "D7", KEY_F1},
    {""               , ""  , KEY_NULL},
};

/* GALAXY RC */
static tButton cButtonsGalaxy[] = {
    {"POWER"          , "25", KEY_POWER},
	{"R"              , "A5", KEY_R},
	{"V.FORMAT"       , "AD", KEY_V},
	{"TIME"           , "8D", KEY_TIME},
	{"MUTE"           , "85", KEY_MUTE},
	{"TV/SAT"         , "C5", KEY_AUX},
	{"Tms"            , "E5", KEY_T},
	{"PRESENTATION"   , "ED", KEY_PRESENTATION},
	{"F1"             , "CD", KEY_F1},
	{"0BUTTON"        , "57", KEY_0},
	{"1BUTTON"        , "B5", KEY_1},
	{"2BUTTON"        , "95", KEY_2},
	{"3BUTTON"        , "BD", KEY_3},
	{"4BUTTON"        , "F5", KEY_4},
	{"5BUTTON"        , "D5", KEY_5},
	{"6BUTTON"        , "FD", KEY_6},
	{"7BUTTON"        , "35", KEY_7},
	{"8BUTTON"        , "15", KEY_8},
	{"9BUTTON"        , "3D", KEY_9},
	{"TV/RADIO"       , "77", KEY_TV2},
	{"RECALL"         , "7F", KEY_BACK},
	{"VOL+"           , "C7", KEY_VOLUMEDOWN},
	{"VOL-"           , "DD", KEY_VOLUMEUP},
	{"PAGE-"          , "5F", KEY_PAGEDOWN},
	{"PAGE+"          , "07", KEY_PAGEUP},
	{"FIND"           , "9D", KEY_FIND},
	{"SAT"            , "1D", KEY_SAT},
	{"REC"            , "45", KEY_RECORD},
	{"FAV"            , "87", KEY_FAVORITES},
	{"MENU"           , "65", KEY_MENU},
	{"INFO"           , "A7", KEY_INFO},
	{"EXIT"           , "4D", KEY_EXIT},
	{"EPG"            , "8F", KEY_EPG},
	{"OK"             , "2F", KEY_OK},
	{"UP"             , "27", KEY_UP},
	{"DOWN"           , "0F", KEY_DOWN},
	{"LEFT"           , "6D", KEY_LEFT},
	{"RIGHT"          , "AF", KEY_RIGHT},
	{"FOLDER"         , "75", KEY_ARCHIVE},
	{"STOP"           , "F7", KEY_STOP},
	{"PAUSE"          , "37", KEY_PAUSE},
	{"PLAY"           , "B7", KEY_PLAY},
	{"PREV"           , "55", KEY_PREVIOUS},
	{"NEXT"           , "D7", KEY_NEXT},
	{"REWIND"         , "17", KEY_REWIND},
	{"FORWARD"        , "97", KEY_FORWARD},
	{"USB"            , "9F", KEY_CLOSE},
	{"RED"            , "7D", KEY_RED},
	{"GREEN"          , "FF", KEY_GREEN},
	{"YELLOW"         , "3F", KEY_YELLOW},
	{"BLUE"           , "BF", KEY_BLUE},
	{"PLAY_MODE"      , "1F", KEY_P},
	{"SLOW"           , "5D", KEY_SLOW},
	{"FAST"	          , "DF", KEY_FASTFORWARD},
	{""               , ""  , KEY_NULL},
};

struct keyMap {
	char *KeyName;
	int KeyCode;
};

struct keyMap linuxKeys[] = {
		/* this is lirc-0.9.0/daemons/input_map.inc */
		{"KEY_0", 11},
		{"KEY_102ND", 86},
		{"KEY_10CHANNELSDOWN", 0x1b9},
		{"KEY_10CHANNELSUP", 0x1b8},
		{"KEY_1", 2},
		{"KEY_2", 3},
		{"KEY_3", 4},
		{"KEY_4", 5},
		{"KEY_5", 6},
		{"KEY_6", 7},
		{"KEY_7", 8},
		{"KEY_8", 9},
		{"KEY_9", 10},
		{"KEY_A", 30},
		{"KEY_AB", 0x196},
		{"KEY_ADDRESSBOOK", 0x1ad},
		{"KEY_AGAIN", 129},
		{"KEY_ALTERASE", 222},
		{"KEY_ANGLE", 0x173},
		{"KEY_APOSTROPHE", 40},
		{"KEY_ARCHIVE", 0x169},
		{"KEY_AUDIO", 0x188},
		{"KEY_AUX", 0x186},
		{"KEY_B", 48},
		{"KEY_BACK", 158},
		{"KEY_BACKSLASH", 43},
		{"KEY_BACKSPACE", 14},
		{"KEY_BASSBOOST", 209},
		{"KEY_BATTERY", 236},
		{"KEY_BLUE", 0x191},
		{"KEY_BLUETOOTH", 237},
		{"KEY_BOOKMARKS", 156},
		{"KEY_BREAK", 0x19b},
		{"KEY_BRIGHTNESS_CYCLE", 243},
		{"KEY_BRIGHTNESSDOWN", 224},
		{"KEY_BRIGHTNESSUP", 225},
		{"KEY_BRIGHTNESS_ZERO", 244},
		{"KEY_BRL_DOT10", 0x1fa},
		{"KEY_BRL_DOT1", 0x1f1},
		{"KEY_BRL_DOT2", 0x1f2},
		{"KEY_BRL_DOT3", 0x1f3},
		{"KEY_BRL_DOT4", 0x1f4},
		{"KEY_BRL_DOT5", 0x1f5},
		{"KEY_BRL_DOT6", 0x1f6},
		{"KEY_BRL_DOT7", 0x1f7},
		{"KEY_BRL_DOT8", 0x1f8},
		{"KEY_BRL_DOT9", 0x1f9},
		{"KEY_C", 46},
		{"KEY_CALC", 140},
		{"KEY_CALENDAR", 0x18d},
		{"KEY_CAMERA", 212},
		{"KEY_CANCEL", 223},
		{"KEY_CAPSLOCK", 58},
		{"KEY_CD", 0x17f},
		{"KEY_CHANNEL", 0x16b},
		{"KEY_CHANNELDOWN", 0x193},
		{"KEY_CHANNELUP", 0x192},
		{"KEY_CHAT", 216},
		{"KEY_CLEAR", 0x163},
		{"KEY_CLOSE", 206},
		{"KEY_CLOSECD", 160},
		{"KEY_COFFEE", 152},
		{"KEY_COMMA", 51},
		{"KEY_COMPOSE", 127},
		{"KEY_COMPUTER", 157},
		{"KEY_CONFIG", 171},
		{"KEY_CONNECT", 218},
		{"KEY_CONTEXT_MENU", 0x1b6},
		{"KEY_COPY", 133},
		{"KEY_CUT", 137},
		{"KEY_CYCLEWINDOWS", 154},
		{"KEY_D", 32},
		{"KEY_DASHBOARD", 204},
		{"KEY_DATABASE", 0x1aa},
		{"KEY_DEL_EOL", 0x1c0},
		{"KEY_DEL_EOS", 0x1c1},
		{"KEY_DELETE", 111},
		{"KEY_DELETEFILE", 146},
		{"KEY_DEL_LINE", 0x1c3},
		{"KEY_DIGITS", 0x19d},
		{"KEY_DIRECTION", 153},
		{"KEY_DIRECTORY", 0x18a},
		{"KEY_DISPLAY_OFF", 245},
		{"KEY_DISPLAYTOGGLE", 0x1af},
		{"KEY_DOCUMENTS", 235},
		{"KEY_DOLLAR", 0x1b2},
		{"KEY_DOT", 52},
		{"KEY_DOWN", 108},
		{"KEY_DVD", 0x185},
		{"KEY_E", 18},
		{"KEY_EDIT", 176},
		{"KEY_EDITOR", 0x1a6},
		{"KEY_EJECTCD", 161},
		{"KEY_EJECTCLOSECD", 162},
		{"KEY_EMAIL", 215},
		{"KEY_END", 107},
		{"KEY_ENTER", 28},
		{"KEY_EPG", 0x16d},
		{"KEY_EQUAL", 13},
		{"KEY_ESC", 1},
		{"KEY_EURO", 0x1b3},
		{"KEY_EXIT", 174},
		{"KEY_F10", 68},
		{"KEY_F11", 87},
		{"KEY_F12", 88},
		{"KEY_F13", 183},
		{"KEY_F14", 184},
		{"KEY_F15", 185},
		{"KEY_F1", 59},
		{"KEY_F16", 186},
		{"KEY_F17", 187},
		{"KEY_F18", 188},
		{"KEY_F19", 189},
		{"KEY_F20", 190},
		{"KEY_F21", 191},
		{"KEY_F22", 192},
		{"KEY_F23", 193},
		{"KEY_F24", 194},
		{"KEY_F2", 60},
		{"KEY_F", 33},
		{"KEY_F3", 61},
		{"KEY_F4", 62},
		{"KEY_F5", 63},
		{"KEY_F6", 64},
		{"KEY_F7", 65},
		{"KEY_F8", 66},
		{"KEY_F9", 67},
		{"KEY_FASTFORWARD", 208},
		{"KEY_FAVORITES", 0x16c},
		{"KEY_FILE", 144},
		{"KEY_FINANCE", 219},
		{"KEY_FIND", 136},
		{"KEY_FIRST", 0x194},
		{"KEY_FN", 0x1d0},
		{"KEY_FN_1", 0x1de},
		{"KEY_FN_2", 0x1df},
		{"KEY_FN_B", 0x1e4},
		{"KEY_FN_D", 0x1e0},
		{"KEY_FN_E", 0x1e1},
		{"KEY_FN_ESC", 0x1d1},
		{"KEY_FN_F", 0x1e2},
		{"KEY_FN_F10", 0x1db},
		{"KEY_FN_F1", 0x1d2},
		{"KEY_FN_F11", 0x1dc},
		{"KEY_FN_F12", 0x1dd},
		{"KEY_FN_F2", 0x1d3},
		{"KEY_FN_F3", 0x1d4},
		{"KEY_FN_F4", 0x1d5},
		{"KEY_FN_F5", 0x1d6},
		{"KEY_FN_F6", 0x1d7},
		{"KEY_FN_F7", 0x1d8},
		{"KEY_FN_F8", 0x1d9},
		{"KEY_FN_F9", 0x1da},
		{"KEY_FN_S", 0x1e3},
		{"KEY_FORWARD", 159},
		{"KEY_FORWARDMAIL", 233},
		{"KEY_FRAMEBACK", 0x1b4},
		{"KEY_FRAMEFORWARD", 0x1b5},
		{"KEY_FRONT", 132},
		{"KEY_G", 34},
		{"KEY_GAMES", 0x1a1},
		{"KEY_GOTO", 0x162},
		{"KEY_GRAPHICSEDITOR", 0x1a8},
		{"KEY_GRAVE", 41},
		{"KEY_GREEN", 0x18f},
		{"KEY_H", 35},
		{"KEY_HANGEUL", 122},
		{"KEY_HANJA", 123},
		{"KEY_HELP", 138},
		{"KEY_HENKAN", 92},
		{"KEY_HIRAGANA", 91},
		{"KEY_HOME", 102},
		{"KEY_HOMEPAGE", 172},
		{"KEY_HP", 211},
		{"KEY_I", 23},
		{"KEY_INFO", 0x166},
		{"KEY_INSERT", 110},
		{"KEY_INS_LINE", 0x1c2},
		{"KEY_ISO", 170},
		{"KEY_J", 36},
		{"KEY_K", 37},
		{"KEY_KATAKANA", 90},
		{"KEY_KATAKANAHIRAGANA", 93},
		{"KEY_KBDILLUMDOWN", 229},
		{"KEY_KBDILLUMTOGGLE", 228},
		{"KEY_KBDILLUMUP", 230},
		{"KEY_KEYBOARD", 0x176},
		{"KEY_KP0", 82},
		{"KEY_KP1", 79},
		{"KEY_KP2", 80},
		{"KEY_KP3", 81},
		{"KEY_KP4", 75},
		{"KEY_KP5", 76},
		{"KEY_KP6", 77},
		{"KEY_KP7", 71},
		{"KEY_KP8", 72},
		{"KEY_KP9", 73},
		{"KEY_KPASTERISK", 55},
		{"KEY_KPCOMMA", 121},
		{"KEY_KPDOT", 83},
		{"KEY_KPENTER", 96},
		{"KEY_KPEQUAL", 117},
		{"KEY_KPJPCOMMA", 95},
		{"KEY_KPLEFTPAREN", 179},
		{"KEY_KPMINUS", 74},
		{"KEY_KPPLUS", 78},
		{"KEY_KPPLUSMINUS", 118},
		{"KEY_KPRIGHTPAREN", 180},
		{"KEY_KPSLASH", 98},
		{"KEY_L", 38},
		{"KEY_LANGUAGE", 0x170},
		{"KEY_LAST", 0x195},
		{"KEY_LEFT", 105},
		{"KEY_LEFTALT", 56},
		{"KEY_LEFTBRACE", 26},
		{"KEY_LEFTCTRL", 29},
		{"KEY_LEFTMETA", 125},
		{"KEY_LEFTSHIFT", 42},
		{"KEY_LINEFEED", 101},
		{"KEY_LIST", 0x18b},
		{"KEY_LOGOFF", 0x1b1},
		{"KEY_M", 50},
		{"KEY_MACRO", 112},
		{"KEY_MAIL", 155},
		{"KEY_MAX", 0x2ff},
		{"KEY_MEDIA", 226},
		{"KEY_MEDIA_REPEAT", 0x1b7},
		{"KEY_MEMO", 0x18c},
		{"KEY_MENU", 139},
		{"KEY_MESSENGER", 0x1ae},
		{"KEY_MHP", 0x16f},
		{"KEY_MINUS", 12},
		{"KEY_MODE", 0x175},
		{"KEY_MOVE", 175},
		{"KEY_MP3", 0x187},
		{"KEY_MSDOS", 151},
		{"KEY_MUHENKAN", 94},
		{"KEY_MUTE", 113},
		{"KEY_N", 49},
		{"KEY_NEW", 181},
		{"KEY_NEWS", 0x1ab},
		{"KEY_NEXT", 0x197},
		{"KEY_NEXTSONG", 163},
		{"KEY_NUMERIC_0", 0x200},
		{"KEY_NUMERIC_1", 0x201},
		{"KEY_NUMERIC_2", 0x202},
		{"KEY_NUMERIC_3", 0x203},
		{"KEY_NUMERIC_4", 0x204},
		{"KEY_NUMERIC_5", 0x205},
		{"KEY_NUMERIC_6", 0x206},
		{"KEY_NUMERIC_7", 0x207},
		{"KEY_NUMERIC_8", 0x208},
		{"KEY_NUMERIC_9", 0x209},
		{"KEY_NUMERIC_POUND", 0x20b},
		{"KEY_NUMERIC_STAR", 0x20a},
		{"KEY_NUMLOCK", 69},
		{"KEY_O", 24},
		{"KEY_OK", 0x160},
		{"KEY_OPEN", 134},
		{"KEY_OPTION", 0x165},
		{"KEY_P", 25},
		{"KEY_PAGEDOWN", 109},
		{"KEY_PAGEUP", 104},
		{"KEY_PASTE", 135},
		{"KEY_PAUSE", 119},
		{"KEY_PAUSECD", 201},
		{"KEY_PC", 0x178},
		{"KEY_PHONE", 169},
		{"KEY_PLAY", 207},
		{"KEY_PLAYCD", 200},
		{"KEY_PLAYER", 0x183},
		{"KEY_PLAYPAUSE", 164},
		{"KEY_POWER", 116},
		{"KEY_POWER2", 0x164},
		{"KEY_PRESENTATION", 0x1a9},
		{"KEY_PREVIOUS", 0x19c},
		{"KEY_PREVIOUSSONG", 165},
		{"KEY_PRINT", 210},
		{"KEY_PROG1", 148},
		{"KEY_PROG2", 149},
		{"KEY_PROG3", 202},
		{"KEY_PROG4", 203},
		{"KEY_PROGRAM", 0x16a},
		{"KEY_PROPS", 130},
		{"KEY_PVR", 0x16e},
		{"KEY_Q", 16},
		{"KEY_QUESTION", 214},
		{"KEY_R", 19},
		{"KEY_RADIO", 0x181},
		{"KEY_RECORD", 167},
		{"KEY_RED", 0x18e},
		{"KEY_REDO", 182},
		{"KEY_REFRESH", 173},
		{"KEY_REPLY", 232},
		{"KEY_RESERVED", 0},
		{"KEY_RESTART", 0x198},
		{"KEY_REWIND", 168},
		{"KEY_RFKILL", 0x20c},
		{"KEY_RIGHT", 106},
		{"KEY_RIGHTALT", 100},
		{"KEY_RIGHTBRACE", 27},
		{"KEY_RIGHTCTRL", 97},
		{"KEY_RIGHTMETA", 126},
		{"KEY_RIGHTSHIFT", 54},
		{"KEY_RO", 89},
		{"KEY_S", 31},
		{"KEY_SAT", 0x17d},
		{"KEY_SAT2", 0x17e},
		{"KEY_SAVE", 234},
		{"KEY_SCALE", 120},
		{"KEY_SCREEN", 0x177},
		{"KEY_SCROLLDOWN", 178},
		{"KEY_SCROLLLOCK", 70},
		{"KEY_SCROLLUP", 177},
		{"KEY_SEARCH", 217},
		{"KEY_SELECT", 0x161},
		{"KEY_SEMICOLON", 39},
		{"KEY_SEND", 231},
		{"KEY_SENDFILE", 145},
		{"KEY_SETUP", 141},
		{"KEY_SHOP", 221},
		{"KEY_SHUFFLE", 0x19a},
		{"KEY_SLASH", 53},
		{"KEY_SLEEP", 142},
		{"KEY_SLOW", 0x199},
		{"KEY_SOUND", 213},
		{"KEY_SPACE", 57},
		{"KEY_SPELLCHECK", 0x1b0},
		{"KEY_SPORT", 220},
		{"KEY_SPREADSHEET", 0x1a7},
		{"KEY_STOP", 128},
		{"KEY_STOPCD", 166},
		{"KEY_SUBTITLE", 0x172},
		{"KEY_SUSPEND", 205},
		{"KEY_SWITCHVIDEOMODE", 227},
		{"KEY_SYSRQ", 99},
		{"KEY_T", 20},
		{"KEY_TAB", 15},
		{"KEY_TAPE", 0x180},
		{"KEY_TEEN", 0x19e},
		{"KEY_TEXT", 0x184},
		{"KEY_TIME", 0x167},
		{"KEY_TITLE", 0x171},
		{"KEY_TUNER", 0x182},
		{"KEY_TV", 0x179},
		{"KEY_TV2", 0x17a},
		{"KEY_TWEN", 0x19f},
		{"KEY_U", 22},
		{"KEY_UNDO", 131},
		{"KEY_UNKNOWN", 240},
		{"KEY_UP", 103},
		{"KEY_UWB", 239},
		{"KEY_V", 47},
		{"KEY_VCR", 0x17b},
		{"KEY_VCR2", 0x17c},
		{"KEY_VENDOR", 0x168},
		{"KEY_VIDEO", 0x189},
		{"KEY_VIDEO_NEXT", 241},
		{"KEY_VIDEOPHONE", 0x1a0},
		{"KEY_VIDEO_PREV", 242},
		{"KEY_VOICEMAIL", 0x1ac},
		{"KEY_VOLUMEDOWN", 114},
		{"KEY_VOLUMEUP", 115},
		{"KEY_W", 17},
		{"KEY_WAKEUP", 143},
		{"KEY_WIMAX", 246},
		{"KEY_WLAN", 238},
		{"KEY_WORDPROCESSOR", 0x1a5},
		{"KEY_WWW", 150},
		{"KEY_X", 45},
		{"KEY_XFER", 147},
		{"KEY_Y", 21},
		{"KEY_YELLOW", 0x190},
		{"KEY_YEN", 124},
		{"KEY_Z", 44},
		{"KEY_ZENKAKUHANKAKU", 85},
		{"KEY_ZOOM", 0x174},
		{"KEY_ZOOMIN", 0x1a2},
		{"KEY_ZOOMOUT", 0x1a3},
		{"KEY_ZOOMRESET", 0x1a4},
		{"BTN_0", 0x100},
		{"BTN_1", 0x101},
		{"BTN_2", 0x102},
		{"BTN_3", 0x103},
		{"BTN_4", 0x104},
		{"BTN_5", 0x105},
		{"BTN_6", 0x106},
		{"BTN_7", 0x107},
		{"BTN_8", 0x108},
		{"BTN_9", 0x109},
		{"BTN_A", 0x130},
		{"BTN_B", 0x131},
		{"BTN_BACK", 0x116},
		{"BTN_BASE", 0x126},
		{"BTN_BASE2", 0x127},
		{"BTN_BASE3", 0x128},
		{"BTN_BASE4", 0x129},
		{"BTN_BASE5", 0x12a},
		{"BTN_BASE6", 0x12b},
		{"BTN_C", 0x132},
		{"BTN_DEAD", 0x12f},
		{"BTN_DIGI", 0x140},
		{"BTN_EXTRA", 0x114},
		{"BTN_FORWARD", 0x115},
		{"BTN_GAMEPAD", 0x130},
		{"BTN_GEAR_DOWN", 0x150},
		{"BTN_GEAR_UP", 0x151},
		{"BTN_JOYSTICK", 0x120},
		{"BTN_LEFT", 0x110},
		{"BTN_MIDDLE", 0x112},
		{"BTN_MISC", 0x100},
		{"BTN_MODE", 0x13c},
		{"BTN_MOUSE", 0x110},
		{"BTN_PINKIE", 0x125},
		{"BTN_RIGHT", 0x111},
		{"BTN_SELECT", 0x13a},
		{"BTN_SIDE", 0x113},
		{"BTN_START", 0x13b},
		{"BTN_STYLUS", 0x14b},
		{"BTN_STYLUS2", 0x14c},
		{"BTN_TASK", 0x117},
		{"BTN_THUMB", 0x121},
		{"BTN_THUMB2", 0x122},
		{"BTN_THUMBL", 0x13d},
		{"BTN_THUMBR", 0x13e},
		{"BTN_TL", 0x136},
		{"BTN_TL2", 0x138},
		{"BTN_TOOL_AIRBRUSH", 0x144},
		{"BTN_TOOL_BRUSH", 0x142},
		{"BTN_TOOL_DOUBLETAP", 0x14d},
		{"BTN_TOOL_FINGER", 0x145},
		{"BTN_TOOL_LENS", 0x147},
		{"BTN_TOOL_MOUSE", 0x146},
		{"BTN_TOOL_PEN", 0x140},
		{"BTN_TOOL_PENCIL", 0x143},
		{"BTN_TOOL_QUADTAP", 0x14f},
		{"BTN_TOOL_RUBBER", 0x141},
		{"BTN_TOOL_TRIPLETAP", 0x14e},
		{"BTN_TOP", 0x123},
		{"BTN_TOP2", 0x124},
		{"BTN_TOUCH", 0x14a},
		{"BTN_TR", 0x137},
		{"BTN_TR2", 0x139},
		{"BTN_TRIGGER", 0x120},
		{"BTN_WHEEL", 0x150},
		{"BTN_X", 0x133},
		{"BTN_Y", 0x134},
		{"BTN_Z", 0x135},
		{NULL, -1}
};

static int lookupKey(char *keyname){
	struct keyMap *l = linuxKeys;
	while (l->KeyName && strcmp(l->KeyName, keyname))
		l++;
	return l->KeyCode;
}

#define STB_ID_GOLDENMEDIA_GM990		"09:00:07"
#define STB_ID_EDISION_PINGULUX			"09:00:08"
#define STB_ID_AMIKO_ALIEN_SDH8900		"09:00:0A"
#define STB_ID_GALAXYINNOVATIONS_S8120	"09:00:0B"

static tButton *pSparkGetButton(char *pData)
{
	if (!strncasecmp(pData, SPARK_RC05_PREDATA, sizeof(SPARK_RC05_PREDATA)))
		return cButtonsEdisionSpark;
	if (!strncasecmp(pData, SPARK_RC08_PREDATA, sizeof(SPARK_RC08_PREDATA)))
		return cButtonsSparkRc08;
	if (!strncasecmp(pData, SPARK_RC09_PREDATA, sizeof(SPARK_RC09_PREDATA))) {
		static tButton *cButtons = NULL;
		if (!cButtons) {
			int fn = open("/proc/cmdline", O_RDONLY);
			if (fn > -1) {
				char procCmdLine[1024];
				int len = read(fn, procCmdLine, sizeof(procCmdLine) - 1);
				if (len > 0) {
					procCmdLine[len] = 0;
					if (strstr(procCmdLine, "STB_ID=" STB_ID_EDISION_PINGULUX))
						cButtons = cButtonsEdisionSpark;
					if (strstr(procCmdLine, "STB_ID=" STB_ID_GALAXYINNOVATIONS_S8120))
						cButtons = cButtonsGalaxy;
				}
				close(fn);
			}
			if (!cButtons)
				cButtons = cButtonsSparkRc09; /* Amiko Alien 8900 */
		}
		return cButtons;
	}
	if (!strncasecmp(pData, SPARK_DEFAULT_PREDATA, sizeof(SPARK_DEFAULT_PREDATA)))
		return cButtonsSparkDefault;
	if (!strncasecmp(pData, SPARK_RC12_PREDATA, sizeof(SPARK_RC12_PREDATA)))
		return cButtonsSparkRc12;
	if (!strncasecmp(pData, SPARK_RC13_PREDATA, sizeof(SPARK_RC13_PREDATA)))
		return cButtonsEdisionSparkPlus;
	return NULL;
}


/* fixme: move this to a structure and
 * use the private structure of RemoteControl_t
 */
static struct sockaddr_un  vAddr;

static int pInit(Context_t* context, int argc, char* argv[]) {

    int vHandle;

    vAddr.sun_family = AF_UNIX;
    strcpy(vAddr.sun_path, "/var/run/lirc/lircd");

    vHandle = socket(AF_UNIX,SOCK_STREAM, 0);

    if(vHandle == -1)  {
        perror("socket");
        return -1;
    }

    if(connect(vHandle,(struct sockaddr *)&vAddr,sizeof(vAddr)) == -1)
    {
        perror("connect");
        return -1;
    }

    if (argc >= 2)
    {
       cLongKeyPressSupport.period = atoi(argv[1]);
    }
    
    if (argc >= 3)
    {
       cLongKeyPressSupport.delay = atoi(argv[2]);
    }

    return vHandle;
}

static int pShutdown(Context_t* context ) {

    close(context->fd);

    return 0;
}

static int pNotification(Context_t* context, const int cOn);

static int pRead(Context_t* context ) {
    char                vBuffer[128];
    char                vData[10];
    int                 vCurrentCode  = -1;
	tButton 			*cButtons = cButtonsEdisionSpark;
	static FILE *Fd = NULL;
	if (!Fd)
		Fd = fdopen(context->fd, "r");
	if (!Fd)
		return -1;

#if 0
0000000099667a85 00 KEY_XXXX RCNAME
0000000099667a85 01 KEY_XXXX RCNAME
              ^^keycode
           ^^
       ^^^^^predata
#endif

	if (fgets(vBuffer, sizeof(vBuffer), Fd)) {
		char keyname[sizeof(vBuffer)];
		int updown;
		if (2 == sscanf(vBuffer, "%*s %o %s %*s", &updown, keyname)) {
				pNotification(context, 1);
				vCurrentCode = lookupKey(keyname);
				if (vCurrentCode == -1) {
						// get RC code
						vData[0] = vBuffer[8];
						vData[1] = vBuffer[9];
						vData[2] = vBuffer[10];
						vData[3] = vBuffer[11];
						vData[4] = '\0';
						// get RC
						cButtons = pSparkGetButton(vData);
						vData[0] = vBuffer[14];
						vData[1] = vBuffer[15];
						vData[2] = '\0';
						vCurrentCode = getInternalCode(cButtons, vData);
				}
				static int nextflag = 0;
				if (updown == 0)
						nextflag++;
				vCurrentCode += (nextflag << 16);
		}
	}

    return vCurrentCode;
}

static int pNotification(Context_t* context, const int cOn) {

    struct aotom_ioctl_data vfd_data;
    static int ioctl_fd = -1;
    if (ioctl_fd < 0)
		ioctl_fd = open("/dev/vfd", O_RDONLY);
    if (ioctl_fd < 0)
		return -1;

    if(cOn)
    {
       vfd_data.u.led.led_nr = 1;
       vfd_data.u.led.on = 10;
       ioctl(ioctl_fd, VFDSETLED, &vfd_data);
    }

    return 0;
}

RemoteControl_t Spark_RC = {
	"Spark RemoteControl",
	Spark,
	&pInit,
	&pShutdown,
	&pRead,
	NULL, //&pNotification,
	cButtonsEdisionSpark,
	NULL,
	NULL,
  	1,
  	&cLongKeyPressSupport,
};

// vim:ts=4
