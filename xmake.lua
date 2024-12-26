set_languages("c++20")
add_rules("mode.debug", "mode.release")

add_includedirs("ext/stb/include")
add_includedirs("ext/tinygltf/include")
includes("ext/acre")

target("acreEditor")
    if get_config("buildir") ~= nil then
        set_targetdir(get_config("buildir") .. "/bin")
    end
    if get_config("buildir") then
        set_installdir(get_config("buildir") .. "/acreEditor")
    end
    --
    before_build(function(target)
        local use_vulkan = false 
        import("config")
        config.setup_config(use_vulkan)
    end)
    --
    add_rules("qt.widgetapp")
    add_deps("acre")
    add_includedirs("include")
    add_files("src/*.cpp", "src/*/*.cpp", "src/*/*/*.cpp")

    after_build(function (target) 
        if is_mode("release") then 
            os.cp(os.projectdir().."/ext/acre/src/shaders/*.hlsl", get_config("buildir") .. "/bin/shaders/")
            os.cp(os.projectdir().."/ext/acre/tools/dxc/bin/x64/dxc.exe", get_config("buildir") .. "/bin")
        end 
    end)

    after_install(function (target)
        os.cp(os.projectdir().."/ext/acre/tools/dxc/bin/x64/*.dll", get_config("buildir") .. "/acreEditor/bin")
        if is_mode("release") then 
            os.cp(os.projectdir().."/ext/acre/src/shaders/*.hlsl", get_config("buildir") .. "/acreEditor/bin/shaders/")
            os.cp(os.projectdir().."/ext/acre/tools/dxc/bin/x64/dxc.exe", get_config("buildir") .. "/acreEditor/bin")

            -- copy shaderMake not from build dir, copy directly
            os.cp(get_config("buildir") .. "/bin/ShaderMake.exe", get_config("buildir") .. "/acreEditor/bin")
        end 
    end)
target_end()
