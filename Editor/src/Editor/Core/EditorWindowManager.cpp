
#include "EditorWindowManager.h"
#include "Editor/Core/EditorRenderer.h"
#include "Editor/Windows/MenuBar.h"
#include "Editor/Core/PopupWindowManager.h"
#include "Editor/Windows/ContentBrowser/ContentBrowserWindow.h"
#include "Editor/Windows/Inspector/InspectorWindow.h"
#include "Editor/Windows/SceneHierarchyWindow.h"
#include "Editor/Windows/StatisticsWindow.h"
#include "Editor/Windows/MaterialEditorWindow.h"
#include "Editor/Windows/Settings/SettingsWindow.h"
#include "Editor/Windows/Viewport/ViewportWindow.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Windows/PopUp/NewProjectPopupWindow.h"
#include "Engine/Utility/File.h"
#include "Engine/Core/Log.h"

//TODO: move to style file or something
static void SetupImGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	style.WindowMenuButtonPosition = ImGuiDir_None;

	// Primary background
	colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);  // #131318
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f); // #131318

	colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);

	// Headers
	colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);

	// Buttons
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.32f, 0.40f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.38f, 0.50f, 1.00f);

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.50f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.38f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

	// Borders
	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.25f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	// Text
	colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);

	// Highlights
	colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.70f, 1.00f, 0.50f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.80f, 1.00f, 0.75f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.90f, 1.00f, 1.00f);

	// Scrollbar
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.55f, 1.00f);

	// Style tweaks
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 0.0f;
	style.FrameRounding = 5.0f;
	style.GrabRounding = 5.0f;
	style.TabRounding = 5.0f;
	style.PopupRounding = 5.0f;
	style.ScrollbarRounding = 5.0f;
	style.WindowPadding = ImVec2(10, 10);
	style.FramePadding = ImVec2(6, 4);
	style.ItemSpacing = ImVec2(8, 6);
	style.PopupBorderSize = 0.f;
}

namespace Editor
{
	namespace
	{
		MenuBar m_MenuBar;
		NewProjectPopupWindow m_NewProjectPopup;

		std::tuple<
			SceneHierarchyWindow,
			ViewportWindow,
			ContentBrowserWindow,
			SettingsWindow,
			InspectorWindow,
			MaterialEditorWindow,
			StatisticsWindow
		> m_Windows;
	}

	void EditorWindowManager::Initialize(const Engine::Window& window)
	{
		PopupManager::Register(&m_NewProjectPopup); //TODO: i dont like this design...

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		constexpr const char* imgui_ini = "Resources/Editor/layouts/imgui.ini";
		if (!std::filesystem::exists(imgui_ini))
		{
			ResetLayout();
			ImGui::SaveIniSettingsToDisk(imgui_ini);
		}

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = imgui_ini;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Engine/fonts/roboto/Roboto-Regular.ttf", 18.0f);
		SetupImGuiStyle();

		EditorRenderer::Initialize(window);

		std::apply([](auto&&... win) { (win.Initialize(), ...); }, m_Windows);
	}

	void EditorWindowManager::Shutdown()
	{
		EditorRenderer::Shutdown();
		ImGui::DestroyContext();
	}

	void EditorWindowManager::Update()
	{
		EditorRenderer::BeginFrame();
		BeginDockSpace();

		m_MenuBar.Draw();
		PopupManager::Draw();
		std::apply([](auto&&... win) {(win.Draw(), ...); }, m_Windows);

		//bool showDemoWindow = true;
		//ImGui::ShowDemoWindow(&showDemoWindow);

		ImGui::End(); // End DockSpace
		EditorRenderer::EndFrame();
	}

	void EditorWindowManager::ResetLayout()
	{
		const std::filesystem::path default_iniPath = "Resources/Editor/layouts/default.ini";
		std::string defaultLayoutContent;

		if (!Engine::ReadFile(default_iniPath, defaultLayoutContent))
		{
			LOG_ERROR("Failed to read default ImGui layout from {}", default_iniPath);
			return;
		}

		ImGui::LoadIniSettingsFromMemory(defaultLayoutContent.c_str());
		LOG_INFO("ImGui layout reset to default");
	}

	void EditorWindowManager::SaveLayout()
	{
		const std::filesystem::path iniPath = "Resources/Editor/layouts/imgui.ini";
		ImGui::SaveIniSettingsToDisk(iniPath.string().c_str());
		LOG_INFO("ImGui layout saved to {}", iniPath);
	}

	void EditorWindowManager::BeginDockSpace()
	{
		ScopedStyle style
		{
			{ ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)}
		};

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		// main docking window
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}
}