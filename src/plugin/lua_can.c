#include <Windows.h>
#include <stdint.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "common.h"
#include "lua_can.h"
#include "PCANBasic.h"
#include "PCAN-ISO-TP.h"
#include "PCAN-UDS.h"

#define LUA_CAN_N_SA       ((BYTE) PUDS_ISO_15765_4_ADDR_TEST_EQUIPMENT)
#define LUA_CAN_N_TA       ((BYTE) PUDS_ISO_15765_4_ADDR_ECU_1)
#define LUA_CAN_N_TA_FUNC  ((BYTE) PUDS_ISO_15765_4_ADDR_OBD_FUNCTIONAL)

static TPCANTPHandle g_channel;

static uint32_t g_IDJustSend;

//参数:波特率(数字),TesterID(16进制数字),ECUID(16进制数字),FuncID(16进制数字)
//返回值:成功返回0，失败-1
static int _openCan(lua_State *L)
{
    uint32_t baud;
    uint32_t testerID;
    uint32_t ECUID;
    uint32_t funcID;
    uint16_t baudcan;
	uint8_t  tmp;
	TPCANTPStatus status;
    
	//检查栈空间
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

    //获取参数
	// -4 波特率
	// -3 TesterID
	// -2 ECUID
	// -1 FuncID
	baud     = (uint32_t)luaL_checkinteger(L, -4);
	testerID = (uint32_t)luaL_checkinteger(L, -3);
	ECUID    = (uint32_t)luaL_checkinteger(L, -2);
	funcID   = (uint32_t)luaL_checkinteger(L, -1);

    //转换波特率
    switch(baud){
        case 250:
            baudcan = PCANTP_BAUD_250K;
            break;
        case 500:
            baudcan = PCANTP_BAUD_500K;
            break;
        default:
            baudcan = PCANTP_BAUD_250K;
            break;
    };

    /* init */
    g_channel = PCANTP_USBBUS1;
    status = CANTP_Initialize(g_channel, baudcan, 0, 0, 0);
    if (PCANTP_ERROR_OK != status){
        luaL_error(L, "PCAN initialize failed [%d]\r\n", status);
		return 0;
    }

    /* add mapping */
    status = CANTP_AddMapping(g_channel, testerID, ECUID,
                			PCANTP_ID_CAN_11BIT, PCANTP_FORMAT_NORMAL, PCANTP_MESSAGE_DIAGNOSTIC,
                			LUA_CAN_N_SA, LUA_CAN_N_TA, PCANTP_ADDRESSING_PHYSICAL, 0x00);
	status |= CANTP_AddMapping(g_channel, ECUID, testerID,
                			PCANTP_ID_CAN_11BIT, PCANTP_FORMAT_NORMAL, PCANTP_MESSAGE_DIAGNOSTIC,
                			LUA_CAN_N_TA, LUA_CAN_N_SA, PCANTP_ADDRESSING_PHYSICAL, 0x00);
	status |= CANTP_AddMapping(g_channel, funcID, CAN_ID_NO_MAPPING,
                			PCANTP_ID_CAN_11BIT, PCANTP_FORMAT_NORMAL, PCANTP_MESSAGE_DIAGNOSTIC,
                			LUA_CAN_N_SA, LUA_CAN_N_TA_FUNC, PCANTP_ADDRESSING_FUNCTIONAL, 0x00); 
    if (PCANTP_ERROR_OK != status){
        luaL_error(L, "PCAN add mapping failed, status [%d] TesterID[%d] ECUID[%d], FUNCID[%d]\r\n", 
                   status, testerID, ECUID, funcID);
		return 0;        
    }

	/* hide PCANTP_MESSAGE_INDICATION from CANTP_Read*/
	tmp = PCANTP_MSG_PENDING_HIDE;
	status = CANTP_SetValue(g_channel, PCANTP_PARAM_MSG_PENDING, &tmp, sizeof(tmp));
	if (PCANTP_ERROR_OK != status) {
		luaL_error(L, "PCAN set configuration failed\r\n");
		return 0;
	}

    g_IDJustSend = 0;
	lua_pushinteger (L, 0);
	return 1;
}

static int _closeCan(lua_State *L)
{
	CANTP_Uninitialize(g_channel);
    return 0;
}

//参数:ID(数字),data(string),len(数字)
//返回值:成功返回0，失败-1
static int _sendRaw(lua_State *L)
{
	TPCANTPStatus status;
    TPCANMsg CANMsg;
    const uint8_t *data;
    uint32_t id;
    int len;
    int cpylen;

	//检查栈空间
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

    /* 获取参数 */
    id = (uint32_t)luaL_checkinteger(L, -3);
    data = luaL_checkstring(L, -2);
    len = (int)luaL_checkinteger(L, -1);

    /* 填写报文 */
    if (id > 0x7FF){
        CANMsg.MSGTYPE = PCAN_MESSAGE_EXTENDED;
    } else {
        CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
    }
    CANMsg.ID = id;
    cpylen = len;
    if (cpylen > 8){
        cpylen = 8;
    }
    CANMsg.LEN = cpylen;
    memcpy(CANMsg.DATA, data, cpylen);
    
    /* 发送 */
    status = CAN_Write(g_channel, &CANMsg);
    if (PCANTP_ERROR_OK != status){
		lua_pushinteger(L, -1);  
    } else {
        g_IDJustSend = CANMsg.ID;
    	lua_pushinteger(L, 0);  
    }

    return 1;
}

/* 参数: 超时时间
   返回值：id（数字），报文（字符串），长度（数字）*/
static int _recvRaw(lua_State *L)
{
    TPCANMsg CANMsg;
    TPCANTimestamp CANTimeStamp;
    TPCANStatus status;
    uint32_t ts;
    uint32_t timeout;

	//检查栈空间
	luaL_checkstack(L, 3, "LUA Stack OverFlow");

	//获取参数
	timeout = (unsigned int)luaL_checkinteger(L, -1);

    //读取报文
    ts = GetTickCount();
    do{
        status = CAN_Read(g_channel, &CANMsg, &CANTimeStamp);
        if (PCAN_ERROR_OK == status && g_IDJustSend == CANMsg.ID){
            status = PCAN_ERROR_QRCVEMPTY;
        }
        if (GetTickCount() - ts > timeout) {
            break;
        }
    } while(PCAN_ERROR_QRCVEMPTY == status);

    //返回数据
    if (PCAN_ERROR_OK == status){
        lua_pushinteger(L, CANMsg.ID);
        lua_pushlstring(L, CANMsg.DATA, CANMsg.LEN);
		lua_pushinteger(L, CANMsg.LEN);
    } else {
        lua_pushinteger(L, 0);
        lua_pushnil(L);
        lua_pushinteger(L, 0);
    }
    return 3;
}

//参数:  data(string),len(数字)
//返回值:成功返回0，失败-1
static int _sendIsotp(lua_State *L)
{
	TPCANTPStatus status;
    TPCANTPMsg Message;
    const uint8_t *data;
	const char *addr;
    int len;
    int cpylen;

	//检查栈空间
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

    /* 获取参数 */
	addr = lua_tolstring(L, -3, NULL);
    data = luaL_checkstring(L, -2);
    len = (int)luaL_checkinteger(L, -1);

    /* 填写报文 */
	Message.SA = LUA_CAN_N_SA;
	Message.TA = LUA_CAN_N_TA;
	Message.RA = 0;
	Message.IDTYPE = PCANTP_ID_CAN_11BIT;
	Message.MSGTYPE = PCANTP_MESSAGE_DIAGNOSTIC;
	Message.FORMAT = PCANTP_FORMAT_NORMAL;
	if (NULL != addr && 0 == strcmp(addr, "FUNC")){
		Message.TA_TYPE = PCANTP_ADDRESSING_FUNCTIONAL;
		Message.TA = LUA_CAN_N_TA_FUNC;
	} else {
		Message.TA_TYPE = PCANTP_ADDRESSING_PHYSICAL;
		Message.TA = LUA_CAN_N_TA;
	}
    
    cpylen = len;
    if (cpylen > PCANTP_MESSAGE_MAX_LENGTH){
        cpylen = PCANTP_MESSAGE_MAX_LENGTH;
    }
    Message.LEN = cpylen;
    memcpy(Message.DATA, data, cpylen);
    
    /* 发送 */
    status = CANTP_Write(g_channel, &Message);
    if (PCANTP_ERROR_OK != status){
		lua_pushinteger(L, -1);  
    } else {
    	lua_pushinteger(L, 0);  
    }

    return 1;
}

/* 参数: 超时时间
   返回值：报文（字符串），长度（数字）*/
static int _recvIsotp(lua_State *L)
{
    TPCANTPMsg CANMsg;
    TPCANTPTimestamp CANTimeStamp;
    TPCANStatus status;
    uint32_t ts;
    uint32_t timeout;

	//检查栈空间
	luaL_checkstack(L, 2, "LUA Stack OverFlow");

	//获取参数
	timeout = (unsigned int)luaL_checkinteger(L, -1);

    //读取报文
    ts = GetTickCount();
    do{
        status = CANTP_Read(g_channel, &CANMsg, &CANTimeStamp);
        if (PCANTP_ERROR_OK == status && LUA_CAN_N_SA == CANMsg.SA &&
			((LUA_CAN_N_TA == CANMsg.TA && PCANTP_ADDRESSING_PHYSICAL == CANMsg.TA_TYPE) ||
			 (LUA_CAN_N_TA_FUNC == CANMsg.TA && PCANTP_ADDRESSING_FUNCTIONAL == CANMsg.TA_TYPE))){
            status = PCANTP_ERROR_NO_MESSAGE;
        }
        if (GetTickCount() - ts > timeout) {
            break;
        }
    } while(PCANTP_ERROR_NO_MESSAGE == status);

    //返回数据
    if (PCAN_ERROR_OK == status){
        lua_pushlstring(L, CANMsg.DATA, CANMsg.LEN);
		lua_pushinteger(L, CANMsg.LEN);
    } else {
        lua_pushnil(L);
        lua_pushinteger(L, 0);
    }
    return 2;
}

//参数:  req(string),len(数字)，超时时间（数字）
//返回值:rsp(string),len(数字)
static int _isotpTrans(lua_State *L)
{
	TPCANTPStatus status;
	TPCANTPMsg Message;
	TPCANTPTimestamp CANTimeStamp;
	const uint8_t *data;
	uint32_t len;
	uint32_t timeout;
	uint32_t ts;
	int cpylen;

	//检查栈空间
	luaL_checkstack(L, 2, "LUA Stack OverFlow");

	/* 获取参数 */
	data = luaL_checkstring(L, -3);
	len = (uint32_t)luaL_checkinteger(L, -2);
	timeout = (uint32_t)luaL_checkinteger(L, -1);

	/* 填写报文 */
	Message.SA = LUA_CAN_N_SA;
	Message.TA = LUA_CAN_N_TA;
	Message.TA_TYPE = PCANTP_ADDRESSING_PHYSICAL;
	Message.RA = 0;
	Message.IDTYPE = PCANTP_ID_CAN_11BIT;
	Message.MSGTYPE = PCANTP_MESSAGE_DIAGNOSTIC;
	Message.FORMAT = PCANTP_FORMAT_NORMAL;

	cpylen = len;
	if (cpylen > PCANTP_MESSAGE_MAX_LENGTH) {
		cpylen = PCANTP_MESSAGE_MAX_LENGTH;
	}
	Message.LEN = cpylen;
	memcpy(Message.DATA, data, cpylen);

	/* 发送 */
	status = CANTP_Write(g_channel, &Message);
	if (PCANTP_ERROR_OK != status) {
		lua_pushinteger(L, -1);
	}
	else {
		lua_pushinteger(L, 0);
	}

	//读取报文
	ts = GetTickCount();
	do {
		status = CANTP_Read(g_channel, &Message, &CANTimeStamp);
		if (PCANTP_ERROR_OK == status && LUA_CAN_N_SA == Message.SA &&
			LUA_CAN_N_TA == Message.TA && PCANTP_ADDRESSING_PHYSICAL == Message.TA_TYPE) {
			status = PCANTP_ERROR_NO_MESSAGE;
		}
		if (GetTickCount() - ts > timeout) {
			break;
		}
	} while (PCANTP_ERROR_NO_MESSAGE == status);

	//返回数据
	if (PCAN_ERROR_OK == status) {
		//printf("CCC:recv SA[%hhu] TA[%hhx]\r\n", Message.SA, Message.TA);
		lua_pushlstring(L, Message.DATA, Message.LEN);
		lua_pushinteger(L, Message.LEN);
	}
	else {
		lua_pushnil(L);
		lua_pushinteger(L, 0);
	}

	return 2;
}

static const struct luaL_Reg g_canlib [] = {
	{"open", _openCan},
	{"close", _closeCan},
	{"sendRaw", _sendRaw},
	{"recvRaw", _recvRaw},
	{"sendIsotp", _sendIsotp},
	{"recvIsotp", _recvIsotp},
	{"isotpTrans", _isotpTrans},
	{NULL, NULL} /* sentinel */
};

int CAN_Regist(lua_State *L)
{
	luaL_newlib(L, g_canlib);
	lua_setglobal(L, "can");

	return ERR_OK;
}

