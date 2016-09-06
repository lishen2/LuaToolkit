#include <stdio.h>
#include <time.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "common.h"
#include "lua_log.h"

static FILE* g_file = NULL;

static void _printf(const char* format, ...)
{
	va_list args;
	va_start (args, format);
	vprintf (format, args);
	if (NULL != g_file){
		vfprintf(g_file, format, args);
	}
	va_end (args);
}

#define DEBUG_HEX_LINE_WIDTH 16
static void _printHexLine(const unsigned char* data, int len)
{
	int i;

	for (i = 0; i < DEBUG_HEX_LINE_WIDTH; ++i){
		if (i < len){
			_printf("%02X ", data[i]);
		} else {
			_printf("   ");
		}
	}

	return;
}

static void _printAsciiLine(const unsigned char* data, int len)
{
	int i;

	for (i = 0; i < len; ++i){
		if (data[i] >= 32 && data[i] <= 126){
			_printf("%c", data[i]);
		} else {
			_printf(".");
		}
	}
}

static void _printHex(const unsigned char* buf, int buflen)
{
	int i;
	int len;
    
	for (i = 0; i < buflen;){

		len = buflen - i;
		if (len > 16){
			len = 16;
		}

        _printf("0x%08X  ", i);
	    _printHexLine(buf + i, len);
        _printf("  ");
		_printAsciiLine(buf + i, len);
		_printf("\r\n");
	
		i += DEBUG_HEX_LINE_WIDTH;
	}

	if (NULL != g_file){
		fflush(g_file);
	}
}

static void _printLog(const char *message)
{
	char timebuf[32];
	struct tm* timep;
	time_t tm;

	//generate time
	tm = time(NULL);
	timep = localtime(&tm);
	snprintf(timebuf, sizeof(timebuf), 
			 "[%04d-%02d-%02d %02d:%02d:%02d] ",
				timep->tm_year + 1900,
				timep->tm_mon + 1,
				timep->tm_mday,
				timep->tm_hour,
				timep->tm_min,
				timep->tm_sec);
	timebuf[sizeof(timebuf) - 1] = 0;

	//print
	_printf("%s%s\n", timebuf, message);

    return;
}

static int _luaInit(lua_State *L)
{
	struct tm* timep;
	char filename[64];
	FILE *fp;
	time_t tm;
	
	tm = time(NULL);
	timep = localtime(&tm);
	snprintf(filename, sizeof(filename), 
			 "log%04d%02d%02d%02d%02d.txt", 
				timep->tm_year + 1900,
				timep->tm_mon + 1,
				timep->tm_mday,
				timep->tm_hour,
				timep->tm_min);
	filename[sizeof(filename) - 1] = 0;

	fp = fopen(filename, "a+");
	if (NULL == fp){
		return ERR_FAILED;
	}

	g_file = fp;

	return 0;
}

static int _luaPrint(lua_State *L)
{
	const char* content;
	//get parameter
	content = luaL_checkstring(L, -1);

	//print
    _printLog(content);
    
	if (NULL != g_file){
		fflush(g_file);
	}

	return 0;
}

static int _luaPrintHex(lua_State *L)
{
	const unsigned char *data;
	int len;

	//get parameter
	data = luaL_checkstring(L, -2);
	len = (int)luaL_checkinteger(L, -1);

	//print
    _printHex(data, len);

	//print to file
	if (NULL != g_file){
		fflush(g_file);
	}

	return 0; 
}

static int _luaPrintHexLine(lua_State *L)
{
	const unsigned char *data;
	int len;
    int i;

	//get parameter
	data = luaL_checkstring(L, -2);
	len = (int)luaL_checkinteger(L, -1);

	for (i = 0; i < len; ++i){
		_printf("%02X ", data[i]);
	}

	//print to file
	if (NULL != g_file){
		fflush(g_file);
	}

	return 0;   
}

static const struct luaL_Reg g_loglib [] = {
	{"init", _luaInit},
	{"print", _luaPrint},
	{"print_hex", _luaPrintHex},
	{"print_hex_line", _luaPrintHexLine},
	{NULL, NULL}
};

int LOG_Regist(lua_State *L)
{
	luaL_newlib(L, g_loglib);
	lua_setglobal(L, "log");

	return ERR_OK;
}

