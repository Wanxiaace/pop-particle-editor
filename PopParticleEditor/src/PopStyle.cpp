#include "../include/PopStyle.h"
#include "../include/Fonts/IconsFontAwesome7.h"
#include "../include/PopState.h"
#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
	ImVec4 Color(int red, int green, int blue, int alpha = 255)
	{
		constexpr float scale = 1.0f / 255.0f;
		return {
			red * scale,
			green * scale,
			blue * scale,
			alpha * scale
		};
	}

	std::filesystem::path ExecutableDirectory()
	{
#ifdef _WIN32
		wchar_t path[MAX_PATH]{};
		const DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);
		if (length > 0 && length < MAX_PATH)
			return std::filesystem::path(path).parent_path();
#endif
		return {};
	}

	std::filesystem::path FindFontFile(const char* fileName)
	{
		const std::filesystem::path currentDirectory = std::filesystem::current_path();
		const std::filesystem::path candidates[] = {
			ExecutableDirectory() / "fonts" / fileName,
			currentDirectory / "fonts" / fileName,
			currentDirectory.parent_path() / "fonts" / fileName
		};

		for (const std::filesystem::path& candidate : candidates)
		{
			std::error_code error;
			if (std::filesystem::is_regular_file(candidate, error))
				return candidate;
		}

		return {};
	}
}

namespace pop
{
	void InitializePopStyle(float dpiScale)
	{
		ApplyPopStyle(dpiScale);
		LoadPopFonts();
	}

	void ApplyPopStyle(float dpiScale)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		const float scale = dpiScale > 0.0f ? dpiScale : 1.0f;

		style.WindowPadding = { 12.0f * scale, 12.0f * scale };
		style.WindowRounding = 8.0f * scale;
		style.WindowBorderSize = 1.0f * scale;
		style.ChildRounding = 8.0f * scale;
		style.ChildBorderSize = 1.0f * scale;
		style.PopupRounding = 6.0f * scale;
		style.PopupBorderSize = 1.0f * scale;
		style.FramePadding = { 12.0f * scale, 7.0f * scale };
		style.FrameRounding = 6.0f * scale;
		style.FrameBorderSize = 1.0f * scale;
		style.ItemSpacing = { 10.0f * scale, 8.0f * scale };
		style.ItemInnerSpacing = { 8.0f * scale, 6.0f * scale };
		style.ScrollbarSize = 13.0f * scale;
		style.ScrollbarRounding = 6.0f * scale;
		style.GrabMinSize = 10.0f * scale;
		style.GrabRounding = 4.0f * scale;
		style.TabRounding = 6.0f * scale;
		style.TabBorderSize = 0.0f;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = Color(240, 242, 250);
		colors[ImGuiCol_TextDisabled] = Color(145, 156, 179);
		colors[ImGuiCol_WindowBg] = Color(18, 18, 24);
		colors[ImGuiCol_ChildBg] = Color(25, 27, 36);
		colors[ImGuiCol_PopupBg] = Color(25, 27, 36, 250);
		colors[ImGuiCol_Border] = Color(58, 63, 82);
		colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0);

		colors[ImGuiCol_FrameBg] = Color(34, 37, 48);
		colors[ImGuiCol_FrameBgHovered] = Color(44, 48, 62);
		colors[ImGuiCol_FrameBgActive] = Color(51, 56, 72);
		colors[ImGuiCol_TitleBg] = Color(20, 22, 29);
		colors[ImGuiCol_TitleBgActive] = Color(27, 30, 40);
		colors[ImGuiCol_TitleBgCollapsed] = Color(20, 22, 29);
		colors[ImGuiCol_MenuBarBg] = Color(21, 23, 30);

		colors[ImGuiCol_ScrollbarBg] = Color(18, 20, 27);
		colors[ImGuiCol_ScrollbarGrab] = Color(54, 59, 76);
		colors[ImGuiCol_ScrollbarGrabHovered] = Color(70, 76, 97);
		colors[ImGuiCol_ScrollbarGrabActive] = Color(85, 93, 118);

		colors[ImGuiCol_CheckMark] = Color(116, 164, 255);
		colors[ImGuiCol_CheckboxSelectedBg] = Color(70, 116, 210);
		colors[ImGuiCol_SliderGrab] = Color(99, 145, 235);
		colors[ImGuiCol_SliderGrabActive] = Color(124, 169, 255);

		colors[ImGuiCol_Button] = Color(38, 41, 53);
		colors[ImGuiCol_ButtonHovered] = Color(48, 52, 67);
		colors[ImGuiCol_ButtonActive] = Color(32, 35, 46);
		colors[ImGuiCol_Header] = Color(38, 41, 53);
		colors[ImGuiCol_HeaderHovered] = Color(48, 52, 67);
		colors[ImGuiCol_HeaderActive] = Color(57, 63, 80);

		colors[ImGuiCol_Separator] = Color(58, 63, 82);
		colors[ImGuiCol_SeparatorHovered] = Color(99, 145, 235);
		colors[ImGuiCol_SeparatorActive] = Color(116, 164, 255);
		colors[ImGuiCol_ResizeGrip] = Color(70, 116, 210, 70);
		colors[ImGuiCol_ResizeGripHovered] = Color(99, 145, 235, 170);
		colors[ImGuiCol_ResizeGripActive] = Color(116, 164, 255, 230);
		colors[ImGuiCol_InputTextCursor] = Color(196, 214, 255);

		colors[ImGuiCol_Tab] = Color(29, 32, 42);
		colors[ImGuiCol_TabHovered] = Color(48, 60, 84);
		colors[ImGuiCol_TabSelected] = Color(43, 53, 73);
		colors[ImGuiCol_TabSelectedOverline] = Color(116, 164, 255);
		colors[ImGuiCol_TabDimmed] = Color(24, 26, 34);
		colors[ImGuiCol_TabDimmedSelected] = Color(34, 39, 52);
		colors[ImGuiCol_TabDimmedSelectedOverline] = Color(78, 107, 162);

		colors[ImGuiCol_TableHeaderBg] = Color(31, 34, 45);
		colors[ImGuiCol_TableBorderStrong] = Color(58, 63, 82);
		colors[ImGuiCol_TableBorderLight] = Color(45, 49, 64);
		colors[ImGuiCol_TableRowBg] = Color(0, 0, 0, 0);
		colors[ImGuiCol_TableRowBgAlt] = Color(255, 255, 255, 8);
		colors[ImGuiCol_TextLink] = Color(124, 169, 255);
		colors[ImGuiCol_TextSelectedBg] = Color(70, 116, 210, 110);
		colors[ImGuiCol_TreeLines] = Color(68, 74, 95);
		colors[ImGuiCol_DragDropTarget] = Color(255, 184, 92);
		colors[ImGuiCol_DragDropTargetBg] = Color(255, 184, 92, 25);
		colors[ImGuiCol_UnsavedMarker] = Color(255, 184, 92);
		colors[ImGuiCol_NavCursor] = Color(124, 169, 255);
		colors[ImGuiCol_NavWindowingHighlight] = Color(240, 242, 250, 180);
		colors[ImGuiCol_NavWindowingDimBg] = Color(8, 9, 12, 150);
		colors[ImGuiCol_ModalWindowDimBg] = Color(8, 9, 12, 180);
	}

	bool LoadPopFonts()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();

		const std::filesystem::path heitiFontPath = FindFontFile("LXGWNeoXiHeiPlus.ttf");
		if (heitiFontPath.empty())
			return false;

		// Use LXGW as the base font so all regular text, including ASCII, uses its glyphs.
		ImFontConfig heitiFontConfig;
		heitiFontConfig.SizePixels = 14.0f;
		static ImVector<ImWchar> textGlyphRanges;
		ImFontGlyphRangesBuilder glyphBuilder;
		glyphBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());
		glyphBuilder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		for (const auto& [index, text] : gMultiLangDictionary)
		{
			glyphBuilder.AddText(index.c_str());
			glyphBuilder.AddText(text.c_str());
		}
		glyphBuilder.BuildRanges(&textGlyphRanges);
		io.FontDefault = io.Fonts->AddFontFromFileTTF(
			heitiFontPath.u8string().c_str(),
			14.0f,
			&heitiFontConfig,
			textGlyphRanges.Data);
		if (!io.FontDefault)
			return false;

		const std::filesystem::path iconFontPath = FindFontFile("fa-solid-900.ttf");
		if (iconFontPath.empty())
			return false;

		static const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig iconFontConfig;
		iconFontConfig.MergeMode = true;
		iconFontConfig.PixelSnapH = true;
		iconFontConfig.GlyphMinAdvanceX = 14.0f;

		if (!io.Fonts->AddFontFromFileTTF(
			iconFontPath.u8string().c_str(),
			14.0f,
			&iconFontConfig,
			iconRanges))
			return false;
		return true;
	}

	void PushPrimaryButtonStyle()
	{
		ImGui::PushStyleColor(ImGuiCol_Button, Color(70, 116, 210));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Color(82, 132, 232));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, Color(58, 101, 190));
		ImGui::PushStyleColor(ImGuiCol_Border, Color(105, 151, 244));
	}

	void PopPrimaryButtonStyle()
	{
		ImGui::PopStyleColor(4);
	}
}
