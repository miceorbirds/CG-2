cmake_minimum_required(VERSION 3.17) #required version of cmake
project(CG) # the project's name
set(CMAKE_CXX_STANDARD 17) # specify the C++ standard

set(SOURCE_FILES # set an expression for sourse files
        main.cpp
        error_logger.cpp error_logger.h
        string_converter.cpp string_converter.h
        render_window.cpp render_window.h
        engine.cpp engine.h
        window_container.cpp window_container.h
        Keyboard/keyboard_event.cpp Keyboard/keyboard_event.h
        Keyboard/keyboard_class.cpp Keyboard/keyboard_class.h
        Mouse/mouse_class.cpp Mouse/mouse_class.h
        Mouse/mouse_event.cpp Mouse/mouse_event.h
        Graphics/graphics.cpp Graphics/graphics.h
        Graphics/adapter_reader.cpp Graphics/adapter_reader.h
        )

include_directories(include/DirectXMath/Inc)

add_executable(CG ${SOURCE_FILES})

add_subdirectory(include/DirectXMath)
#include_directories(${PROJECT_SOURCE_DIR}/include)

set(LIBS # set an expression for library files
        d3d11
        dxgi
        DirectXMath
        )
# this command are used for wWinMain
# (look at https://ru.stackoverflow.com/a/588590)
target_link_options(CG PRIVATE -municode)


target_link_libraries(CG ${LIBS})