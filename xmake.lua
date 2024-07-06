add_rules("mode.debug", "mode.release")
if is_mode("release") then
    set_optimize("fastest")
end
set_languages("c++20")
add_cxxflags("-Wall", "-Wextra")
add_ldflags("-pthread")
set_allowedplats("windows", "linux", "macosx")
set_toolchains("clang")

-- package manager
add_includedirs("./")
add_requires("yaml-cpp")
add_requires("nlohmann_json")
add_requires("asio")
add_requires("spdlog")
add_packages("yaml-cpp", "nlohmann_json", "asio", "spdlog")

-- build target
target("server")
    set_kind("binary")
    add_files("cmd/main.cpp")
    
target("client")
    set_kind("binary")
    add_files("cmd/client.cpp")

local test_files = os.files("test/*.cpp")
for _, file in ipairs(test_files) do
    add_defines("RESOURCES_DIR=\"$(projectdir)/test\"")
    local target_name = path.basename(file)
    target(target_name)
        set_kind("binary")
        add_files(file)
end

-- format script
task("format")
    set_category("plugin")
    on_run(function ()
        -- format all .h and .cpp files in the project except those in the kernel folder
        local files = os.files("**.h|kernel/**.h")
        table.join2(files, os.files("**.cpp|kernel/**.cpp"))
        local total_files = #files
        for i, file in ipairs(files) do
            os.exec("clang-format -i " .. file)
            local progress = math.floor((i / total_files) * 100)
            io.write(string.format("[%3d%%] Formatting %s \n",progress,file))
            io.flush()
        end
        -- success
        cprint("Formatting completed!")
    end)
    set_menu {
        usage = "xmake format",
        description = "Format all .h and .cpp files in the project except those in the kernel folder."
    }
