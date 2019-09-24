require("class_base");

local THREAD_IS_RUNNING   = 0x00000001;
local THREAD_IS_DELETED   = 0x00000002;
local THREAD_IS_SLEEPING   = 0x00000004;

CoThread=class();

function CoThread:ctor(interval)
    self.m_thread = nil;
    self.m_flags = 0;
    self.m_schedule_timer = nil;
    if not interval then
        interval = 50;
    end
    self.m_schedule_interval = interval;
    self.m_sleep_time = 0;
    self.m_total_sleep_time = 0;
end

function CoThread:IsDead()
    return (self.m_flags&THREAD_IS_DELETED) ~= 0;
end

function CoThread:IsSuspend()
    return (self.m_flags&THREAD_IS_RUNNING) == 0;
end

function CoThread:IsRunning()
    return not self:IsSuspend();
end

function CoThread:IsSleeping(interval)
    if self.m_flags&THREAD_IS_SLEEPING ~= 0 then
        self.m_sleep_time = self.m_sleep_time + interval;
        if self.m_sleep_time >= self.m_total_sleep_time then
            self.m_flags = self.m_flags & (~THREAD_IS_SLEEPING);
            self.m_sleep_time = 0;
            self.m_total_sleep_time = 0;
        end
        return true;
    end
    return false;
end

function CoThread:Sleep(ms)
    if ms > 0 then
        self.m_flags = self.m_flags | THREAD_IS_SLEEPING;
        self.m_sleep_time = 0;
        self.m_total_sleep_time = ms;
        coroutine.yield(self.m_thread);
    end
end

function CoThread:Suspend(inside_thread)
    if self:IsDead() then return end
    if self:IsSuspend() then return end
    self.m_flags = self.m_flags & (~THREAD_IS_RUNNING);
    self.m_schedule_timer:Stop();
    self.m_schedule_timer = nil;
    if inside_thread then
        coroutine.yield(self.m_thread);
    end
end

function CoThread:Resume()
    if self:IsDead() then return end
    if not self:IsSuspend() then return end
    self.m_flags = self.m_flags | THREAD_IS_RUNNING;
    self.m_schedule_timer = Timer.new();
    self.m_schedule_timer:SetTimeout(self.m_schedule_interval);
    self.m_schedule_timer:SetCallback(function(interval)
        self:ScheduleOnce(interval);
    end);
    self.m_schedule_timer:Start();
end

function CoThread:Quit()
    self.m_flags = self.m_flags | THREAD_IS_DELETED;
end

function CoThread:ScheduleOnce(interval)
    if self:IsDead() then
        self.m_thread = nil;
        return self.m_schedule_timer:Stop();
    end

    if not interval then
        interval = self.m_schedule_interval;
    end

    if self:IsSleeping(interval) then
        return;
    end

    if self:IsSuspend() then
        return;
    end

    local ok,err = coroutine.resume(self.m_thread);
    if not ok then
        print("CoThread dead with error:"..err);
        self:Quit();
    end
end

function CoThread:Start(func,...)
    if self.m_thread then return end
    local params = {...};
    self.m_thread = coroutine.create(function()
        if func then
            func(self,table.unpack(params));
        else
            self:Run();
        end
        self:Quit();
    end);
    self:Resume();
end
