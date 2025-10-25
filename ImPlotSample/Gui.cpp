#include "Gui.h"
#include "IniWrapper.h"

// static メンバー定義 一度しか実行してはいけないため、基本的にはソースコード(.cpp)ファイルに記述
GLFWwindow* Gui::window_ = nullptr;
float Gui::monitorScale = 1.0f;
int Gui::monitorWidth = 0;
int Gui::monitorHeight = 0;
int Gui::windowFlag = 4; // ImGuiCond_FirstUseEver
Gui::WindowCfg Gui::window;

// ESCキーでウィンドウを閉じるコールバック
void HandleKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// 初期化処理
void Gui::Initialize(
    const char title[],
    const int windowPosX, const int windowPosY,
    const int windowWidth, const int windowHeight
) {
    IniWrapper ini;
    ini.load("config.ini");
    int windowPosX_ = ini.get<int>("Window", "PosX", windowPosX);
    int windowPosY_ = ini.get<int>("Window", "PosY", windowPosY);
    int windowWidth_ = ini.get<int>("Window", "Width", windowWidth);
    int windowHeight_ = ini.get<int>("Window", "Height", windowHeight);
	// GLFW 初期化
    if (!glfwInit()) {
        std::cerr << "[Error] Failed to initialize GLFW\n";
        return;
    }

    // OpenGL コンテキスト設定
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    int xpos, ypos;
    glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &monitorWidth, &monitorHeight);
    //std::cout << std::format("Monitor w:{}, h:{}", monitorWidth, monitorHeight) << std::endl;

    monitorScale = ImGui_ImplGlfw_GetContentScaleForMonitor(monitor); // Valid on GLFW 3.3+ only
    
    window_ = glfwCreateWindow(
        (int)(windowWidth_ * monitorScale),
        (int)(windowHeight_ * monitorScale),
        title, NULL, NULL
    );
    if (window_) {
        glfwSetWindowPos(window_, windowPosX_, windowPosY_);
    }
    
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // VSync 有効化

    // ImGui & ImPlot 初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.FontGlobalScale = 1.f; // 全体のフォントスケールを変更
    ImFont* myFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Lucon.ttf", 24.0f);
    ImGui::PushFont(myFont);

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(monitorScale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = monitorScale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)


    // バックエンド初期化
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // キー入力コールバック登録
    glfwSetKeyCallback(window_, HandleKeyInput);
}

// 終了処理
void Gui::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwGetWindowSize(window_, &(window.size.x), &(window.size.y));
    glfwGetWindowPos(window_, &(window.pos.x), &(window.pos.y));
	IniWrapper ini;
	ini.set("Window", "Width", window.size.x);
	ini.set("Window", "Height", window.size.y);
	ini.set("Window", "PosX", window.pos.x);
	ini.set("Window", "PosY", window.pos.y);
	ini.save("config.ini");

    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    glfwTerminate();
}

// フレーム開始処理
void Gui::BeginFrame() {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

// フレーム描画処理
void Gui::EndFrame() {
    if (!window_) return;

    ImGui::Render();

    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window_);
}

void Gui::setStyle(const int theme) {
    // ImGuiスタイル設定
    if (theme == 0) ImGui::StyleColorsDark(); // Default
    else if (theme == 1) ImGui::StyleColorsClassic();
    else if (theme == 2) ImGui::StyleColorsLight();
    else if (theme == 3) ImGui_SetGrayTheme();
    else if (theme == 4) ImGui_SetNeonBlueTheme();
    else if (theme == 5) ImGui_SetNeonGreenTheme();
    else if (theme == 6) ImGui_SetNeonRedTheme();
    else if (theme == 7) ImGui_SetEvaTheme();
}
void Gui::ImGui_SetGrayTheme()
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // タイトルバーの色設定
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

    // 背景とウィンドウ
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);

    // ボーダーとフレーム
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);

    // ボタン
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

    // スライダーとチェックボックス
    colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.40f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.50f, 0.95f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.60f, 0.90f, 1.00f);

    // タブ
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);

    // スタイル設定（角丸やパディングなど）
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 4.0f;
}

void Gui::ImGui_SetNeonBlueTheme()
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.Colors;

    // タイトルバーの色設定（青系ネオン）
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.30f, 0.50f, 1.00f); // 非アクティブ時
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.80f, 1.00f, 1.00f); // アクティブ時
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.20f, 0.35f, 1.00f); // 折りたたみ時

    // 背景：ダークネイビー
    colors[ImGuiCol_WindowBg] = ImVec4(0.02f, 0.05f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.03f, 0.06f, 0.12f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.08f, 0.15f, 1.00f);

    // フレームとボーダー：ブルーグロー
    colors[ImGuiCol_Border] = ImVec4(0.00f, 0.70f, 1.00f, 0.60f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.30f, 0.50f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.70f, 1.00f, 1.00f);

    // ボタン：ネオンブルー
    colors[ImGuiCol_Button] = ImVec4(0.00f, 0.40f, 0.70f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.60f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.80f, 1.00f, 1.00f);

    // スライダーとチェック：エレクトリックブルー
    colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.90f, 1.00f, 1.00f);

    // タブ：ブルーグラデーション
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.30f, 0.50f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.00f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.80f, 1.00f, 1.00f);

    // スタイル：シャープで近未来的な印象
    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding = 3.0f;
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
}

void Gui::ImGui_SetNeonGreenTheme()
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // タイトルバーの色設定（緑系ネオン）
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.40f, 0.20f, 1.00f); // 非アクティブ時
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.80f, 0.40f, 1.00f); // アクティブ時
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.25f, 0.12f, 1.00f); // 折りたたみ時

    // 背景：ダークグリーン系
    colors[ImGuiCol_WindowBg] = ImVec4(0.02f, 0.08f, 0.05f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.03f, 0.10f, 0.06f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.12f, 0.08f, 1.00f);

    // フレームとボーダー：ネオングリーンの輝き
    colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 0.50f, 0.60f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.40f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.60f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.80f, 0.40f, 1.00f);

    // ボタン：鮮やかなグリーン
    colors[ImGuiCol_Button] = ImVec4(0.00f, 0.60f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.80f, 0.40f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 0.50f, 1.00f);

    // スライダーとチェック：エレクトリックグリーン
    colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 0.60f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 1.00f, 0.70f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 0.50f, 1.00f);

    // タブ：グリーングラデーション
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.40f, 0.20f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.00f, 0.70f, 0.35f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.90f, 0.45f, 1.00f);

    // スタイル：シャープで近未来的な印象
    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding = 3.0f;
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
}

void Gui::ImGui_SetNeonRedTheme()
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // タイトルバーの色設定（赤系ネオン）
    colors[ImGuiCol_TitleBg] = ImVec4(0.40f, 0.00f, 0.00f, 1.00f); // 通常
    colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 0.20f, 0.20f, 1.00f); // アクティブ時
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.30f, 0.00f, 0.00f, 1.00f); // 折りたたみ時

    // 背景：ダークグレー〜ブラック
    colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.02f, 0.02f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.03f, 0.03f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.04f, 0.04f, 1.00f);

    // フレームとボーダー：赤い光の縁取り
    colors[ImGuiCol_Border] = ImVec4(1.00f, 0.20f, 0.20f, 0.60f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.40f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.60f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);

    // ボタン：ネオンレッド
    colors[ImGuiCol_Button] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.40f, 0.40f, 1.00f);

    // スライダーとチェック：エレクトリックレッド
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.40f, 0.40f, 1.00f);

    // タブ：赤いグラデーション
    colors[ImGuiCol_Tab] = ImVec4(0.50f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.20f, 0.20f, 1.00f);

    // スタイル：シャープでエッジの効いた印象
    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding = 3.0f;
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
}

void Gui::ImGui_SetEvaTheme()
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 2.0f;
    style.ScrollbarRounding = 3.0f;
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.WindowPadding = ImVec2(8, 8);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.08f, 1.00f); // 黒紫
    colors[ImGuiCol_Border] = ImVec4(0.3f, 0.0f, 0.4f, 0.6f);     // 紫
    colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.0f, 0.4f, 0.8f);     // 紫
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.5f, 0.0f, 0.7f, 1.00f);     // 濃紫
    colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.3f, 0.0f, 1.00f);     // 初号機の緑
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.5f, 0.0f, 1.00f);     // 明るい緑
    colors[ImGuiCol_Button] = ImVec4(0.3f, 0.0f, 0.4f, 1.00f);     // 紫
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.5f, 0.0f, 0.7f, 1.00f);     // 濃紫
    colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.6f, 0.0f, 1.00f);     // 緑
    colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.8f, 0.0f, 1.00f);     // 緑
    colors[ImGuiCol_SliderGrab] = ImVec4(0.6f, 0.0f, 0.8f, 1.00f);     // 紫
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.0f, 1.0f, 1.00f);     // 明るい紫
    colors[ImGuiCol_Header] = ImVec4(0.2f, 0.0f, 0.3f, 1.00f);     // 紫
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.0f, 0.6f, 1.00f);     // 濃紫
    colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.6f, 0.0f, 1.00f);     // 緑
}

// Implementing a simple custom widget using the public API.
// You may also use the <imgui_internal.h> API to get raw access to more data/helpers, however the internal API isn't guaranteed to be forward compatible.
// FIXME: Need at least proper label centering + clipping (internal functions RenderTextClipped provides both but api is flaky/temporary)
bool MyKnob(const char* label, float* p_value, float v_min, float v_max)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    float radius_outer = 20.0f;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
    float line_height = ImGui::GetTextLineHeight();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float ANGLE_MIN = 3.141592f * 0.75f;
    float ANGLE_MAX = 3.141592f * 2.25f;

    ImGui::InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 2 + line_height + style.ItemInnerSpacing.y));
    bool value_changed = false;
    bool is_active = ImGui::IsItemActive();
    bool is_hovered = ImGui::IsItemActive();
    if (is_active && io.MouseDelta.x != 0.0f)
    {
        float step = (v_max - v_min) / 200.0f;
        *p_value += io.MouseDelta.x * step;
        if (*p_value < v_min) *p_value = v_min;
        if (*p_value > v_max) *p_value = v_max;
        value_changed = true;
    }

    float t = (*p_value - v_min) / (v_max - v_min);
    float angle = ANGLE_MIN + (ANGLE_MAX - ANGLE_MIN) * t;
    float angle_cos = cosf(angle), angle_sin = sinf(angle);
    float radius_inner = radius_outer * 0.40f;
    draw_list->AddCircleFilled(center, radius_outer, ImGui::GetColorU32(ImGuiCol_FrameBg), 16);
    draw_list->AddLine(ImVec2(center.x + angle_cos * radius_inner, center.y + angle_sin * radius_inner), ImVec2(center.x + angle_cos * (radius_outer - 2), center.y + angle_sin * (radius_outer - 2)), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), 2.0f);
    draw_list->AddCircleFilled(center, radius_inner, ImGui::GetColorU32(is_active ? ImGuiCol_FrameBgActive : is_hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 16);
    draw_list->AddText(ImVec2(pos.x, pos.y + radius_outer * 2 + style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), label);

    if (is_active || is_hovered)
    {
        ImGui::SetNextWindowPos(ImVec2(pos.x - style.WindowPadding.x, pos.y - line_height - style.ItemInnerSpacing.y - style.WindowPadding.y));
        ImGui::BeginTooltip();
        ImGui::Text("%.3f", *p_value);
        ImGui::EndTooltip();
    }

    return value_changed;
}

bool MyKnobInt(const char* label, int* p_value, const int v_min, const int v_max)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    float radius_outer = 20.0f;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
    float line_height = ImGui::GetTextLineHeight();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float ANGLE_MIN = 3.141592f * 0.75f;
    float ANGLE_MAX = 3.141592f * 2.25f;

    ImGui::InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 2 + line_height + style.ItemInnerSpacing.y));
    bool value_changed = false;
    bool is_active = ImGui::IsItemActive();
    bool is_hovered = ImGui::IsItemActive();
    if (is_active && io.MouseDelta.x != 0.0f)
    {
        float step = 1;
        *p_value += io.MouseDelta.x * step;
        if (*p_value < v_min) *p_value = v_min;
        if (*p_value > v_max) *p_value = v_max;
        value_changed = true;
    }

    float t = (float)(*p_value - v_min) / (v_max - v_min);
    float angle = ANGLE_MIN + (ANGLE_MAX - ANGLE_MIN) * t;
    float angle_cos = cosf(angle), angle_sin = sinf(angle);
    float radius_inner = radius_outer * 0.40f;
    draw_list->AddCircleFilled(center, radius_outer, ImGui::GetColorU32(ImGuiCol_FrameBg), 16);
    draw_list->AddLine(ImVec2(center.x + angle_cos * radius_inner, center.y + angle_sin * radius_inner), ImVec2(center.x + angle_cos * (radius_outer - 2), center.y + angle_sin * (radius_outer - 2)), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), 2.0f);
    draw_list->AddCircleFilled(center, radius_inner, ImGui::GetColorU32(is_active ? ImGuiCol_FrameBgActive : is_hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 16);
    draw_list->AddText(ImVec2(pos.x, pos.y + radius_outer * 2 + style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), label);

    if (is_active || is_hovered)
    {
        ImGui::SetNextWindowPos(ImVec2(pos.x - style.WindowPadding.x, pos.y - line_height - style.ItemInnerSpacing.y - style.WindowPadding.y));
        ImGui::BeginTooltip();
        ImGui::Text("%d", *p_value);
        ImGui::EndTooltip();
    }

    return value_changed;
}
