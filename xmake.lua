set_languages("c++20")
add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    set_runtimes("MDd")
else
    set_runtimes("MD")
end

includes("ext/acre")
includes("ext/codec")

-- Global config:
-- Set default target
-- For each target, set targetdir and installdir
on_load(function(target)
    local buildir = get_config("buildir")
    if is_mode("debug") then
        target:set("targetdir", buildir .. "/debug")
    elseif is_mode("release") then
        target:set("targetdir", buildir .. "/release")
    end
    target:set("installdir", buildir .. "/acreEditor")
    
    if target:name() == "acreEditor" then
        target:set("default", true)
    else
        target:set("default", false)
    end
end)

target("acreEditor", function()
    add_rules("qt.widgetapp")
    add_deps("acre")
    add_deps("codec")
    add_includedirs("include")
    add_includedirs("ext/stb/include")
    add_includedirs("ext/tinygltf/include")
    add_includedirs("ext/codec/include")
    add_files("src/**.cpp")
    
    -- add_defines("USE_VULKAN")

    after_install(function(target)
        os.cp(os.scriptdir() .. "/tools/dxc/bin/x64/*", target:targetdir())
        if is_mode("release") then
            os.cp(os.scriptdir() .. "/src/shaders/*.hlsl", target:targetdir() .. "/shaders/")
            
            -- copy executables to installdir
            os.cp(target:targetdir() .. "/*.exe", target:installdir())
        end
    end)
end)
