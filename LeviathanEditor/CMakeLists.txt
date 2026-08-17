file(
    GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/source/*.cpp"
)

include(FetchContent)
add_executable(LeviathanEditor ${SOURCES})

if (EDITOR_MODE)
    add_compile_definitions(IS_EDITOR=1)
endif()

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
)

SET(GLM_TEST_ENABLE OFF CACHE BOOL "GLM Build unit tests")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared libraries" FORCE)

FetchContent_MakeAvailable(glm)

target_link_libraries(LeviathanEditor
        PUBLIC glm::glm
        PUBLIC Leviathan
        PUBLIC ImGui
)

target_include_directories(LeviathanEditor
        PUBLIC "Source/include"
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(TARGET LeviathanEditor
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                                "${CMAKE_CURRENT_SOURCE_DIR}/Config"
                                "$<TARGET_FILE_DIR:LeviathanEditor>/Config"
        COMMENT "Copying runtime Config directory to build folder"
)

add_custom_command(TARGET LeviathanEditor
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                                "${CMAKE_CURRENT_SOURCE_DIR}/Content"
                                "$<TARGET_FILE_DIR:LeviathanEditor>/Content"
        COMMENT "Copying runtime Config directory to build folder"
)