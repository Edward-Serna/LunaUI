//
// Created by Edward on 8/18/2025.
//

#include "app.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <SDL3/SDL_opengl.h>
#include <iostream>

app::app() {
	if (!start_renderer()) {
		std::cout << "Failed to start renderer" << std::endl;
	}
	_imgui_window_manager = std::make_unique<window_manager>();
	InitializeWindows();
}

void app::InitializeWindows() const {
	_imgui_window_manager->AddWindow<settings_window>("Settings");
}

bool app::start_renderer()
{
	// Setup SDL
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
		printf("Error: %s\n", SDL_GetError());
		SDL_Quit();
		return false;
	}

#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// Decide GL+GLSL versions
	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	// Create a window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_WindowFlags sdl_window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_INPUT_FOCUS;
	_window = SDL_CreateWindow("Summer 2025", 600, 900, sdl_window_flags);
	if (_window == nullptr) {
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		SDL_Quit();
		return false;
	}

	// SDL_Surface* icon = IMG_Load("resources/images/my_icon.png");
	// if (icon == nullptr)
	// {
	// 	printf("Error: SDL_LoadBMP(): %s\n", SDL_GetError());
	// }
	// else {
	// 	SDL_SetWindowIcon(_window, icon);
	// 	SDL_DestroySurface(icon);
	// }

    if (!SDL_MaximizeWindow(_window)) {
        printf("Can NOT Maximize: %s\n", SDL_GetError());
    }

    _gl_context = SDL_GL_CreateContext(_window);
    if (_gl_context == nullptr) {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        SDL_DestroyWindow(_window);
        SDL_Quit();
    	return false;
    }

    SDL_GL_MakeCurrent(_window, _gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	SetupImGuiStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(_window, _gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    ImFont* font = io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 15.0f);
    if (font == nullptr) {
        io.Fonts->AddFontDefault();
    }
    io.Fonts->Build();

	std::cout << "Renderer Started!" << std::endl;
	return true;
}

void app::render_loop() const {
    ImGuiIO &io = ImGui::GetIO(); // Main configuration and I/O between your application and ImGui (also see: ImGuiPlatformIO)
    ImVec4 bgColor = ImVec4(0.15f, 0.15f,  0.15f, 1.00f);// Background Color

	bool show_demo_window = true;
	bool menu_visibility_toggled = true;
	bool done = false;

    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(_window))
                done = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

    	io.FontGlobalScale = std::max(1.0f, io.DisplaySize.y / 1080.0f);

    	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        if (ImGui::IsKeyPressed(ImGuiKey_F10)) { menu_visibility_toggled = !menu_visibility_toggled; }

    	if (menu_visibility_toggled) {
    		if (ImGui::BeginMainMenuBar()) {
    			window_base* settings_window = _imgui_window_manager->GetWindow("Settings");
    			bool visible = settings_window->IsVisible();
    			if (ImGui::MenuItem(settings_window->GetTitle().c_str(), nullptr, &visible)) {
    				(visible) ? settings_window -> Show() : settings_window->Hide();
    			}
    		}
    		ImGui::EndMainMenuBar();
    	}

    	_imgui_window_manager->RenderAll();

    	ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Render();

        glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
        glClearColor(bgColor.x * bgColor.w, bgColor.y * bgColor.w, bgColor.z * bgColor.w, bgColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        SDL_GL_SwapWindow(_window);
    }
}

app::~app() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if (_gl_context) {
        SDL_GL_DestroyContext(_gl_context);
    }
    if (_window) {
        SDL_DestroyWindow(_window);
    }

    SDL_Quit();
}


void app::SetupImGuiStyle() {
	// Fork of Comfy style from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.4000000059604645f;
	style.WindowPadding = ImVec2(11.5f, 6.099999904632568f);
	style.WindowRounding = 5.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(20.0f, 30.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding = 5.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 5.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(5.0f, 3.5f);
	style.FrameRounding = 5.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(5.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
	style.CellPadding = ImVec2(8.600000381469727f, 4.199999809265137f);
	style.IndentSpacing = 5.0f;
	style.ColumnsMinSpacing = 5.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 5.0f;
	style.TabRounding = 5.0f;
	style.TabBorderSize = 0.0f;
	// style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 0.3776823878288269f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.2231759428977966f, 0.2231737077236176f, 0.2231737077236176f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.540772557258606f, 0.6393193006515503f, 1.0f, 0.24935622215271f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2618025541305542f, 0.2573080956935883f, 0.2573080956935883f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3803921639919281f, 0.4235294163227081f, 0.572549045085907f, 0.5490196347236633f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.5371254086494446f, 0.5573320388793945f, 0.7725322246551514f, 0.6137338876724243f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2014293521642685f, 0.2012746781110764f, 0.2103004455566406f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.333216667175293f, 0.3405999541282654f, 0.5751073360443115f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1700897067785263f, 0.1722071170806885f, 0.2317596673965454f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1673820018768311f, 0.1673803329467773f, 0.1673803329467773f, 0.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.454935610294342f, 0.4549310505390167f, 0.4549310505390167f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.553648054599762f, 0.5536425113677979f, 0.5536425113677979f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3433476686477661f, 0.3433442413806915f, 0.3433442413806915f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7639485001564026f, 0.7639408707618713f, 0.7639408707618713f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4278951287269592f, 0.4881949424743652f, 0.8669527769088745f, 0.9490196347236633f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.4738344848155975f, 0.4778077006340027f, 0.9356223344802856f, 0.5490196347236633f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.4434599578380585f, 0.5152584314346313f, 0.9656652212142944f, 0.54935622215271f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.6333327293395996f, 0.7070270776748657f, 0.9399141669273376f, 0.5490196347236633f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4667243957519531f, 0.5025524497032166f, 0.8841201663017273f, 0.5490196347236633f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.4751972258090973f, 0.4712556898593903f, 0.7038626670837402f, 0.5490196347236633f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.39532870054245f, 0.4216172695159912f, 0.6309012770652771f, 0.5490196347236633f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.7956408262252808f, 0.772549033164978f, 0.9647058844566345f, 0.5490196347236633f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.6196078658103943f, 0.5764706134796143f, 0.7686274647712708f, 0.5490196347236633f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.464182436466217f, 0.5383070111274719f, 0.7725322246551514f, 0.5793991088867188f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.785744309425354f, 0.772549033164978f, 0.9647058844566345f, 0.5490196347236633f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.4372253715991974f, 0.3771481812000275f, 0.5021458864212036f, 0.1888412237167358f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.6941176652908325f, 0.710611879825592f, 0.886274516582489f, 0.5490196347236633f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.7758477926254272f, 0.772549033164978f, 0.9647058844566345f, 0.5490196347236633f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.5836910009384155f, 0.597984790802002f, 1.0f, 0.5490196347236633f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.5626370310783386f, 0.598031759262085f, 0.8798283338546753f, 0.5490196347236633f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.3648068904876709f, 0.4195594787597656f, 1.0f, 0.5490196347236633f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.4509803950786591f, 1.0f, 0.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 0.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.294117659330368f, 0.294117659330368f, 0.294117659330368f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.5688997507095337f, 0.5718759894371033f, 0.9141631126403809f, 0.6480686664581299f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.5476953387260437f, 0.5430934429168701f, 0.8111587762832642f, 0.7982832789421082f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.5843746662139893f, 0.5644974112510681f, 0.8540772199630737f, 0.6952790021896362f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.2021035403013229f, 0.2021050453186035f, 0.2231759428977966f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.55922931432724f, 0.579698920249939f, 0.7081544995307922f, 0.5490196347236633f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.3803921639919281f, 0.3968863487243652f, 0.572549045085907f, 0.5708154439926147f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03433477878570557f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.6941176652908325f, 0.7056636214256287f, 0.886274516582489f, 0.5490196347236633f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(9.999999974752427e-07f, 9.999899930335232e-07f, 9.999899930335232e-07f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.4291845560073853f, 0.4291802644729614f, 0.4291802644729614f, 1.0f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.03004294633865356f, 0.03004264459013939f, 0.03004264459013939f, 0.7424892783164978f);

	style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.4383944869041443f, 0.4559024572372437f, 0.729613721370697f, 1.0f);
}

