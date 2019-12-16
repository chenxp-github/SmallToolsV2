require("common")

PrintBuffer = class();

function PrintBuffer:ctor()
    self.mf_text = MemFile.new();
    self.m_log_level = 0;
    self.m_eol = EOL;
end

function PrintBuffer:Clear()
    self.mf_text:SetSize(0);
end

function PrintBuffer:IncLogLevel(l)
    self.m_log_level = self.m_log_level + l;
end

function PrintBuffer:Print(str,nl)
    self.mf_text:Puts(str);
    if nl then
        self.mf_text:Puts(self.m_eol);
    end
end

function PrintBuffer:GetText()
    if self.mf_text:GetSize() <= 0 then
        return "";
    end
    return file_to_string(self.mf_text);
end

function PrintBuffer:Printf(...)
    return self:Print(string.format(...));
end

function PrintBuffer:Log(...)
    self:Tab();
    return self:Print(string.format(...),true);
end

function PrintBuffer:Tab()
    for i=1,self.m_log_level,1 do
        self:Print("    ");
    end
end

function PrintBuffer:Eol()
    self:Print(self.m_eol);
end

function PrintBuffer:SaveToFile(fn)
    return self.mf_text:WriteToFile(fn);
end

function PrintBuffer:SetEol(eol)
    self.m_eol = eol;
end

function PrintBuffer:Dump()
    print_whole_file(self.mf_text);
end

function PrintBuffer:Puts(str)
    self.mf_text:Puts(str);
end

function PrintBuffer:GetInnerFile()
    return self.mf_text;
end