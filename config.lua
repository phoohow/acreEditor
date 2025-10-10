-- helper function
function find_path(filePath, file)
    if not os.exists(filePath) then
        cprint("Not Found ${bright red}" .. file .. "${white}, Please check!")
    end
end

-- helper function
function create_dir(filePath)
    if not os.exists(filePath) then
        os.mkdir(filePath)
    end
end

import("core.project.config")

function get_include_dir()
    return os.scriptdir() .. "/include"
end

-- setup config.h
function setup_config(target, use_vulkan)
    local config_h_in_path = get_include_dir() .. "/config.h.in"
    find_path(config_h_in_path, "config.h.in")
    
    local file = io.open(config_h_in_path, "r")
    local content = file:read("*all")
    file:close()
    
    local new_content = content
    
    if use_vulkan then
        new_content = string.gsub(new_content, "$USE_VULKAN", "1")
    else
        new_content = string.gsub(new_content, "$USE_VULKAN", "0")
    end
    
    local src_dir = string.gsub(os.scriptdir() .. "/ext/acre", "\\", "/")
    new_content = string.gsub(new_content, "$SRC_DIR", src_dir)
    
    local dst_dir = string.gsub(target:targetdir(), "\\", "/")
    new_content = string.gsub(new_content, "$DST_DIR", dst_dir)
    
    -- cache: build problem
    local config_h_path = get_include_dir() .. "/config.h"
    if os.exists(config_h_path) then
        local oldfile = io.open(config_h_path, "r")
        local old_context = oldfile:read("*all")
        oldfile:close()
        if new_content == old_context then
            return
        end
    end
    
    print("Refresh editor config.h...")
    
    local outfile = io.open(config_h_path, "w")
    outfile:write(new_content)
    outfile:close()
end
