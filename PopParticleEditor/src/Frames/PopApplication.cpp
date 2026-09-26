#include "../../include/Frames/PopApplication.h"
#include "../../include/Fonts/IconsFontAwesome7.h"
#include "../../include/PopState.h"
#include <imgui.h>
#include <nfd.h>
#include <imgui_internal.h>

namespace
{
	std::string GetDialogError()
	{
		const char* error = NFD_GetError();
		return error != nullptr ? error : pop::ML("dialog.native_failed");
	}
}

namespace pop
{
	PopApplication::PopApplication() :_state(State::STATE_HOME)
	{
		_home.GetOnCreateNewEvent().Subscribe([this]() {_create_new();});
		_home.GetOnOpenProjectEvent().Subscribe([this]() {_open_project();});
	}

	PopApplication::~PopApplication() {};

	void PopApplication::OnRender()
	{
		bool saveRequested = false;
		bool saveAsRequested = false;
		bool publishRequested = false;

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu(ML("menu.files")))
		{
			if(ImGui::MenuItem(ML("menu.open_project")))
				_open_project();

			ImGui::BeginDisabled(_state != State::STATE_BENCH);
			if (ImGui::MenuItem(ML("menu.save"), ML("shortcut.save")))
				saveRequested = true;

			if (ImGui::MenuItem(ML("menu.save_as")))
				saveAsRequested = true;
			ImGui::Text("");

			if (ImGui::MenuItem(ML("menu.publish")))
				publishRequested = true;
			ImGui::EndDisabled();

			ImGui::EndMenu();
		}

		ImGui::BeginDisabled(_state != State::STATE_BENCH);

		if (ImGui::BeginMenu(ML("menu.resources")))
		{

			if (ImGui::MenuItem(ML("menu.load")))
			{
				nfdu8filteritem_t filterList[] = {
					{ ML("filter.images"), "png,jpg,jpeg" },
				};
				nfdfiltersize_t filterCount = 1;

				const nfdpathset_t* outPaths = NULL;

				nfdresult_t result = NFD_OpenDialogMultiple(
					&outPaths,
					filterList,
					filterCount,
					nullptr
				);

				if (result == NFD_OKAY)
				{
					nfdpathsetsize_t count;
					NFD_PathSet_GetCount(outPaths, &count);

					for (nfdpathsetsize_t i = 0; i < count; ++i) {
						nfdu8char_t* path = nullptr;
						NFD_PathSet_GetPath(outPaths, i, &path);

						_bench.LoadResource(std::filesystem::u8path(path));
					}

					NFD_PathSet_Free(outPaths);
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndDisabled();

		if (ImGui::BeginMenu(ML("menu.settings")))
		{
			if (ImGui::BeginMenu(ML("menu.languages")))
			{
				if (ImGui::MenuItem("简体中文"))
				{
					gUserConfig.language = PopLanguage::Chinese;
					SaveUserConfig();

					_show_notification(MLF("notification.switch_lang", "简体中文"), false);
				}

				if (ImGui::MenuItem("English"))
				{
					gUserConfig.language = PopLanguage::English;
					SaveUserConfig();

					_show_notification(MLF("notification.switch_lang", "English"), false);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::Text(ML("status.fps"), ImGui::GetIO().Framerate);

		if (_state == State::STATE_BENCH &&
			ImGui::Shortcut(
				ImGuiMod_Ctrl | ImGuiKey_S,
				ImGuiInputFlags_RouteGlobal))
		{
			saveRequested = true;
		}

		ImGui::EndMainMenuBar();

		switch (_state)
		{
		case State::STATE_HOME:
			_home.OnRender();
			break;
		case State::STATE_BENCH:
			_bench.OnRender();
			break;
		default:
			break;
		}

		if (saveAsRequested)
			_save_project(true);
		else if (saveRequested)
			_save_project(false);
		if (publishRequested)
			_publish_project();

		_render_notification();
	}

	void PopApplication::_create_new()
	{
		_bench.CreateNewProject();
		_state = State::STATE_BENCH;
	}

	void PopApplication::_open_project()
	{
		const nfdu8filteritem_t filterList[] = {
			{ ML("filter.project"), "json" },
			{ ML("filter.particle_xml"), "xml" }
		};
		nfdu8char_t* selectedPath = nullptr;
		const nfdresult_t result = NFD_OpenDialogU8(
			&selectedPath,
			filterList,
			2,
			nullptr);

		if (result == NFD_CANCEL)
			return;
		if (result == NFD_ERROR)
		{
			_show_notification(MLF(
				"notification.open_failed",
				GetDialogError().c_str()), true);
			return;
		}

		const std::filesystem::path path(std::filesystem::u8path(selectedPath));
		NFD_FreePathU8(selectedPath);

		std::string error;
		if (!_bench.OpenProject(path, error))
		{
			_show_notification(MLF("notification.open_failed", error.c_str()), true);
			return;
		}

		_state = State::STATE_BENCH;
		const std::string filename = path.filename().u8string();
		_show_notification(MLF("notification.opened", filename.c_str()), false);
	}

	void PopApplication::_save_project(bool saveAs)
	{
		std::filesystem::path path;
		if (!saveAs && _bench.HasProjectPath())
		{
			path = _bench.GetProjectPath();
		}
		else
		{
			const nfdu8filteritem_t filterList[] = {
				{ ML("filter.project"), "json" }
			};
			const std::string defaultPath = _bench.HasProjectPath()
				? _bench.GetProjectPath().parent_path().u8string()
				: std::string{};
			const std::string defaultName = _bench.HasProjectPath()
				? _bench.GetProjectPath().filename().u8string()
				: ML("file.default_project_name");

			nfdu8char_t* selectedPath = nullptr;
			const nfdresult_t result = NFD_SaveDialogU8(
				&selectedPath,
				filterList,
				1,
				defaultPath.empty() ? nullptr : defaultPath.c_str(),
				defaultName.c_str());

			if (result == NFD_CANCEL)
				return;
			if (result == NFD_ERROR)
			{
				_show_notification(MLF(
					"notification.save_failed",
					GetDialogError().c_str()), true);
				return;
			}

			path = selectedPath;
			NFD_FreePathU8(selectedPath);
			if (!path.has_extension())
				path += ".json";
		}

		std::string error;
		if (!_bench.SaveProject(path, error))
		{
			_show_notification(MLF("notification.save_failed", error.c_str()), true);
			return;
		}

		const std::string filename = path.filename().u8string();
		_show_notification(MLF("notification.saved", filename.c_str()), false);
	}

	void PopApplication::_publish_project()
	{
		const nfdu8filteritem_t filterList[] = {
			{ ML("filter.particle_xml"), "xml" }
		};
		const std::string defaultPath = _bench.HasProjectPath()
			? _bench.GetProjectPath().parent_path().u8string()
			: std::string{};
		const std::string defaultName = _bench.HasProjectPath()
			? _bench.GetProjectPath().stem().u8string() + ".xml"
			: ML("file.default_xml_name");

		nfdu8char_t* selectedPath = nullptr;
		const nfdresult_t result = NFD_SaveDialogU8(
			&selectedPath,
			filterList,
			1,
			defaultPath.empty() ? nullptr : defaultPath.c_str(),
			defaultName.c_str());
		if (result == NFD_CANCEL)
			return;
		if (result == NFD_ERROR)
		{
			_show_notification(MLF(
				"notification.publish_failed",
				GetDialogError().c_str()), true);
			return;
		}

		std::filesystem::path path(selectedPath);
		NFD_FreePathU8(selectedPath);
		if (!path.has_extension())
			path += ".xml";

		std::string error;
		if (!_bench.PublishProject(path, error))
		{
			_show_notification(MLF("notification.publish_failed", error.c_str()), true);
			return;
		}

		const std::string filename = path.filename().u8string();
		_show_notification(MLF("notification.published", filename.c_str()), false);
	}

	void PopApplication::_show_notification(std::string message, bool error)
	{
		_notification_message = std::move(message);
		_notification_error = error;
		_notification_until = ImGui::GetTime() + 3.0;
	}

	void PopApplication::_render_notification()
	{
		if (_notification_message.empty() ||
			ImGui::GetTime() >= _notification_until)
		{
			return;
		}

		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(
			{ viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
				viewport->WorkPos.y + 10.0f },
			ImGuiCond_Always,
			{ 0.5f, 0.0f });

		const wx::Float32 maxWidth = ImClamp(
			viewport->WorkSize.x - 20.0f,
			240.0f,
			640.0f);

		ImGui::SetNextWindowSizeConstraints(
			{ 0.0f, 0.0f },
			{ maxWidth, 1000.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
		ImGui::PushStyleColor(
			ImGuiCol_WindowBg,
			_notification_error
				? ImVec4(0.24f, 0.08f, 0.09f, 0.97f)
				: ImVec4(0.06f, 0.20f, 0.14f, 0.97f));
		ImGui::PushStyleColor(
			ImGuiCol_Border,
			_notification_error
				? ImVec4(0.85f, 0.25f, 0.28f, 1.0f)
				: ImVec4(0.20f, 0.72f, 0.45f, 1.0f));

		const ImGuiWindowFlags flags =
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoSavedSettings;
		if (ImGui::Begin("##project_notification", nullptr, flags))
		{
			// NoFocusOnAppearing keeps the toast from stealing focus, but also means
			// it is not guaranteed to be placed above the window that opened it.
			ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
			ImGui::PushTextWrapPos(
				maxWidth - ImGui::GetStyle().WindowPadding.x * 2.0f);
			ImGui::TextColored(
				_notification_error
					? ImVec4(1.0f, 0.58f, 0.58f, 1.0f)
					: ImVec4(0.62f, 1.0f, 0.76f, 1.0f),
				"%s  %s",
				_notification_error ? ICON_FA_CIRCLE_EXCLAMATION : ICON_FA_CIRCLE_CHECK,
				_notification_message.c_str());
			ImGui::PopTextWrapPos();
		}
		ImGui::End();

		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);

	}
}
