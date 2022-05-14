#include "renderer.h"

static ID3D11Device           * g_pd3dDevice           = nullptr;
static ID3D11DeviceContext    * g_pd3dDeviceContext    = nullptr;
static IDXGISwapChain         * g_pSwapChain           = nullptr;
static ID3D11RenderTargetView * g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D( HWND );
void CleanupDeviceD3D( );
void CreateRenderTarget( );
void CleanupRenderTarget( );
LRESULT __stdcall WndProc( HWND, UINT , WPARAM, LPARAM );

void theme::MenuTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // window size
    //style.WindowMinSize = ImVec2(600, 500);
    
    // window title bar
    style.WindowTitleAlign = ImVec2(0.5, 0.5);
    style.Colors[ImGuiCol_TitleBg] = colors::main_col;
    style.Colors[ImGuiCol_TitleBgActive] = colors::main_col;
    style.Colors[ImGuiCol_TitleBgCollapsed] = colors::main_col_less_alpha;

    /*style.FramePadding = ImVec2(8, 6);*/ // ser lidt grimt ud

    // border styling
    style.WindowBorderSize = 2.0f;
    style.Colors[ImGuiCol_Border] = colors::main_col; // border

    // background colors
    style.Colors[ImGuiCol_WindowBg] = colors::secondary_col;
    style.Colors[ImGuiCol_ChildBg] = colors::main_col; // child

    // buttons
    style.Colors[ImGuiCol_Button] = colors::buttonInactive;
    style.Colors[ImGuiCol_ButtonActive] = colors::buttonActive;
    style.Colors[ImGuiCol_ButtonHovered] = colors::buttonHovered;

    // text
    //style.Colors[ImGuiCol_Text] = colors::text_col;
 
}

void theme::MenuFonts() {
    ImGuiIO& io = ImGui::GetIO(); 

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\LeelaUIb.ttf", 15.0f); // default

    //ImFont* titleBar = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\LeelaUIb.ttf", 18.0f);

}

int StartRendering( ) 
{   
    const auto title = utils::random_string( );
    utils::spoof_exe( false );

    WNDCLASSEX wc = 
    { 
        sizeof( WNDCLASSEX ),
        CS_CLASSDC,
        WndProc, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        title.c_str(),
        0
    };

    RegisterClassExA( &wc );
    HWND hwnd = CreateWindowExA(0, wc.lpszClassName , title.c_str() , WS_SYSMENU , 100 , 100 , 1280 , 800 , 0 , 0 , wc.hInstance , 0 );

    if ( !CreateDeviceD3D( hwnd ) )
    {
        CleanupDeviceD3D( );
        UnregisterClassA( wc.lpszClassName , wc.hInstance );

        return 1;
    }

    ShowWindow( GetConsoleWindow( ) , SW_HIDE );
    ShowWindow( hwnd , SW_SHOWDEFAULT );
    UpdateWindow( hwnd );

    ImGui::CreateContext( );

    ImGuiIO& io = ImGui::GetIO( ); 

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    io.WantSaveIniSettings = false;

    theme::MenuTheme(); // apply menu styling
    theme::MenuFonts(); // apply menu fonts

    ImGui_ImplWin32_Init( hwnd );
    ImGui_ImplDX11_Init( g_pd3dDevice , g_pd3dDeviceContext );

    const auto clear_color = ImVec4( 0.45f , 0.55f , 0.60f , 1.00f );
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };

    while ( true )
    {
        MSG msg;

        while ( PeekMessageA( &msg , NULL , 0U , 0U , PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessageA( &msg );

            if ( msg.message == WM_QUIT )
                break;
        }

        ImGui_ImplDX11_NewFrame( );
        ImGui_ImplWin32_NewFrame( );
        ImGui::NewFrame( );

        { // ---> MENU CODE HERE <---

            // setting dummy vars
            static bool dummy_bool = false;
            static float dummy_float = 0.f;
            static int dummy_int = 0;
            static const char* aimBones[]{ "Head", "Chest", "Stomach" };
            static int selectedBone = 0;
            static int selected[3]{};

            // menu vars
            static ImVec2 menuPos;
            static bool showBotBar = false;
            
            // for selecting tabs
            static int selectedTab = 0;

            // main window

            ImGui::SetNextWindowSize(ImVec2(600.f, 500.f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.f, 8.f));
            
            if (ImGui::Begin("CLUES - Counter-Strike: Global Offensive", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
                
                ImGui::PopStyleVar();

                menuPos = ImGui::GetWindowPos(); // grab window position

                ImGui::PushStyleColor(ImGuiCol_Border, colors::border_col);

                if (ImGui::BeginChild("##Tabs", ImVec2(ImGui::GetContentRegionAvail().x, 50.f), true)) {
                    
                    int tabs = 5; // = [aimbot, visuals, misc, skins, configs]
                    float tabSpacing = 2.0f;
                    float tabSizeX = (ImGui::GetContentRegionAvail().x / tabs) - tabSpacing;
                    float tabSizeY = ImGui::GetContentRegionAvail().y;

                    
                    ImGui::PushStyleColor(ImGuiCol_Button, selectedTab == 0 ? colors::buttonActive : colors::buttonInactive);
                    if (ImGui::Button("Aimbot", ImVec2(tabSizeX, tabSizeY))) {
                        selectedTab = 0;
                    }
                    ImGui::SameLine(0.f, tabSpacing);

                    ImGui::PushStyleColor(ImGuiCol_Button, selectedTab == 1 ? colors::buttonActive : colors::buttonInactive);
                    if (ImGui::Button("Visuals", ImVec2(tabSizeX, tabSizeY))) {
                        selectedTab = 1;
                    }
                    ImGui::SameLine(0.f, tabSpacing);

                    ImGui::PushStyleColor(ImGuiCol_Button, selectedTab == 2 ? colors::buttonActive : colors::buttonInactive);
                    if (ImGui::Button("Misc", ImVec2(tabSizeX, tabSizeY))) {
                        selectedTab = 2;
                    }
                    ImGui::SameLine(0.f, tabSpacing);

                    ImGui::PushStyleColor(ImGuiCol_Button, selectedTab == 3 ? colors::buttonActive : colors::buttonInactive);
                    if (ImGui::Button("Skins", ImVec2(tabSizeX, tabSizeY))) {
                        selectedTab = 3;
                    }
                    ImGui::SameLine(0.f, tabSpacing);

                    ImGui::PushStyleColor(ImGuiCol_Button, selectedTab == 4 ? colors::buttonActive : colors::buttonInactive);
                    if (ImGui::Button("Configs", ImVec2(tabSizeX, tabSizeY))) {
                        selectedTab = 4;
                    }

                    ImGui::PopStyleColor(5);

                } ImGui::EndChild(); 

                ImGui::PopStyleColor();
                
                ImGui::PushStyleColor(ImGuiCol_Border, colors::border_col);

                if (ImGui::BeginChild("##ActiveTab", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true)) {
                    switch (selectedTab)
                    {
                    case 0: // aimbot
                        ImGui::Checkbox("Enable Aimbot", &dummy_bool);
                        ImGui::SliderInt("FOV", &dummy_int, 0, 100);
                        ImGui::SliderFloat("Speed", &dummy_float, 0.f, 100.f);

                        if (ImGui::BeginCombo("Bone", " ")) {
                            for (size_t i = 0; i < IM_ARRAYSIZE(aimBones); i++)
                            {
                                ImGui::Selectable(aimBones[i], &selected[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
                            }

                            ImGui::EndCombo();
                        }

                        break;
                    case 1: // visuals
                        ImGui::Columns(2);

                        ImGui::Button("Dummy 1");
                        ImGui::Button("Dummy 2");

                        ImGui::NextColumn();

                        ImGui::Button("Dummy 3");
                        ImGui::Button("Dummy 4");

                        break;
                    case 2: // misc
                        if (ImGui::BeginChild(1, ImVec2(100.f, 100.f))) {
                            ImGui::Checkbox("Enable Bhop", &dummy_bool);
                            ImGui::Button("Dummy 2");

                            ImGui::EndChild();
                        } 

                        if (ImGui::BeginChild(2, ImVec2(100.f, 100.f))) {
                            ImGui::Button("Dummy 3");
                            ImGui::Button("Dummy 4");

                            ImGui::EndChild();
                        } 

                        break;
                    case 3: // skins
                        break;
                    case 4: // configs
                        break;
                    }

                } ImGui::EndChild();
                
                ImGui::PopStyleColor();

            } ImGui::End();

            // show/hide button
            ImGui::SetNextWindowSize(ImVec2(600.f, 50.f));
            ImGui::SetNextWindowPos(ImVec2(menuPos.x, menuPos.y + 480.f));
            if (ImGui::Begin("Show/Hide Button", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground)) {

                ImGui::PushStyleColor(ImGuiCol_Button, colors::hidden);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors::hidden);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors::hidden);

                // make "click" hide on click
                if (showBotBar == true) {
                    ImGui::PushStyleColor(ImGuiCol_Text, colors::hidden);
                } else ImGui::PushStyleColor(ImGuiCol_Text, colors::text_col);

                if (ImGui::Button("click", ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetWindowHeight()))) {
                    showBotBar = !showBotBar;
                }

                ImGui::PopStyleColor(4);
                
            } ImGui::End();

            // bottom-bar
            if (showBotBar) {
                ImGui::SetNextWindowSize(ImVec2(600.f, 58.f));
                ImGui::SetNextWindowPos(ImVec2(menuPos.x, menuPos.y + 505.f));

                if (ImGui::Begin("Version 1 - By MrClue", 0, ImGuiWindowFlags_NoResize /* | ImGuiWindowFlags_NoCollapse */ | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar)) {

                    ImGui::Text("Status: ");
                    ImGui::SameLine(0.f, 0.f);
                    ImGui::TextColored(colors::status_undetected, "Undectected");
                    ImGui::SameLine(0.f, 0.f);
                    ImGui::Text(".");
                    
                } ImGui::End(); 
                
            } 

        }

        ImGui::Render( );
       
        g_pd3dDeviceContext->OMSetRenderTargets( 1 , &g_mainRenderTargetView , NULL );
        g_pd3dDeviceContext->ClearRenderTargetView( g_mainRenderTargetView , clear_color_with_alpha );
       
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

        g_pSwapChain->Present(0, 0); 
    }

    ImGui_ImplDX11_Shutdown( );
    ImGui_ImplWin32_Shutdown( );
    ImGui::DestroyContext( );

    CleanupDeviceD3D( );
    DestroyWindow( hwnd );
    UnregisterClassA( wc.lpszClassName , wc.hInstance );

    return 0;
}


bool CreateDeviceD3D( HWND hWnd )
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd , sizeof( sd ) );
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    if ( D3D11CreateDeviceAndSwapChain( NULL , D3D_DRIVER_TYPE_HARDWARE , NULL , createDeviceFlags , featureLevelArray , 2 , D3D11_SDK_VERSION , &sd , &g_pSwapChain , &g_pd3dDevice , &featureLevel , &g_pd3dDeviceContext ) != S_OK )
        return false;

    CreateRenderTarget( );
    return true;
}

void CleanupDeviceD3D( )
{
    CleanupRenderTarget( );
    if ( g_pSwapChain ) { g_pSwapChain->Release( ); g_pSwapChain = NULL; }
    if ( g_pd3dDeviceContext ) { g_pd3dDeviceContext->Release( ); g_pd3dDeviceContext = NULL; }
    if ( g_pd3dDevice ) { g_pd3dDevice->Release( ); g_pd3dDevice = NULL; }
}

void CreateRenderTarget( )
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer( 0 , IID_PPV_ARGS( &pBackBuffer ) );
    g_pd3dDevice->CreateRenderTargetView( pBackBuffer , NULL , &g_mainRenderTargetView );
    pBackBuffer->Release( );
}

void CleanupRenderTarget( )
{
    if ( g_mainRenderTargetView ) { g_mainRenderTargetView->Release( ); g_mainRenderTargetView = NULL; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );

LRESULT WINAPI WndProc( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd , msg , wParam , lParam ) )
        return true;

    switch ( msg )
    {
    case WM_SIZE:
        if ( g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED )
        {
            CleanupRenderTarget( );
            g_pSwapChain->ResizeBuffers( 0 , (UINT)LOWORD( lParam ) , (UINT)HIWORD( lParam ) , DXGI_FORMAT_UNKNOWN , 0 );
            CreateRenderTarget( );
        }
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
            return 0;
        break;
    case WM_DESTROY:
        ExitProcess( 0 );
        PostQuitMessage( 0 );
        return 0;
    }
    return ::DefWindowProc( hWnd , msg , wParam , lParam );
}
