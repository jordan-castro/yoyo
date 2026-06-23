cmake_minimum_required(VERSION 3.22)
project(YoYoApp CXX C)

# yoyo uses c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    Corrosion
    GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
    GIT_TAG v0.6
)
FetchContent_MakeAvailable(Corrosion)
corrosion_import_crate(MANIFEST_PATH extensions/yoyo_net/Cargo.toml)

file(GLOB CORE_SOURCES "core/*.cpp")

# the YoYo cli is just a wrapper around `core`.
add_executable(yoyo
    "main.cpp"
    ${CORE_SOURCES}
)

target_include_directories(yoyo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/core/include"
)
target_include_directories(yoyo SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/pixelscript"
    "${CMAKE_CURRENT_SOURCE_DIR}/extensions/yoyo_net"
)

target_compile_definitions(yoyo PRIVATE
    $<$<CONFIG:Debug>:YOYO_DEBUG>
)

target_compile_definitions(yoyo PRIVATE
    YOYO_APP   # For yoyo app only. 
    YOYO_CORE  # Optionally include the `yoyo` main module.
    YOYO_OS    # Optionally include the `yoyo.os` module.
    YOYO_PXS   
    YOYO_FS    
    YOYO_SHELL
    YOYO_NET
)

target_link_directories(yoyo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/pixelscript/pxsb"
)

target_link_libraries(yoyo PRIVATE
    lua
    pocketpy
    quickjs
    pixelscript
)

target_link_libraries(yoyo PUBLIC yoyo_net)

if(MSVC)
    target_link_libraries(yoyo PRIVATE
        ws2_32
        ntdll
        userenv
    )

endif()