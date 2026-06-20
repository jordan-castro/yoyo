cmake_minimum_required(VERSION 3.15)
project(YoYoApp CXX C)

# yoyo uses c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Yoyo has few cpp files. Its mostly headers and static libraries.
add_executable(yoyo
    "main.cpp"
    "core/yoyo.cpp"
)

target_include_directories(yoyo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
)
target_include_directories(yoyo SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/pixelscript"
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

if(MSVC)
    target_link_libraries(yoyo PRIVATE
        ws2_32
        ntdll
        userenv
    )

endif()