#include "ModuleUI.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleCamera.h"
#include "ModuleModelLoader.h"
#include "ModuleProgram.h"
#include "ModuleInput.h"
#include <string>
#include <SDL.h>
#include <il.h>
#include <ilu.h>
#include <ilut.h>
#include <glew.h>

#define RED ImVec4(1.0F, 0.0F, 0.0F, 1.0F)
#define GREEN ImVec4(0.0F, 1.0F, 0.0F, 1.0F)
#define BLUE ImVec4(0.0F, 0.0F, 1.0F, 1.0F)
#define PINK ImVec4(1.0F, 0.0F, 1.0F, 1.0F)
#define YELLOW ImVec4(1.0F, 1.0F, 0.0F, 1.0F)
#define LIGHT_BLUE ImVec4(0.0F, 1.0F, 1.0F, 1.0F)

bool ModuleUI::Init() {
	LOG("Init Module UI\n");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = new ImGuiIO();
	*io = ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(&App->window->GetWindow(), App->renderer->GetContext());
	ImGui_ImplOpenGL3_Init("#version 430 core");
	width = App->window->GetWidth();
	height = App->window->GetHeight();
	screenWidth = App->window->GetDesktopWidth();
	screenHeight = App->window->GetDesktopHeight();
	return true;
}

bool ModuleUI::Start() {
	if (SDL_Has3DNow()) caps += "3DNow, ";
	if (SDL_HasAVX()) caps += "AVX, ";
	if (SDL_HasAVX2()) caps += "AVX2, ";
	if (SDL_HasMMX()) caps += "MMX, ";
	if (SDL_HasRDTSC()) caps += "RDTSC, ";
	if (SDL_HasSSE()) caps += "SSE, ";
	if (SDL_HasSSE2()) caps += "SSE2, ";
	if (SDL_HasSSE3()) caps += "SSE3, ";
	if (SDL_HasSSE41()) caps += "SSE41, ";
	if (SDL_HasSSE42()) caps += "SSE42";
	return true;
}

UpdateStatus ModuleUI::PreUpdate() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(&App->window->GetWindow());
	ImGui::NewFrame();
	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleUI::Update() {
	currentTime = SDL_GetTicks();

	UpdateStatus ret = DrawMainBar();

	if (showSceneWindow)
		DrawSceneWindow(&showSceneWindow);

	if (showConfigWindow)
		DrawConfigWindow(&showConfigWindow);

	if (showConsoleLogWindow)
		DrawConsoleLogWindow(&showConsoleLogWindow);

	if (showPropertiesWindow)
		DrawPropertiesWindow(&showPropertiesWindow);

	if (showAboutWindow)
		DrawAboutWindow(&showAboutWindow);

	if (showDemoWindow)
		ImGui::ShowTestWindow(&showDemoWindow);

	return ret;
}

UpdateStatus ModuleUI::PostUpdate() {
	ImGui::Render();
	glViewport(0, 0, io->DisplaySize.x, io->DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(&App->window->GetWindow());
	return UpdateStatus::CONTINUE;
}

bool ModuleUI::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	delete io;
	return true;
}

void ModuleUI::ProcessEvent(const SDL_Event *event) const {
	ImGui_ImplSDL2_ProcessEvent(event);
}

void ModuleUI::SetEditorWidth(int newWidth) {
	width = newWidth;
}

void ModuleUI::SetEditorHeight(int newHeight) {
	height = newHeight;
}

UpdateStatus ModuleUI::DrawMainBar() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File")) {
		//ImGui::CheckboxFlags("io.ConfigFlags: DockingEnable", (unsigned int *)&io->ConfigFlags, ImGuiConfigFlags_DockingEnable);
		if (ImGui::MenuItem("Quit", "Escape"))
			return UpdateStatus::STOP;
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window")) {
		ImGui::MenuItem("Configuration", "Ctrl+Alt+C", &showConfigWindow);
		ImGui::MenuItem("Console", "Ctrl+Shift+C", &showConsoleLogWindow);
		ImGui::MenuItem("Properties", "Ctrl+Shift+P", &showPropertiesWindow);
		ImGui::MenuItem("Scene", "Ctrl+Shift+S", &showSceneWindow);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help")) {
		ImGui::MenuItem((std::string("About ") + std::string(TITLE)).c_str(), nullptr, &showAboutWindow);
		ImGui::Separator();
		if (ImGui::MenuItem("GitHub Repository"))
			RequestBrowser("https://github.com/JoanStinson/Stinson_Engine");
		if (ImGui::MenuItem("Documentation"))
			RequestBrowser("https://github.com/JoanStinson/Stinson_Engine/wiki");
		if (ImGui::MenuItem("Download Latest"))
			RequestBrowser("https://github.com/JoanStinson/Stinson_Engine/releases");
		if (ImGui::MenuItem("Report a Bug"))
			RequestBrowser("https://github.com/JoanStinson/Stinson_Engine/issues");
		ImGui::Separator();
		ImGui::MenuItem("Gui Demo", nullptr, &showDemoWindow);
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
	return UpdateStatus::CONTINUE;
}

void ModuleUI::DrawSceneWindow(bool *p_open) {

	//static ImGuiID dockspaceID = 0;
	//bool active = true;
	//if (ImGui::Begin("Master Window", &active, ImGuiDockNodeFlags_PassthruCentralNode)) {
	//	ImGui::TextUnformatted("DockSpace below");
	//}
	//if (active) {
	//	dockspaceID = ImGui::GetID("HUB_1ockSpace");
	//	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode/*|ImGuiDockNodeFlags_NoResize*/);
	//}
	//ImGui::End();

	//ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
	//if (ImGui::Begin("Dockable Window")) {
	//	ImGui::TextUnformatted("Test");
	//}
	//ImGui::End();

	if (ImGui::Begin("Scene", p_open)) {
		ImGui::GetWindowDrawList()->AddImage(
			(void *)App->renderer->GetRenderTexture(), ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + App->window->GetWidth() / 1.5, ImGui::GetCursorScreenPos().y + App->window->GetHeight() / 1.5), ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
}

void ModuleUI::DrawConfigWindow(bool *p_open) {
	if (ImGui::Begin("Configuration", p_open)) {
		ImGui::Text("Welcome to %s!", TITLE);
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Camera"))
			DrawCameraHeader();

		if (ImGui::CollapsingHeader("Hardware"))
			DrawHardwareHeader();

		if (ImGui::CollapsingHeader("Input"))
			DrawInputHeader();

		if (ImGui::CollapsingHeader("Performance"))
			DrawPerformanceHeader();

		if (ImGui::CollapsingHeader("Renderer"))
			DrawRendererHeader();

		if (ImGui::CollapsingHeader("Textures"))
			DrawTexturesHeader();

		if (ImGui::CollapsingHeader("Window"))
			DrawWindowHeader();
	}
	ImGui::End();
}

void ModuleUI::DrawPropertiesWindow(bool *p_open) {
	if (ImGui::Begin("Properties", p_open)) {

		if (ImGui::CollapsingHeader("Transform")) {

		}
		if (ImGui::CollapsingHeader("Geometry")) {

		}
		if (ImGui::CollapsingHeader("Texture")) {

		}

	}
	ImGui::End();
}

void ModuleUI::DrawAboutWindow(bool *p_open) {
	if (ImGui::Begin((std::string("About ") + std::string(TITLE)).c_str(), p_open)) {
		ImGui::Text(TITLE);
		ImGui::SameLine();
		ImGui::TextColored(YELLOW, "%s WIP", &version[0]);
		ImGui::Separator();
		ImGui::Text("Description:");
		ImGui::SameLine();
		ImGui::TextColored(LIGHT_BLUE, "%s", &description[0]);
		ImGui::Text("Credits:");
		ImGui::SameLine();
		ImGui::TextColored(PINK, "%s", &author[0]);
		ImGui::Separator();
		ImGui::Text("%s is licensed under the MIT License.", TITLE);
		if (ImGui::Button("GitHub Repository"))
			RequestBrowser("https://github.com/JoanStinson/Stinson_Engine");
		ImGui::SameLine();
		if (ImGui::Button("MIT License"))
			RequestBrowser("https://github.com/JoanStinson/Stinson_Engine/blob/master/LICENSE");
		ImGui::SameLine();
		HelpMarker("See LICENSE for more information");
	}
	ImGui::End();
}

void ModuleUI::DrawCameraHeader() {
	static float fov = App->camera->frustum.verticalFov;
	static float aspectRatio = App->camera->aspectRatio;
	static ImVec4 color = ImVec4(114.0F / 255.0F, 144.0F / 255.0F, 154.0F / 255.0F, 200.0F / 255.0F);

	if (ImGui::DragFloat3("Front", (float*)&App->camera->frustum.front, 0.1F)) {
		App->camera->CalculateMatrixes();
	}
	if (ImGui::DragFloat3("Up", (float*)&App->camera->frustum.up, 0.1F)) {
		App->camera->CalculateMatrixes();
	}
	if (ImGui::DragFloat3("Position", (float*)&App->camera->frustum.pos, 0.1F)) {
		App->camera->CalculateMatrixes();
	}
	ImGui::DragFloat("Mov Speed", &App->camera->movSpeed, 0.1F);
	ImGui::DragFloat("Rot Speed", &App->camera->rotSpeed, 0.1F);
	ImGui::DragFloat("Zoom Speed", &App->camera->zoomSpeed, 0.1F);
	if (ImGui::DragFloat("Near Plane", &App->camera->frustum.nearPlaneDistance, 0.1F)) {
		App->camera->CalculateMatrixes();
	}
	if (ImGui::DragFloat("Far Plane", &App->camera->frustum.farPlaneDistance, 0.1F)) {
		App->camera->CalculateMatrixes();
	}
	if (ImGui::DragFloat("Field of View", &fov, 0.1F)) {
		App->camera->SetFOV(fov);
	}
	if (ImGui::DragFloat("Aspect Ratio", &aspectRatio, 0.1F)) {
		App->camera->SetAspectRatio(aspectRatio);
	}
	if (ImGui::Button("Reset Camera")) {
		App->camera->ResetCamera();
		fov = App->camera->frustum.verticalFov;
		aspectRatio = App->camera->aspectRatio;
		color = ImVec4(114.0F / 255.0F, 144.0F / 255.0F, 154.0F / 255.0F, 200.0F / 255.0F);
	}
}

void ModuleUI::DrawHardwareHeader() {
	ImGui::Text("CPUs:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%d", SDL_GetCPUCount());
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "(Cache: %dkb)", SDL_GetCPUCacheLineSize());
	ImGui::Text("System RAM:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%.1fGb", (float)SDL_GetSystemRAM() / 1000);
	ImGui::Text("Caps:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%s", &caps[0]);
	ImGui::Separator();
	ImGui::Text("GPU:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%s", glGetString(GL_RENDERER));
	ImGui::Text("Brand:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%s", glGetString(GL_VENDOR));
	static GLint totalMemoryKb = 0;
	static GLint currentMemoryKb = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemoryKb);
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentMemoryKb);
	ImGui::Text("VRAM Budget:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%.1f Mb", (float)totalMemoryKb / 1000);
	ImGui::Text("VRAM Usage:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%.1f Mb", ((float)totalMemoryKb / 1000) - ((float)currentMemoryKb / 1000));
	ImGui::Text("VRAM Available:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%.1f Mb", (float)currentMemoryKb / 1000);
	ImGui::Text("VRAM Reserved:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%.1f Mb", 0.0F);
	ImGui::Separator();
	static SDL_version compiled;
	SDL_VERSION(&compiled);
	ImGui::Text("SDL Version:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%d.%d.%d", compiled.major, compiled.minor, compiled.patch);
	ImGui::Text("Glew Version:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%s", glewGetString(GLEW_VERSION));
	ImGui::Text("DevIL Version:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%d.%d.%d", (ilGetInteger(IL_VERSION_NUM) % 1000) / 100, (ilGetInteger(IL_VERSION_NUM) % 100) / 10, ilGetInteger(IL_VERSION_NUM) % 10);
	ImGui::Text("OpenGL Version:");
	ImGui::SameLine();
	ImGui::TextColored(YELLOW, "%s", glGetString(GL_VERSION));
}

void ModuleUI::DrawInputHeader() {
	ImGui::Text("Mouse Position: (%d, %d)", App->input->GetMousePosition().x, App->input->GetMousePosition().y);
	ImGui::Text("Mouse Motion: (%d, %d)", App->input->GetMouseMotion().x, App->input->GetMouseMotion().y);
	ImGui::Text("Mouse Wheel: %d", App->input->GetMouseWheel()->y);
	ImGui::Text("Mouse Right Click: %d", App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::REPEAT);
	ImGui::Text("Mouse Left Click: %d", App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::REPEAT);
	ImGui::Text("Mouse Wheel Click: %d", App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KeyState::REPEAT);
	ImGui::Text("Keys Pressed:");
	for (int i = 0; i < MAX_KEYS; ++i) {
		if (App->input->GetKey(i) == KeyState::REPEAT) {
			ImGui::SameLine();
			char key = i;
			if (key == -30) ImGui::Text("ALT");
			else if (key == 39) ImGui::Text("0,");
			else if (key > 29 && key < 39) ImGui::Text("%c,", key + 19);
			else ImGui::Text("%c,", key + 61);
		}
	}
}

void ModuleUI::DrawPerformanceHeader() {
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0F / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	DrawLogFrames(fpsLog, ImGui::GetIO().Framerate, fpsTitle, sizeof(fpsTitle) / sizeof(fpsTitle[0]), "Framerate %.1f", "##framerate", 100.0F, fpsTime);
	DrawLogFrames(msLog, 1000.0F / ImGui::GetIO().Framerate, msTitle, sizeof(msTitle) / sizeof(msTitle[0]), "Milliseconds %0.1f", "##milliseconds", 40.0F, msTime);
}

void ModuleUI::DrawRendererHeader() {
}

void ModuleUI::DrawTexturesHeader() {
	if (ImGui::BeginMenu("Wrapping Mode")) {
		if (ImGui::Checkbox("Repeat", &showRepeat)) {
			//if (showRepeat)
				//App->renderer->texture = App->textures->Load("../Resources/Assets/link.dds", App->renderer->imageInfo, 1, 0, true);
		}
		ImGui::SameLine();
		HelpMarker("Repeats the texture image");
		if (ImGui::Checkbox("Mirrored Repeat", &showMirroredRepeat)) {
			//if (showMirroredRepeat)
				//App->renderer->texture = App->textures->Load("../Resources/Assets/link.dds", App->renderer->imageInfo, 2, 0, true);
		}
		ImGui::SameLine();
		HelpMarker("Same as repeat, but mirrors the image with each repeat");
		if (ImGui::Checkbox("Clamp to Edge", &showClampToEdge)) {
			//if (showClampToEdge)
				//App->renderer->texture = App->textures->Load("../Resources/Assets/link.dds", App->renderer->imageInfo, 3, 0, true);
		}
		ImGui::SameLine();
		HelpMarker("Clamps the coordinates between 0 and 1. The result is that higher coordinates become clamped to the edge, resulting in a stretched edge pattern");
		if (ImGui::Checkbox("Clamp to Border", &showClampToBorder)) {
			//if (showClampToBorder)
				//App->renderer->texture = App->textures->Load("../Resources/Assets/link.dds", App->renderer->imageInfo, 4, 0, true);
		}
		ImGui::SameLine();
		HelpMarker("Coordinates outside the range are now given a user-specified border color");
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Filtering Mode")) {
		if (ImGui::Checkbox("Nearest", &showNearest)) {
			//if (showNearest)
				//App->renderer->texture = App->textures->Load("../Resources/Assets/link.dds", App->renderer->imageInfo, 4, 1, false);
		}
		ImGui::SameLine();
		HelpMarker("OpenGL selects the pixel which center is closest to the texture coordinate");
		if (ImGui::Checkbox("Linear", &showLinear)) {
			//if (showLinear)
				//App->renderer->texture = App->textures->Load("../Resources/Assets/link.dds", App->renderer->imageInfo, 4, 0, false);
		}
		ImGui::SameLine();
		HelpMarker("Takes an interpolated value from the texture coordinate's neighboring texels, approximating a color between the texels");
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Mipmaps", &showMipMaps)) {
		if (ImGui::Checkbox("Yes", &showMipMaps)) {
			//if (showMipMaps)
				//App->model->ChangeTexture(App->textures->Load(App->model->activeMesh->filename, App->model->activeTexture, 4, 0, true));
		}
		ImGui::SameLine();
		HelpMarker("After a certain distance threshold from the viewer, OpenGL will use a different mipmap texture that best suits the distance to the object. Because the object is far away, the smaller resolution will not be noticeable to the user. Also, mipmaps have the added bonus feature that they're good for performance as well.");
		ImGui::EndMenu();
	}

	ImGui::Separator();
	ImGui::Text("Width:");
	ImGui::SameLine();
	ImGui::TextColored(LIGHT_BLUE, "%dpx", App->model->activeTexture->Width);
	ImGui::Text("Height:");
	ImGui::SameLine();
	ImGui::TextColored(LIGHT_BLUE, "%dpx", App->model->activeTexture->Height);
	ImGui::Text("Depth:");
	ImGui::SameLine();
	ImGui::TextColored(LIGHT_BLUE, "%d", App->model->activeTexture->Depth);
	ImGui::Text("BPP:");
	ImGui::SameLine();
	ImGui::TextColored(LIGHT_BLUE, "%d", App->model->activeTexture->Bpp);
	ImGui::Text("Format:");
	ImGui::SameLine();
	ILenum format = App->model->activeTexture->Type;
	if (format == (int)IL_PNG)		ImGui::TextColored(LIGHT_BLUE, "PNG");
	else if (format == (int)IL_JPG) ImGui::TextColored(LIGHT_BLUE, "JPG");
	else if (format == (int)IL_DDS) ImGui::TextColored(LIGHT_BLUE, "DDS");
	else ImGui::TextColored(RED, "INVALID");
	if (ImGui::Checkbox("Checkers Texture", &checkers)) {
		if (checkers)
			App->model->ChangeTexture(App->textures->Load("../Resources/Assets/Textures/Checkers.jpg", App->model->activeTexture), false);
		else
			App->model->ChangeTexture(App->model->previousTexture);
	}
}

void ModuleUI::DrawWindowHeader() {
	if (ImGui::SliderFloat("Brightness", &brightness, 0.0F, 1.0F))
		App->window->SetWindowBrightness(brightness);
	if (ImGui::SliderInt("Width", &width, 0, screenWidth))
		App->window->SetWindowSize(width, height);
	if (ImGui::SliderInt("Height", &height, 0, screenHeight))
		App->window->SetWindowSize(width, height);
	ImGui::Text("Refresh rate:");
	ImGui::SameLine();
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	ImGui::TextColored(ImVec4(1.0F, 1.0F, 0.0F, 1.0F), "%d", current.refresh_rate);
	if (ImGui::Checkbox("Fullscreen", &fullscreen))
		App->window->SetFullscreen(fullscreen);
	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &resizable))
		App->window->SetResizable(resizable);
	if (ImGui::Checkbox("Borderless", &borderless))
		App->window->SetBorderless(borderless);
	ImGui::SameLine();
	if (ImGui::Checkbox("Full Desktop", &fullDesktop))
		App->window->SetFullDesktop(fullDesktop);
}

void ModuleUI::DrawLogFrames(std::vector<float> &frames, float value, char *titleBuf, int titleSize, const char *title, const char *histogramTitle, float maxHeight, unsigned int &time) {
	if (frames.size() < 1)
		frames.push_back(value);
	else if (currentTime > time + 1000) {
		if (frames.size() > 100) {
			for (size_t i = 1; i < frames.size(); ++i)
				frames[i - 1] = frames[i];
			frames[frames.size() - 1] = value;
		}
		else frames.push_back(value);
		time = currentTime;
	}
	sprintf_s(titleBuf, titleSize, title, frames[frames.size() - 1]);
	ImGui::PlotHistogram(histogramTitle, &frames[0], frames.size(), 0, titleBuf, 0.0F, maxHeight, ImVec2(310, 100));
}

void ModuleUI::HelpMarker(const char *desc, bool showText) const {
	if (showText) ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0F);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void ModuleUI::RequestBrowser(const char *link) const {
	ShellExecuteA(nullptr, "open", link, nullptr, nullptr, SW_SHOWNORMAL);
}
