--打开日志
log.init();
math.randomseed(os.time())

--TCP Server
local function tc_on_connect(conn)
	
	--timer
	local sendtm = 0
	local recvtotm = 0
	--random buf
	local randbuf = ""
	local randlen = 0
	--send ts
	local sendts = 0
	--stat
	local txsize = 0
	local rxsize = 0
	
	--定时器，在指定时间发送数据
	local function sendData()
		
		--生成随机数
		randlen = math.random(10, 1600);
		randbuf = buf.random(randlen);
	
		--发送
		conn:send(randbuf, randlen)
		log.print(string.format("TC: Tx size [%d] ", randlen))
		sendts = ts.get_ts()
		txsize = txsize + randlen
	end
	
	--定时接收检查
	local function recvTimeout()
		--如果距离上次发送时间超过5秒没有收到回复，则再次发送
		if  (0 ~= sendts) and ((ts.get_ts() - sendts) > 5000) then
			sendData()
		end
		--每隔1秒检查一次
		recvtotm = timer.set_timer(1000, recvTimeout)
	end
	
	--注册关闭函数，连接断开时取消定时
	conn:on("disconnect", function () 
							  log.print("TC: Disconnected") 
							  timer.clear_timer(sendtm)
							  timer.clear_timer(recvtotm)
						  end);
						  
	--注册接收函数，收到数据和内存对比
	conn:on("data", function(data, len) 
						--print log
						log.print(string.format("TC: Rx size [%d] ", len))
						
						--对比和发送的数据是否相同
						if (len <= randlen) then
							if (0 ~= buf.memcmp(data, randbuf, len)) then
								log.print("TC: Rx error, mismatch")
								log.print_hex(data, len)
								log.print_hex(randbuf, len)
							end
							randbuf = string.sub(randbuf, len + 1, -1)
							randlen = randlen - len
						else
							randlen = 0
							log.print("TC: Rx error, unsequence")
						end
												
						--注册下次发送
						if 0 == randlen then
							local dealy = math.random(0, 2000)
							sendtm = timer.set_timer(dealy, sendData)
							sendts = 0
							rxsize =  rxsize + len
						end
					  end)
					  
	--注册接收超时检查
	recvtotm = timer.set_timer(1000, recvTimeout)
	
	--log
	log.print("TC: Connected")		
	
	--连接后立刻发送一次数据	
	sendData()
	

end

local function createTcpClient(rip, rport, family)

	--时间花费
	local start_time
	--timer
	local sendtm = 0
	--random buf
	local randbuf = ""
	local randlen = 0
	local sendpos = 0
	local recvpos = 0
	--udp socket
	local client = net.create_client({ip_addr = rip, port = rport, family = family})
	
	local function continueSend()
		local sendlen = randlen - sendpos
		
		if sendlen > 512 then
			sendlen = 512
		end
	
		--发送
		client:send(string.sub(randbuf, sendpos + 1, sendpos + sendlen + 1), sendlen)
		log.print(string.format("TS: Remote[%s:%d], Tx pos [%d] size [%d] ", rip, rport, sendpos, sendlen))
		
		sendpos = sendpos + sendlen;
	end
	
	local function sendFile()
		--生成随机数
		randlen = math.random(5 * 1024, 500 * 1024);
		randbuf = buf.random(randlen);
		sendpos = 0
		recvpos = 0		
		
		--log
		log.print(string.format("TS: Remote[%s:%d], Begin send file, size [%d]", rip, rport, randlen))
		
		--开始发送
		start_time = ts.get_ts()
		continueSend()		
	end
	
	--注册接收事件
	client:on("data", function(data, len) 
						  log.print(string.format("TS: Remote[%s:%d], Rx size [%d] ", rip, rport, len))
						  
						  --如果长度在合法范围内
						  if (recvpos + len <= randlen) then
						      --比较数据是否和发送的相同
							  if 0 ~= buf.memcmp(data, string.sub(randbuf, recvpos + 1, recvpos + len + 1), len) then 
							  	  log.print(string.format("TS: Remote[%s:%d], Rx error, mismatch", rip, rport))
								  log.print_hex(data, len)
								  log.print_hex(string.sub(randbuf, recvpos + 1, len + 1), len)
						      end
							  recvpos = recvpos + len
							  
							  --判断是否处理完整个文件
							  if recvpos == randlen then
							      log.print(string.format("TS: Remote[%s:%d], Finish. time used [%d]", rip, rport,  ts.get_ts() - start_time))
                                  --下次发送1分钟之后
                                  sendtm = timer.set_timer(1 * 60 * 1000, sendFile)
								  return
							  end
							  
							  --如果接收赶上发送进度，则继续发送下一包
							  if recvpos >= sendpos then
								  continueSend();
							  end
						  else 
							log.print(string.format("TS: Remote[%s:%d], Rx error, unsequence", rip, rport))
                            --下次发送5分钟之后
                            sendtm = timer.set_timer(5 * 60 * 1000, sendFile)								  							
						  end
					  end)	
	
	--注册关闭函数，连接断开时取消定时
	client:on("disconnect", function () 
							  log.print(string.format("TS: Remote[%s:%d], Disconnected", rip, rport))
							  timer.clear_timer(sendtm)
							end);
	
	--注册连接事件
	client:on("connect", function () 
							log.print(string.format("TS: Remote[%s:%d], Connected", rip, rport)) 
							
							--首次连上后5s后发送数据
							sendtm = timer.set_timer(5 * 1000, sendFile)
						 end);

	return client
end

local function createUdp(lport, rip, rport, family)

	--random buf
	local randbuf = ""
	local randlen = 0
	--udp socket
	local udp = net.create_udp({ip_addr = "any", port = lport, family = family})
	
	--定时器，在指定时间发送数据
	local function sendData()
		
		--生成随机数
		randlen = math.random(10, 1200);
		randbuf = buf.random(randlen);

		--发送
		udp:send(rip, rport, randbuf, randlen)
		log.print(string.format("UDP: remote[%s:%d], Tx size [%d] ", rip, rport, randlen))
	end

	local function udp_on_data(ip, port, data, len)
		--print log
		log.print(string.format("UDP: remote[%s:%d], Rx size [%d] ", ip, port, len))
		
		--对比和发送的数据是否相同
		if (len ~= randlen) or (0 ~= buf.memcmp(data, randbuf, len)) then			
			log.print(string.format("UDP: remote[%s:%d], Rx error, mismatch", ip, port))
			log.print_hex(data, len)
			log.print_hex(randbuf, randlen)
		end
								
		--注册下次发送
		local dealy = math.random(0, 5000)
		timer.set_timer(dealy, sendData)
	end

	udp:on("data", udp_on_data)
	timer.set_timer(2000, sendData)
	
	return udp
end

local server4 = net.create_server({ip_addr = "any", port = 9000, family = "ipv4"})
local server6 = net.create_server({ip_addr = "any", port = 9000, family = "ipv6"})
server4:on("connect", tc_on_connect)
server6:on("connect", tc_on_connect)
--local client = createTcpClient("192.168.50.118", 9000, "ipv4")
local client = createTcpClient("2001:da8:207::9403", 9000, "ipv6")
local udp = createUdp(8000, "192.168.50.118", 8000, "ipv4")
local udp = createUdp(8001, "2001:da8:207::9403", 8000, "ipv6")
