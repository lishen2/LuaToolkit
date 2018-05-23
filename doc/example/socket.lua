
count = 0

local function timercb()
	if count < 10 then	
		timer.set_timer(500, timercb)
		io.write(string.format("Timer count[%d]\n", count));
		io.write(string.format("ts.get_ts() [%d]", ts.get_ts()))
	else 
		io.write("Timer runout\n");
	end
	count = count + 1
end

timercb();

local function timerdelete()

		id = timer.set_timer(2000, timerdelete)
		io.write(string.format("Timer added, count [%d] id [%d]\n", count, id));
		count = count + 1
		if count > 3 then
			timer.clear_timer(id)
		end
end

--timerdelete();

--[[
client = net.create_client({ip_addr = "fe80::224:54ff:fe5c:aa2e", 
						   port = 9000,
						   family="ipv6"})
client:on("connect", function () io.write("connected\n") 
                                 client:send("1234567890", 10) 
					 end);
client:on("disconnect", function () io.write("disconnected\n") end);
client:on("data", function(data, len) 
					client:send(data, len)
					io.write(string.format("%s\n", data))
				  end)
]]--
--log.init();
--[[
count = 0
server = net.create_server({ip_addr = "any",
							port = 9000,
							family = "ipv6"})
server:on("connect", function(conn)
						log.print("connected")
						count = 0
						conn:on("disconnect", function () log.print("disconnected") end);
						conn:on("data", function(data, len) 
											conn:send(data, len)
											count = count + 1
											log.print(string.format("count [%d], size [%d] ", count, len))
										  end)
					 end)
]]--
--[[
udp = net.create_udp({ip_addr = "fe80::224:54ff:fe5c:aa2d", port=5000, family="ipv6"})
udp:on("data", function(ip, port, data, len) 
					udp:send(ip, port, data, len)
					io.write(string.format("%s\n", data))
				end);
]]--
