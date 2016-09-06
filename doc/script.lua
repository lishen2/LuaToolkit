
local function getMsgHandler ()
	local STATE_ADDR = 1
	local STATE_LEN = 2
	local STATE_DATA = 3
	local state = STATE_ADDR
	local msglen = 0
	local msgpos = 0
	--local msgbuf = {}
	
--	return function(data)
--[[	
		if state == STATE_ADDR then
			msglen = 0;
			msgpos = 0;
			msgbuf[msgpos] = data;
			msgpos = msgpos + 1;
			state = STATE_LEN;
		elseif state == STATE_LEN then
			msglen = tonumber(data);
			if msglen >= 4 and msglen < 99 then
				msgbuf[msgpos] = data;
				msgpos = msgpos + 1;
				state = STATE_DATA;
			else 
				state = STATE_ADDR;
			end
		elseif state == STATE_DATA then
			msgbuf[msgpos] = data;
			msgpos = msgpos + 1;
			if (msgpos == len + 1) then
				checkMessage(msgbuf:concat(), msgpos);
				state = STATE_ADDR;
			end
		else
			state = STATE_ADDR;
		end --]]
--	end
end

local function checkMessage(msgbuf, msglen)
	ret = alg.crc16(msgbuf, msglen);
	if 0 == ret then
		onSuccess(msgbuf, msglen);
	else
		onCrcError(msgbuf, msglen);
	end
end

local function onLengthError(length)
	log.print(sring.format("Length Error LEN[%02x]", 
							tonumber(length)));
end

local function onCrcError(msgbuf, msglen)
	hexary = {}
	
	for i = 0, msglen, 1 do
		hexary[i] = string.format("%02x", msgbuf:byte(i));
	end
	
	log.print(string.format("CRC Error [%s]", hexary:concat()));
end

local function onSuccess(msgbuf, msglen)
	hexary = {}
	
	for i = 2, msglen, 1 do
		hexary[i] = string.format("%02x ", msgbuf:byte(i));
	end
	
	log.print(string.format("ADDR[%02x] LEN[%02x] DATA[%s]", 
							msgbuf:byte(0),
							msgbuf:byte(1),
							hexary:concat()));
end

--log.init();
serial.open("COM1", 115200, 8, 1, "none");

local msgHandler = getMsgHandler();

while true do
	resault, n = serial.reads(1, 1000);
	msgHandler(resautl);
end
