#include "UI.hpp"
#include "structs.hpp"
#include "CustomFileDialog.hpp"
#include "structs.hpp"
#include "stdafx.h"
#include <glm/gtc/type_ptr.hpp>
#include "imgui_internal.h"
#include <filesystem>
#include "ASSIMPio.hpp"

namespace TDModelView 
{
    void UI::SetDefaultStyle()
    {
        ImGuiStyle* style = &ImGui::GetStyle();
        style->Alpha = 1.000000;
        style->ChildRounding = 5.000000;
        style->ChildBorderSize = 2.000000;
        style->PopupRounding = 5.000000;
        style->PopupBorderSize = 2.000000;
        style->FramePadding = ImVec2(10, 10);
        style->FrameRounding = 5.000000;
        style->FrameBorderSize = 1.000000;
        style->ItemSpacing = ImVec2(10, 14);
        style->ItemInnerSpacing = ImVec2(10, 3);
        style->CellPadding = ImVec2(4, 4);
        style->TouchExtraPadding = ImVec2(0, 0);
        style->IndentSpacing = 26.000000;
        style->ColumnsMinSpacing = 6.000000;
        style->ScrollbarSize = 14.000000;
        style->ScrollbarRounding = 5.000000;
        style->GrabMinSize = 9.000000;
        style->GrabRounding = 2.000000;
        style->TabRounding = 5.000000;
        style->TabBorderSize = 1.000000;
        //style->TabMinWidthForUnselectedCloseButton = 0.000000;
        style->ColorButtonPosition = 1;
        style->ButtonTextAlign = ImVec2(0.5, 0.5);
        style->SelectableTextAlign = ImVec2(0, 0);
        style->DisplayWindowPadding = ImVec2(19, 19);
        style->DisplaySafeAreaPadding = ImVec2(15, 0);
        style->MouseCursorScale = 1.000000;
        style->AntiAliasedLines = 1;
        style->AntiAliasedFill = 1;
        style->CurveTessellationTol = 1.250000;
        //style->CircleSegmentMaxError = 1.600000;
        style->WindowPadding = ImVec2(5, 15);
        style->WindowRounding = 5.000000;
        style->WindowBorderSize = 2.000000;
        style->WindowMinSize = ImVec2(32, 32);
        style->WindowTitleAlign = ImVec2(0, 0.5);
        style->WindowMenuButtonPosition = 0;
        style->Colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 1);
        style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.6, 0.6, 0.6, 1);
        style->Colors[ImGuiCol_WindowBg] = ImVec4(0.94, 0.94, 0.94, 0.86);
        style->Colors[ImGuiCol_ChildBg] = ImVec4(0.8, 0.796078, 0.807843, 0.501961);
        style->Colors[ImGuiCol_PopupBg] = ImVec4(1, 1, 1, 0.6);
        style->Colors[ImGuiCol_Border] = ImVec4(0.925, 0.3725, 0.6078, 0.35);
        style->Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
        style->Colors[ImGuiCol_FrameBg] = ImVec4(0.925, 0.3725, 0.6078, 0.35);
        style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.258824, 0.588235, 0.588235, 0.4);
        style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.211765, 0.65098, 0.678431, 0.670588);
        style->Colors[ImGuiCol_TitleBg] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98, 0.98, 0.98, 0.53);
        style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69, 0.69, 0.69, 0.8);
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49, 0.49, 0.49, 0.8);
        style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49, 0.49, 0.49, 1);
        style->Colors[ImGuiCol_CheckMark] = ImVec4(0.203922, 0.780392, 0.780392, 1);
        style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.203922, 0.745098, 0.776471, 0.780392);
        style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.231373, 0.541176, 0.694118, 0.6);
        style->Colors[ImGuiCol_Button] = ImVec4(0.207843, 0.737255, 0.756863, 0.4);
        style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.258824, 0.588235, 0.588235, 1);
        style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.0980392, 0.529412, 0.541176, 1);
        style->Colors[ImGuiCol_Header] = ImVec4(0.14902, 0.588235, 0.596078, 0.309804);
        style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26, 0.80, 0.75, 0.6);
        style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.26, 0.89, 0.82, 1);
        style->Colors[ImGuiCol_Separator] = ImVec4(0.0705882, 0.380392, 0.34902, 0.619608);
        style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14, 0.44, 0.8, 0.78);
        style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.14, 0.44, 0.8, 1);
        style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.8, 0.8, 0.8, 0.56);
        style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26, 0.59, 0.98, 0.67);
        style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26, 0.59, 0.98, 0.95);
        style->Colors[ImGuiCol_Tab] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TabHovered] = ImVec4(0.0980392, 0.541176, 0.552941, 0.8);
        style->Colors[ImGuiCol_TabActive] = ImVec4(0.431373, 0.694118, 0.698039, 1);
        style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.486275, 0.92549, 0.933333, 0.984314);
        style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.466667, 0.919608, 0.913725, 1);
        style->Colors[ImGuiCol_TableBorderLight] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0980392, 0.541176, 0.552941, 0.8);
        style->Colors[ImGuiCol_TableRowBg] = ImVec4(0.431373, 0.694118, 0.698039, 1);
        style->Colors[ImGuiCol_PlotLines] = ImVec4(0.39, 0.39, 0.39, 1);
        style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1, 0.43, 0.35, 1);
        style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.9, 0.7, 0, 1);
        style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1, 0.45, 0, 1);
        style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.258824, 0.823529, 0.796078, 0.34902);
        style->Colors[ImGuiCol_DragDropTarget] = ImVec4(0.26, 0.59, 0.98, 0.95);
        style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.26, 0.59, 0.98, 0.8);
        style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.7, 0.7, 0.7, 0.7);
        style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.2, 0.2, 0.2, 0.2);
        style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.2, 0.2, 0.2, 0.35);
    }
    void UI::cursorCallback(GLFWwindow* window, double xpos, double ypos) {
        double deltaX = xpos - eng->ui->lastX;
        double deltaY = eng->ui->lastY - ypos; // reversed since y-coordinates go from bottom to top
        eng->ui->lastX = xpos;
        eng->ui->lastY = ypos;
        if (eng->ui->firstMouse) {
            eng->ui->lastX = xpos;
            eng->ui->lastY = ypos;
            eng->ui->firstMouse = false;
        }
        if (eng->scene == nullptr){
            return;
        }
        static const double mouse_sensitivity_attenuation = 0.05;
        double dx = mouse_sensitivity_attenuation * deltaX;
        double dy = mouse_sensitivity_attenuation * deltaY;

        static glm::vec3 rot;
        rot = eng->scene->m_Camera.angles + 0.1f * glm::radians(glm::vec3(deltaY, deltaX, 0.0f));
        rot.z = 0.0;
        if (eng->ui->mouseLook) { 
            eng->scene->m_Camera.Rotate(rot);
        }
    }
    void UI::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            eng->ui->mousePressed[GLFW_MOUSE_BUTTON_LEFT] = true;
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
            eng->ui->mousePressed[GLFW_MOUSE_BUTTON_MIDDLE] = true;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            eng->ui->mousePressed[GLFW_MOUSE_BUTTON_RIGHT] = true;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            eng->ui->mousePressed[GLFW_MOUSE_BUTTON_LEFT] = false;
            eng->ui->mouseReleased[GLFW_MOUSE_BUTTON_LEFT] = true;
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
            eng->ui->mousePressed[GLFW_MOUSE_BUTTON_MIDDLE] = false;
            eng->ui->mouseReleased[GLFW_MOUSE_BUTTON_MIDDLE] = true;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            eng->ui->mousePressed[GLFW_MOUSE_BUTTON_RIGHT] = false;
            eng->ui->mouseReleased[GLFW_MOUSE_BUTTON_RIGHT] = true;
        }

        if (eng->ui->mousePressed[GLFW_MOUSE_BUTTON_MIDDLE]) {
            eng->ui->mouseLook = true;
        }
        else
            eng->ui->mouseLook = false;

        auto& io = ImGui::GetIO();
        if (action == GLFW_PRESS && button >= 0 && button < 3) {
            eng->ui->mousePressed[button] = true;
        }

        io.MouseClickedPos[button] = io.MousePos;
    }
    void deleteTextureHelper(ImTextureID tx_){
        GLuint tx = (GLuint)tx_;
        if(tx>0)
            glDeleteTextures(1, &tx);
    }
    void UI::terminate(){
        try{
            deleteTextureHelper(fileID);
            deleteTextureHelper(helpIconID);
            deleteTextureHelper(homeID);
            deleteTextureHelper(upArrowID);
            if (logoIcon)
                glDeleteTextures(1, &logoIcon);

            if (fontTexture)
            {
                glDeleteTextures(1, &fontTexture);
                ImGui::GetIO().Fonts->TexID = 0;
                fontTexture = 0;
            }

            ImGui_ImplGlfw_Shutdown();
            ImGui_ImplOpenGL3_Shutdown();
        }
        catch (std::exception e1){
            ErrorMessageBox(e1.what());
        } 
    }
	void UI::init()
	{
        if (isInitialized)
            return;

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.IniFilename = NULL;								   // Disable loading from .ini file.
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;
        glfwSetInputMode(eng->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        io.MouseDrawCursor = true;
        unsigned char* pixels;
        int width, height;
        std::vector<unsigned char> fnt = eng->ui->getDefaultFont();
        io.Fonts->AddFontFromMemoryTTF(fnt.data(), fnt.size(), 14.0f);
        TDModelView::CustomFileDialog::Instance()->runAfterLoadingAFont();
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        glGenTextures(1, &fontTexture);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        io.Fonts->TexID = (void*)(intptr_t)fontTexture;
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
#ifdef _WIN32
        io.ImeWindowHandle = glfwGetWin32Window(eng->window);
#endif
        //if (instantCallbacks)
        {
            glfwSetCursorPosCallback(eng->window, (GLFWcursorposfun)cursorCallback);
            glfwSetMouseButtonCallback(eng->window, MouseButtonCallback);
        }
        int w, h, display_w, display_h;
        glfwGetWindowSize(eng->window, &w, &h);
        glfwGetFramebufferSize(eng->window, &display_w, &display_h);
        io.DisplaySize.x = static_cast<float>(w);
        io.DisplaySize.y = static_cast<float>(h);
        io.DisplayFramebufferScale.x = static_cast<float>(display_w) / w;
        SetDefaultStyle();
        loadAllEmbeddedFiles();
        isInitialized = true;
	}
    void UI::render(){
        if (eng->windowClose)
            return;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        renderUIHelper();
        ImGui::EndFrame();
        if (eng && eng->window && !eng->windowClose) {
            ImGui::Render();
            ImDrawData* imdata = ImGui::GetDrawData();
            ImGui_ImplOpenGL3_RenderDrawData(imdata);
        }
    }
    void UI::TitleBarHelper()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Load##Model_titlebar", nullptr))
                {
                    CustomFileDialog::Instance()->OpenDialog(
                        "Load Model",
                        "Load Model",
                        this->fileDialogSize,
                        this->fileDialogPath);
                }
                if (ImGui::MenuItem("Exit##main_menu", nullptr))
                {
                    eng->windowClose = true;
                    glfwWindowShouldClose(eng->window);
                    glfwDestroyWindow(eng->window);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Options")) 
            {
                ImGui::Checkbox("Wireframe Mode", &eng->render->wireframeModeOn);
                if(ImGui::SliderFloat("Light Rotation", &lightRot, 0.0f, 1.0f)) {
                    float theta = 6.28318530718f * lightRot;
                    float phi = cos(2.0f * theta);
                    glm::vec3 L = (glm::vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta)));
                    eng->scene->m_Light.x = L.x;
                    eng->scene->m_Light.y = L.y;
                    eng->scene->m_Light.z = L.z;
                }
                ImGui::SliderFloat("Light Intensity", &eng->scene->m_Light.w, 0.0f, 50.0f);
                if (ImGui::Button("Camera to Center")) {
                    eng->scene->m_Camera.position = eng->scene->bbox.center();
                    eng->scene->m_Camera.Update();
                }
                ImGui::SliderFloat("Camera Speed", &eng->scene->m_Camera.movementSpeed, 0.0f, 20.0f);
                if (ImGui::Checkbox("Cull Backfaces", &eng->render->cullBackfaces))
                {
                    if (eng->render->cullBackfaces)
                        glEnable(GL_CULL_FACE);
                    else
                        glDisable(GL_CULL_FACE);
                }
                ImGui::Checkbox("Use Model Normals", &eng->render->useModelNormals);
                if(!eng->render->useModelNormals)
                    ImGui::Checkbox("Use Bump Maps", &eng->render->useBumpMaps);
                ImGui::SliderFloat("Ambient Light Blend", &eng->render->ambientLightBlend, 0.0f, 10.0f);
                ImGui::SliderFloat("Ambient Occlusion Strength", &eng->render->aoStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Reflection Strength", &eng->render->reflectionStrength, 0.0f, 10.0f);


                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Details"))
            {
                static auto& io = ImGui::GetIO();
                std::string str = "FPS: " + std::to_string(1.0f/io.DeltaTime);
                ImGui::Text(str.c_str());
                str = "# tris: " + std::to_string(eng->scene->triCount);
                ImGui::Text(str.c_str());
                str = "# verts: " + std::to_string(eng->scene->vertexCount);
                ImGui::Text(str.c_str());
                ImGui::EndMenu();
            }
            
            ImGui::EndMainMenuBar();
        }
    }
    void UI::renderUIHelper()
    {
        static auto& io = ImGui::GetIO();

        if (glfwGetWindowAttrib(eng->window, GLFW_RESIZABLE))
        {
            // setup display size (every frame to accommodate for window resizing)
            static int w, h, display_w, display_h;
            glfwGetWindowSize(eng->window, &w, &h);
            glfwGetFramebufferSize(eng->window, &display_w, &display_h);
            io.DisplaySize.x = static_cast<float>(w);
            io.DisplaySize.y = static_cast<float>(h);;
            io.DisplayFramebufferScale.x = static_cast<float>(display_w) / w;
            io.DisplayFramebufferScale.y = static_cast<float>(display_h) / h;
        }


        // setup time step
        static double current_time = glfwGetTime();
        io.DeltaTime = glm::abs(current_time - glfwGetTime());
        current_time = glfwGetTime();

        // Get mouse position in screen coordinates
        glfwGetCursorPos(eng->window, &mousePixels_x, &mousePixels_y);
        io.MousePosPrev = io.MousePos;
        io.MousePos = ImVec2(static_cast<float>(mousePixels_x),
            static_cast<float>(this->mousePixels_y));

        //// NEW CODE -- Use ImGui window space coordinates.
        this->mouseNDC_x = ((this->mousePixels_x / window_width) - 0.5) * 2.0;
        this->mouseNDC_y = -((this->mousePixels_y / window_height) - 0.5) * 2.0;

        // Get mouse delta.
        this->mouseDeltaX = this->mouseNDC_x - this->mouseLastNDC_x;
        this->mouseDeltaY = this->mouseNDC_y - this->mouseLastNDC_y;

        for (auto i = 0; i < 3; i++)
        {
            io.MouseDown[i] = this->mousePressed[i] ||
                glfwGetMouseButton(eng->window, i) != 0;
            // If a mouse press event came, always pass it as
            // "this frame", so we don't miss click-release events
            // that are shorter than 1 frame.
            this->mousePressed[i] = false;
        }

        // Replacement for scroll callback, which overrides imgui's default mousewheel funtionality.
        if (io.MouseWheel != 0.0f)
        {
            eng->scene->m_Camera.position += eng->scene->m_Camera.front * io.MouseWheel * eng->scene->m_Camera.movementSpeed;
            eng->scene->m_Camera.Update();
        }
       
        // ========================================================

        TitleBarHelper();

        if (showFileDialog)
            FileDialogModalPopup();
    }
    void UI::FileDialogModalPopup()
    {
        ImGui::SetNextWindowPos(ImVec2(
            0.5f * (window_width - fileDialogSize.x),
            0.5f * (window_height - fileDialogSize.y)
        ));
        ImGui::SetNextWindowSize(fileDialogSize, ImGuiCond_Always);

        if (CustomFileDialog::Instance()->FileDialog("Load Model", ImGuiWindowFlags_NoCollapse)) {
            if (CustomFileDialog::Instance()->IsOk == true) {
                try {
                    ASSIMPreader ai(CustomFileDialog::Instance()->GetFilepathName());
                }
                catch (std::exception e1) {
                    ErrorMessageBox(e1.what());
                }
            }
            CustomFileDialog::Instance()->CloseDialog("Load Model");
        }
    }
}