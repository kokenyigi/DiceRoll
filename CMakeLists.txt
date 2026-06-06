cmake_minimum_required(VERSION 3.16)
project(MyApp LANGUAGES CXX)

# ---- C++ standard ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Forrásfájlok ----
file(GLOB_RECURSE SOURCES
    src/*.cpp
	 thirdParty/stb_image/src/*.cpp
)

file(GLOB_RECURSE HEADERS
    include/*.h
    thirdParty/stb_image/include/*.h
)

add_executable(MyApp ${SOURCES})

# ---- Include könyvtárak ----
target_include_directories(MyApp PRIVATE
    include
    thirdParty/GLM/include
    thirdParty/GLFW/include
    thirdParty/GLEW/include
    thirdParty/stb_image/include
)



# ---- Preprocessor makrók ----
target_compile_definitions(MyApp PRIVATE
    GLEW_STATIC
    WIN32
)

# ---- Linkelési könyvtárak ----
target_link_directories(MyApp PRIVATE
    thirdParty/GLFW/lib-vc2022
    thirdParty/GLEW/lib/Release/Win32
)

# ---- Linkelt library-k ----
target_link_libraries(MyApp PRIVATE
    glfw3_mt.lib
    glew32s.lib
    opengl32
)


# ---- Resources ----
file(GLOB_RECURSE RES_FILES res/*)
target_sources(MyApp PRIVATE ${HEADERS} ${RES_FILES})

# ---- Visual Studio file grouping ----
source_group(TREE ${CMAKE_SOURCE_DIR}
    FILES ${SOURCES} ${HEADERS} ${RES_FILES}
)

# ---- EXE kimenet a projekt rootba ----
set_target_properties(MyApp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
)

# ---- MSVC runtime statikus linkelése ----
if (MSVC)
    set_property(TARGET MyApp PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded")
		
	set_target_properties(MyApp PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:MyApp>"
    )
	
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT MyApp)

	set(CMAKE_SUPPRESS_REGENERATION ON)

	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    if(TARGET ALL_BUILD)
        set_target_properties(ALL_BUILD PROPERTIES FOLDER "_CMake")
    endif()

    if(TARGET ZERO_CHECK)
        set_target_properties(ZERO_CHECK PROPERTIES FOLDER "_CMake")
    endif()
	
endif()