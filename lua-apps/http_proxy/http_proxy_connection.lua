require("common")
require("cothread")

HttpProxyConnection = class();

function HttpProxyConnection:ctor(local_socket)
    self.local_socket = local_socket;
    self.remote_socket = nil;
    self.forward_thread = nil;
    self.backward_thread = nil;
    self.socket_fd = local_socket:GetSocketFd();
end

function HttpProxyConnection:Start()
    function _forward_thread(thread);
        self:ForwardThread(thread)
    end
    self.forward_thread = CoThread.new(1);
    self.forward_thread:Start(_forward_thread);
end

function HttpProxyConnection:ParseHttpHeader(qbuf)
    local line = new_mem();
    local ret = {};
    line:SetSplitChars(" \t:");

    local context = new_mem();
    qbuf:SaveContext(context);

    while qbuf:ReadLine(line) do
        if line:C(0) == 0 then
            break
        end
        line:Seek(0);

        local method = line:NextString();

        if method == "CONNECT" then
            line:SetSplitChars(" \t:");
            ret.method = "CONNECT";
            ret.host = line:NextString();
            ret.port = tonumber(line:NextString());
        elseif method == "GET" or method == "POST" then
            line:SetSplitChars(" /:\t");
            ret.method = method;
            line:NextString(); --skip http://
            ret.host = line:NextString();
            ret.port = 80;
            qbuf:RestoreContext(context);
            break;
        else
            print(line:CStr());
        end       
    end

    context:Destroy();
    if ret.host and ret.port then
        return ret;
    end
end

function HttpProxyConnection:ForwardThread(thread)
    local qbuf =  QueueFile.new(64*1024);
    local tmp = new_mem(8*1024);

    local remote;
    --first step is http conversation
    while self.local_socket:IsConnected() do
        tmp:SetSize(0);        
        if qbuf:GetFreeSize() >= tmp:GetMaxSize() then
            local rs = self.local_socket:Read(tmp,tmp:GetMaxSize());
            qbuf:Write(tmp,rs);
        end     

        if qbuf:HasEmptyLine() then
            remote = self:ParseHttpHeader(qbuf);
            break;
        end
        thread:Sleep(1);
    end

    self.remote_socket = nil;

    if remote then        
        local done = false;  
        TcpSocket.NewTcpConnector(remote.host,remote.port,
            function(event,new_socket)
                if event == EVENT_CONNECTED then                          
                    self.remote_socket = new_socket;
                end
                done = true;
            end
        );   

        while not done and self.local_socket:IsConnected() do
            thread:Sleep(100);
        end

        if remote.method == "CONNECT" then
            if self.remote_socket then
                printfnl("remote connection established %s:%d",remote.host,remote.port);
                self:WriteBlock(thread,self.local_socket,"HTTP/1.1 200 Connection Established\r\n\r\n");
            else
                printfnl("connect to %s:%d fail.",remote.host,remote.port);
                self:WriteBlock(thread,self.local_socket,"HTTP/1.1 500 Connect Fail.\r\n\r\n");
            end        
        end
    end
    
    if self.remote_socket then
        self:StartBackwordThread();
        self:CommonSocketForwarding(
            thread,
            self.local_socket,
            self.remote_socket,
            qbuf,
            tmp     
        );
    end

    self:Close();
    qbuf:Destroy();
    tmp:Destroy();
end

function HttpProxyConnection:StartBackwordThread()
    function _backward_thread(thread);
        self:BackwardThread(thread)
    end
    self.backward_thread = CoThread.new(1);
    self.backward_thread:Start(_backward_thread);
end

function HttpProxyConnection:BackwardThread(thread)
    local qbuf =  QueueFile.new(64*1024);
    local tmp = new_mem(8*1024);

    self:CommonSocketForwarding(
        thread,
        self.remote_socket,
        self.local_socket,
        qbuf,
        tmp
    );

    self:Close();
    qbuf:Destroy();
    tmp:Destroy();    
end

function HttpProxyConnection:CommonSocketForwarding(thread,from_socket,to_socket,qbuf,tmp)
    while from_socket:IsConnected() and to_socket:IsConnected() do        
        if qbuf:GetFreeSize() >= tmp:GetMaxSize() then
            tmp:SetSize(0);
            local rs = from_socket:Read(tmp,tmp:GetMaxSize());
            qbuf:Write(tmp,rs);
        end

        if qbuf:GetSize() > 0 then
            tmp:SetSize(0);
            local rs = qbuf:PeekData(tmp,tmp:GetMaxSize());
            assert(rs == tmp:GetSize(),"rs == tmp:GetSize()");    
            local ws = to_socket:Write(tmp,rs);
            if ws > 0 then qbuf:Skip(ws) end
        end
        thread:Sleep(1);
    end
end

function HttpProxyConnection:WriteBlock(thread,socket,data)
    if type(data) == "string" then
        local mem = Mem.new();
        mem:SetRawBuf(data);
        data = mem;
    end
    
    local offset = 0;
    while socket:IsConnected() and offset < data:GetSize() do
        local ws = socket:WriteFile(data,offset,data:GetSize());
        if ws > 0 then
            offset = offset + ws;
        end
        thread:Sleep(1);
    end
end

function HttpProxyConnection:IsConnected() 
    if not self.local_socket:IsConnected() then
        return false;
    end

    if self.remote_socket then
        if not self.remote_socket:IsConnected() then
            return false;
        end
    end
    return true;
end

function HttpProxyConnection:Close()
    self.local_socket:Destroy();
    if self.remote_socket then
        self.remote_socket:Destroy();
    end
end
