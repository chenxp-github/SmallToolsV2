require("common")
require("cothread")

LocalConnection = class();

function LocalConnection:ctor(host_peer,socket,handle)
    self.socket = socket;
    self.host_peer = host_peer;
    self.is_running = true;
    self.read_thread = nil;
    self.write_thread = nil;
    self.handle = handle;
end
