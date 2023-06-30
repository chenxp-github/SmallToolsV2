require("common")
require("cothread")

LocalUdpConnection = class();

function LocalUdpConnection:ctor(host_peer)
    self.host_peer = host_peer;
    self.forwarding_thread = nil;
	self.from_peer_name = "_";
end

function LocalUdpConnection:StartServerForwarding(socket,handle,bind_port)
	self.socket = socket;
    self.handle = handle;
    self.bind_port = bind_port;
    local function _server_forwarding_thread(thread);
        self:ServerForwardingThread(thread)
    end
    self.forwarding_thread = CoThread.new();
    self.forwarding_thread:Start(_server_forwarding_thread);
end

function LocalUdpConnection:ServerForwardingThread(thread)  
	local mem = new_mem();
	while self.socket do
		for i=1,1000, 1 do
			if self.socket:RecvMsg(mem) then
				self.host_peer:SetDestPeerName(self.from_peer_name);
				self.host_peer:SendData(self.handle,mem);
			else
				break
			end
		end
		thread:Sleep(1);
	end  
end

function LocalUdpConnection:Close()
    if self.socket then
        self.socket:Destroy();
        self.socket = nil;
    end
end

function LocalUdpConnection:SendLocalData(ip,port,data)
	if not self.socket then
		return -1;
	end
	self.socket:SetDestIpAndPort(ip,port);
	if self.socket:SendMsg(data) then
		return data:GetSize();
	end
	return 0;
end

function LocalUdpConnection:StartClientForwarding(local_port,remote_port,dest_ip,dest_port)
	self.bind_port = local_port;
	self.remote_port = remote_port;
	self.remote_dest_ip = dest_ip;
	self.remote_dest_port = dest_port;

    local function _client_forwarding_thread(thread);
		while true do
        	self:ClientForwardingThread(thread)
			thread:Sleep(2000);
			printfnl("create remote udp socket again.");
		end
    end
    self.forwarding_thread = CoThread.new();
    self.forwarding_thread:Start(_client_forwarding_thread);
end

function LocalUdpConnection:ClientForwardingThread(thread) 
	local local_port = self.bind_port;
	local udp_socket = UdpSocket.new();
    udp_socket:Create();

	if local_port > 0 then
		if not udp_socket:Bind(local_port) then
			printfnl("bind fail on port %d",local_port);
			self:Close();
			return
		end
	end

	self.socket = udp_socket;
	
	local ret = self.host_peer:BindRemote_Async(thread, self.remote_port);
	if not ret.value then 
		printfnl("bind remote port %d fail: timeout.",self.remote_port);
		udp_socket:Destroy();
		return
	end

	if ret.value.handle <= 0 then
		printfnl("bind remote port %d fail: %s.",self.remote_port,ret.value.errStr);
		udp_socket:Destroy();
		return
	end

	self.handle = ret.value.handle;

	local mem = new_mem();
	while self.socket do
		for i=1,1000, 1 do
			if self.socket:RecvMsg(mem) then
				local ip,port = self.socket:GetSrcAddr(true);
				self.host_peer.from_ip = ip;
				self.host_peer.from_port = port;

				local ret = self.host_peer:SendData_Async(
					thread,self.handle,
					self.remote_dest_ip,
					self.remote_dest_port,
					mem
				);

				if not ret.value then 
					printfnl("send remote data fail:timeout.");
					self:Close();
					break;
				end

				if ret.value.ws < 0 then
					printfnl("send remote data fail:ws=%d.",ret.value.ws);
					self:Close();
					break;
				end
			else
				break
			end
		end
		thread:Sleep(1);
	end  
	self:Close();	
end

