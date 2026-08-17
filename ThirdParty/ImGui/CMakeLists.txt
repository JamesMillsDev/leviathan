set(IMGUI_SOURCES 
	"ImGui_Src/imconfig.h"
	"ImGui_Src/imgui.h"
	"ImGui_Src/imgui_internal.h"
	"ImGui_Src/imstb_rectpack.h"
	"ImGui_Src/imstb_textedit.h"
	"ImGui_Src/imstb_truetype.h"
	"ImGui_Src/backends/imgui_impl_glfw.h"
	"ImGui_Src/backends/imgui_impl_opengl3.h"
	"ImGui_Src/imgui.cpp"
	"ImGui_Src/imgui_demo.cpp"
	"ImGui_Src/imgui_draw.cpp"
	"ImGui_Src/imgui_tables.cpp"
	"ImGui_Src/imgui_widgets.cpp"
	"ImGui_Src/backends/imgui_impl_glfw.cpp"
	"ImGui_Src/backends/imgui_impl_opengl3.cpp"
)

include(FetchContent)
add_library(ImGui STATIC ${IMGUI_SOURCES})

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

target_link_libraries(ImGui
        PUBLIC glfw
)

target_include_directories(ImGui
		PUBLIC "ImGui_Src"
)