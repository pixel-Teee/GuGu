add_rules("mode.debug","mode.debug")
target("GuGu")
    add_includedirs(".")

    add_files("*.cpp")
    add_headerfiles("*.h")

    -- Window
    add_files("Window/*.cpp")
    add_headerfiles("Window/*.h")

    -- Application
    add_files("Application/*.cpp")
    add_headerfiles("Application/*.h")

    set_pcxxheader("pch.h")

    -- Platform
    if is_plat("windows") then
        set_kind("binary")
        add_files("Window/Platform/Windows/*.cpp")
        add_headerfiles("Window/Platform/Windows/*.h")

        add_files("Application/Platform/Windows/*.cpp")
        add_headerfiles("Application/Platform/Windows/*.h")

        add_defines("WIN32", "UNICODE", "_UNICODE")
        add_cxflags("/execution-charset:utf-8")
        add_syslinks("User32")
        add_ldflags("/subsystem:windows")
    end

    if is_plat("android") then

        set_kind("shared")
        add_files("Application/Platform/Android/*.cpp")
        add_headerfiles("Application/Platform/Android/*.h")

        add_files("Application/Platform/Android/*.cpp")
        add_headerfiles("Application/Platform/Android/*.h")

        add_defines("ANDROID")
        --add_packages("game-activity")
        --on_load(function (target)
        --    target:add(find_packages("game-activity"))
        --end)
        --find_packages("game-activity")
        add_links("game-activity")



    end