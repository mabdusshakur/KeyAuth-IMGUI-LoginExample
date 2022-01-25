#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "main.h"
#include <Windows.h>
#include "auth.hpp"
#include <string>


using namespace KeyAuth;

std::string name = ""; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = ""; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = ""; // app secret, the blurred text on licenses tab and other tabs
std::string version = "1.0"; // leave alone unless you've changed version on website

api KeyAuthApp(name, ownerid, secret, version);

static int null0 = 0;
static bool isWindowOpen = true;
char username[100] = "";
char password[100] = "";
char license[100] = "";

//To Hide Some Consoles That Become Visible and Close withing few seconds of starting
void hideStartupConsoleOnce()
{
    HWND Stealth;
    AllocConsole();
    Stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(Stealth, 0);
}
int main()
{
    hideStartupConsoleOnce();
    KeyAuthApp.init();
    if (!KeyAuthApp.data.success)
    {
        std::cout << "\n Status: " + KeyAuthApp.data.message;
        Sleep(1500);
        exit(0);
    }
    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("KeyAuth-Imgui-Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("KeyAuth-Imgui-Example"),  WS_POPUP, (desktop.right / 2) - (WindowWidth / 2), (desktop.bottom / 2) - (WindowHeight / 2), WindowWidth, WindowHeight, 0, 0, wc.hInstance, 0);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);

    if (CreateDeviceD3D(hwnd) < null0)
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);




    //Colors & styles & Customizations
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    ImGuiStyle* style = &ImGui::GetStyle();
    style->FrameRounding = 3.0f;
    style->WindowTitleAlign = ImVec2(0.3f, 0.3f);
    style->WindowRounding = 0;


    //Core Codes
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    DWORD dwFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        if (!isWindowOpen) ExitProcess(EXIT_SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        {
            ImGui::Begin("KeyAuth-Imgui-Example", &isWindowOpen, dwFlag);
            ImGui::AlignTextToFramePadding();
            ImGui::InputText("Username", username, sizeof(username), 0, 0, 0);
            ImGui::InputText("Password", password, sizeof(password), 0, 0, 0);
            ImGui::InputText("License", license, sizeof(license), 0, 0, 0);

            if (ImGui::Button("Register | Username, Password, License")) {
                KeyAuthApp.regstr(username, password, license);
                if (KeyAuthApp.data.success) {
                    MessageBoxA(0, "Success", KeyAuthApp.data.message.c_str(), 0);
                }
                else if (!KeyAuthApp.data.success) {
                    MessageBoxA(0, "Failed", KeyAuthApp.data.message.c_str(), 0);
                }
            }

            if (ImGui::Button("Login | Username And Password")) {
                KeyAuthApp.login(username, password);
                if (KeyAuthApp.data.success) {
                    MessageBoxA(0, "Success", KeyAuthApp.data.message.c_str(), 0);
                }
                else if (!KeyAuthApp.data.success) {
                    MessageBoxA(0, "Failed", KeyAuthApp.data.message.c_str(), 0);
                }
            }

            if (ImGui::Button("Login With License Only")) {
                KeyAuthApp.license(license);
                if (KeyAuthApp.data.success) {
                    MessageBoxA(0, "Success", KeyAuthApp.data.message.c_str(), 0);
                }
                else if(!KeyAuthApp.data.success) {
                    MessageBoxA(0, "Failed", KeyAuthApp.data.message.c_str(), 0);
                }
            }
            if (ImGui::Button("Upgrade | Username and License")) {
                KeyAuthApp.upgrade(username, license);
                if (KeyAuthApp.data.success) {
                    MessageBoxA(0, "Success", KeyAuthApp.data.message.c_str(), 0);
                }
                else if (!KeyAuthApp.data.success) {
                    MessageBoxA(0, "Failed", KeyAuthApp.data.message.c_str(), 0);
                }
            }
            ImGui::End();
        }

        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}