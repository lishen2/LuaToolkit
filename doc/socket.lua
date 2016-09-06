--[[
local function start()
	timer.set_timer(500, start)
	io.write("print from lua\n");
end

start()
]]--
--[[
client = net.createClient({ip_addr = "127.0.0.1", 
						   port = 9000,
						   family="ipv4"})
client:on("connect", function () io.write("connected\n") end);
client:on("disconnect", function () io.write("disconnected\n") end);
client:on("data", function(data, len) 
					client:send(data, len)
					io.write(string.format("%s\n", data))
				  end)
]]--

--[[
client = net.create_client({ip_addr = "127.0.0.1", 
						   port = 9000,
						   family="ipv4"})
client:on("connect", function () io.write("connected\n") end);
client:on("disconnect", function () io.write("disconnected\n") end);
client:on("data", function(data, len) 
					client:send(data, len)
					io.write(string.format("%s\n", data))
				  end)

server = net.create_server({ip_addr = "any",
							port = 8000,
							family = "ipv4"})
server:on("connect", function(conn)
						conn:on("connect", function () io.write("connected\n") end);
						conn:on("disconnect", function () io.write("disconnected\n") end);
						conn:on("data", function(data, len) 
											conn:send(data, len)
											io.write(string.format("%s\n", data))
										  end)
					 end)
]]--

udp = net.create_udp({ip_addr = "127.0.0.1", port=5000, family="ipv4"})
udp:on("data", function(ip, port, data, len) 
					udp:send(ip, port, data, len)
					io.write(string.format("%s\n", data))
				end);

