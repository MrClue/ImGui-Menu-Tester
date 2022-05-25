#include "menu.h"
#include "settings.h"
#include <vector>

static ID3D11Device           * g_pd3dDevice           = nullptr;
static ID3D11DeviceContext    * g_pd3dDeviceContext    = nullptr;
static IDXGISwapChain         * g_pSwapChain           = nullptr;
static ID3D11RenderTargetView * g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D( HWND );
void CleanupDeviceD3D( );
void CreateRenderTarget( );
void CleanupRenderTarget( );
LRESULT __stdcall WndProc( HWND, UINT , WPARAM, LPARAM );

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
    
    menu::themes::MenuTheme(); // APPLY STYLING

    ImGuiIO& io = ImGui::GetIO( ); 

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    io.WantSaveIniSettings = false;

    //menu::themes::MenuTheme(); // APPLY STYLING

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

        menu::render::RenderMenu(); // RENDER MENU

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

//
// SECTION: MENU IMPLEMENTATION STUFF!
//

void menu::widgets::MainMenu() {
    ImGui::PushFont(menu::fonts::titleFont); // apply title font

    ImGui::SetNextWindowSize(menu::sizes::main_menu);
    //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.f, 8.f));
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 0.f));
    if (ImGui::Begin("CLUES - Counter-Strike: Global Offensive", 0, menu::flags::main_menu)) {
        //ImGui::PopStyleVar(2);
        
        menu::main_menu_position = ImGui::GetWindowPos(); // grab window position
        
        ImGui::PushFont(menu::fonts::defaultFont); // apply default font

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
        //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
        menu::navbar::TabNavigation();
        ImGui::PopStyleVar();

        menu::navbar::TabContent();

        ImGui::PopFont(); // default font

    } ImGui::End();
    
    ImGui::PopFont(); // title font
}

void menu::widgets::ToggleBottomBar() {
    ImGui::PushFont(menu::fonts::defaultFont);

    ImGui::SetNextWindowSize(menu::sizes::click_to_show);
    ImGui::SetNextWindowPos(ImVec2(menu::main_menu_position.x, menu::main_menu_position.y + 480.f));

    if (ImGui::Begin("Show/Hide Button", 0, menu::flags::click_to_show)) {

        ImGui::PushStyleColor(ImGuiCol_Button, colors::hidden);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors::hidden);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors::hidden);

        // make "click" hide on click
        if (menu::showBotBar == true) {
            ImGui::PushStyleColor(ImGuiCol_Text, colors::hidden);
        }
        else ImGui::PushStyleColor(ImGuiCol_Text, colors::text);

        if (ImGui::Button("click", ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetWindowHeight()))) {
            menu::showBotBar = !menu::showBotBar;
        }

        ImGui::PopStyleColor(4); // click-btn styles

    } ImGui::End();

    ImGui::PopFont();
}

void menu::widgets::InitBottomBar() {
    ImGui::PushFont(menu::fonts::defaultFont);

    ImGui::SetNextWindowSize(menu::sizes::bottom_bar);
    ImGui::SetNextWindowPos(ImVec2(menu::main_menu_position.x, menu::main_menu_position.y + 505.f));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5, 0.5));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, colors::dark_purple);
    if (ImGui::Begin("Version 1 - By MrClue", 0, menu::flags::bottom_bar)) {

        ImGui::Text("Status: ");
        ImGui::SameLine(0.f, 0.f);
        if (!menu::isDetected) {
            ImGui::TextColored(colors::status_undetected, "Undectected");
        } else ImGui::TextColored(colors::statis_detected, "Detected");
        ImGui::SameLine(0.f, 0.f);
        ImGui::Text(".");

    } ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGui::PopFont();
}

void menu::navbar::TabNavigation() {
    ImGui::PushStyleColor(ImGuiCol_Border, colors::border);
    if (ImGui::BeginChild("##Tabs", ImVec2(ImGui::GetContentRegionAvail().x, 50.f), true)) {

        float tabSizeX = (ImGui::GetContentRegionAvail().x / menu::tabs) - menu::tabSpacing;
        float tabSizeY = ImGui::GetContentRegionAvail().y;

        ImGui::PushStyleColor(ImGuiCol_Button, menu::selectedTab == 0 ? colors::button_active : colors::button_inactive);
        ImGui::PushStyleColor(ImGuiCol_Text, menu::selectedTab == 0 ? colors::text : colors::text_inactive);
        if (ImGui::Button("Aimbot", ImVec2(tabSizeX, tabSizeY))) {
            menu::selectedTab = 0;
        }

        ImGui::SameLine(0.f, menu::tabSpacing);

        ImGui::PushStyleColor(ImGuiCol_Button, menu::selectedTab == 1 ? colors::button_active : colors::button_inactive);
        ImGui::PushStyleColor(ImGuiCol_Text, menu::selectedTab == 1 ? colors::text : colors::text_inactive);
        if (ImGui::Button("Visuals", ImVec2(tabSizeX, tabSizeY))) {
            menu::selectedTab = 1;
        }

        ImGui::SameLine(0.f, menu::tabSpacing);

        ImGui::PushStyleColor(ImGuiCol_Button, menu::selectedTab == 2 ? colors::button_active : colors::button_inactive);
        ImGui::PushStyleColor(ImGuiCol_Text, menu::selectedTab == 2 ? colors::text : colors::text_inactive);
        if (ImGui::Button("Misc", ImVec2(tabSizeX, tabSizeY))) {
            menu::selectedTab = 2;
        }

        ImGui::SameLine(0.f, menu::tabSpacing);

        ImGui::PushStyleColor(ImGuiCol_Button, menu::selectedTab == 3 ? colors::button_active : colors::button_inactive);
        ImGui::PushStyleColor(ImGuiCol_Text, menu::selectedTab == 3 ? colors::text : colors::text_inactive);
        if (ImGui::Button("Skins", ImVec2(tabSizeX, tabSizeY))) {
            menu::selectedTab = 3;
        }

        ImGui::SameLine(0.f, menu::tabSpacing);

        ImGui::PushStyleColor(ImGuiCol_Button, menu::selectedTab == 4 ? colors::button_active : colors::button_inactive);
        ImGui::PushStyleColor(ImGuiCol_Text, menu::selectedTab == 4 ? colors::text : colors::text_inactive);
        if (ImGui::Button("Configs", ImVec2(tabSizeX, tabSizeY))) {
            menu::selectedTab = 4;
        }

        ImGui::PopStyleColor(10); // for each tab

    } ImGui::EndChild();
    ImGui::PopStyleColor(); // for child: ##tabs
}

void menu::navbar::TabContent() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(8.0f, 4.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, colors::border);
    if (ImGui::BeginChild("##ActiveTab", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true)) {
        switch (menu::selectedTab)
        {
        case 0:
            menu::init_tab::AimbotTab();
            break;
        case 1:
            menu::init_tab::VisualsTab();
            break;
        case 2:
            menu::init_tab::MiscTab();
            break;
        case 3:
            menu::init_tab::SkinsTab();
            break;
        case 4:
            menu::init_tab::ConfigsTab();
            break;
        }

    } ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void menu::init_tab::AimbotTab() {
 
    ImGui::BeginGroupPanel("settings", ImVec2(-1.f, -1.f));

    ImGui::Checkbox("Enable Aimbot", &settings::aimbot::enable_aimbot);
    ImGui::SliderFloat("FOV", &settings::aimbot::fov, 0.f, 100.f, "%.1f");
    ImGui::SliderFloat("Speed", &settings::aimbot::speed, 0.f, 100.f, "%.1f");

    if (ImGui::BeginCombo("Bones", " ")) {
        for (size_t i = 0; i < IM_ARRAYSIZE(settings::aimbot::bones); i++)
        {
            ImGui::Selectable(settings::aimbot::bones[i], &settings::aimbot::selected_array[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
        }

        ImGui::EndCombo();
    }

    ImGui::EndGroupPanel(); // settings

    ImGui::BeginGroupPanel("extra");

    ImGui::Checkbox("Target In Air", &settings::aimbot::misc::target_in_air);
    ImGui::Checkbox("Target In Smoke", &settings::aimbot::misc::target_in_smoke);
    ImGui::Checkbox("Auto Pistol", &settings::aimbot::misc::auto_pistol);

    ImGui::EndGroupPanel(); // extra


}

void menu::init_tab::VisualsTab() {

    /* TODO
    * Fix with for each side -> find out seperator + each side spacing width -> contentX = (GetContentRegionAvail().x - seperatorTotalX) / 2
    */

    auto contentX = (ImGui::GetContentRegionAvail().x - 18.f) / 2.f; // almost perfect lmao

    ImGui::PushStyleColor(ImGuiCol_ChildBg, colors::light_purple);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    if (ImGui::BeginChild("PREVIEW_TAB", ImVec2(contentX, ImGui::GetContentRegionAvail().y), true)) {
       
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
        if (ImGui::BeginChild("PREV_TAB_BAR", ImVec2(ImGui::GetContentRegionAvail().x, 30.f), true)) {
            float tabSizeX = (ImGui::GetContentRegionAvail().x / 3.f);
            float tabSizeY = ImGui::GetContentRegionAvail().y;

            ImGui::PushStyleColor(ImGuiCol_Button, menu::selected_prev_tab == 0 ? colors::button_active : colors::button_inactive);
            ImGui::PushStyleColor(ImGuiCol_Text, menu::selected_prev_tab == 0 ? colors::text : colors::text_inactive);
            if (ImGui::Button("Enemy", ImVec2(tabSizeX, 30.f))) {
                menu::selected_prev_tab = 0;
            }
            ImGui::PopStyleColor(2);

            ImGui::SameLine(0.f, 0.f);
            
            ImGui::PushStyleColor(ImGuiCol_Button, menu::selected_prev_tab == 1 ? colors::button_active : colors::button_inactive);
            ImGui::PushStyleColor(ImGuiCol_Text, menu::selected_prev_tab == 1 ? colors::text : colors::text_inactive);
            if (ImGui::Button("Friendly", ImVec2(tabSizeX, 30.f))) {    
                menu::selected_prev_tab = 1;
            }
            ImGui::PopStyleColor(2);

            ImGui::SameLine(0.f, 0.f);

            ImGui::PushStyleColor(ImGuiCol_Button, menu::selected_prev_tab == 2 ? colors::button_active : colors::button_inactive);
            ImGui::PushStyleColor(ImGuiCol_Text, menu::selected_prev_tab == 2 ? colors::text : colors::text_inactive);
            if (ImGui::Button("Local", ImVec2(tabSizeX, 30.f))) {
                menu::selected_prev_tab = 2;
            }
            ImGui::PopStyleColor(2);

        } ImGui::EndChild();
        //ImGui::PopStyleVar();
        
        // restore padding & render the rest
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));

        // esp preview logic

        std::vector<std::pair<std::string, ImColor>> flags_vector;
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImColor c = ImColor(32, 114, 247);
        const auto cur_window = ImGui::GetCurrentWindow();
        const ImVec2 w_pos = cur_window->Pos;
        auto position = 51;

        int box_w = 210;
        int box_h = 320;

        auto pY = 25; // extra y padding

        //box
        if (settings::visuals::esp::box_esp)
            cur_window->DrawList->AddRect(ImVec2(w_pos.x + 72, w_pos.y + 55+pY), ImVec2(w_pos.x + box_w, w_pos.y + box_h), ImGui::GetColorU32(ImGuiCol_Text));

        //hp
        if (settings::visuals::esp::health_esp)
            cur_window->DrawList->AddRectFilled(ImVec2(w_pos.x + 69, w_pos.y + 55 + pY), ImVec2(w_pos.x + 71, w_pos.y + box_h), ImGui::GetColorU32(ImVec4(83 / 255.f, 200 / 255.f, 84 / 255.f, 255 / 255.f)));

        //armor
        if (settings::visuals::esp::armor_esp)
            cur_window->DrawList->AddRectFilled(ImVec2(w_pos.x + 72, w_pos.y + box_h + 3), ImVec2(w_pos.x + 210, w_pos.y + box_h + 1), ImGui::GetColorU32(ImVec4(0 / 255.f, 80 / 255.f, 255 / 255.f, 255 / 255.f)));

        //name
        if (settings::visuals::esp::name_esp)
            cur_window->DrawList->AddText(ImVec2(w_pos.x + 130, w_pos.y + 39 + pY), ImGui::GetColorU32(ImGuiCol_Text), ("name"));

        //weapon
        if (settings::visuals::esp::weapon_esp)
            cur_window->DrawList->AddText(ImVec2(w_pos.x + 123/*125*/, w_pos.y + box_h + 1), ImGui::GetColorU32(ImGuiCol_Text), ("weapon"));

        if (settings::visuals::esp::esp_flags[0])
            flags_vector.push_back(std::pair<std::string, ImColor>(("bot"), ImColor(255, 0, 0)));

        if (settings::visuals::esp::esp_flags[1])
            flags_vector.push_back(std::pair<std::string, ImColor>(("hk"), ImColor(255, 255, 255)));

        if (settings::visuals::esp::esp_flags[2])
            flags_vector.push_back(std::pair<std::string, ImColor>(std::string(("$16000")), ImColor(120, 180, 10)));

        if (settings::visuals::esp::esp_flags[3])
            flags_vector.push_back(std::pair<std::string, ImColor>(std::string(("zoom")), ImColor(80, 160, 200, 255)));

        if (settings::visuals::esp::esp_flags[4])
            flags_vector.push_back(std::pair<std::string, ImColor>(std::string(("flashed")), ImColor(255, 255, 255)));

        if (settings::visuals::esp::esp_flags[5])
            flags_vector.push_back(std::pair<std::string, ImColor>(std::string(("desync")), ImColor(255, 0, 0)));

        if (settings::visuals::esp::esp_flags[6])
            flags_vector.push_back(std::pair<std::string, ImColor>(std::string(("defusing")), ImColor(247, 202, 24)));

        for (auto text : flags_vector) {
            cur_window->DrawList->AddText(ImVec2(w_pos.x + box_w + 2, w_pos.y + position + pY), ImColor(text.second), text.first.c_str());
            position += 10;
        }

        ImGui::PopStyleVar();

    } ImGui::EndChild();
    ImGui::PopStyleVar();

    // seperator
    {
        ImGui::SameLine(0.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine(0.f);
    }

    custom::BeginFeatureTab("ESP / GLOW / CHAMS", ImVec2(ImGui::GetContentRegionAvail().x, 30.f));

    if (ImGui::BeginChild("ESP_TAB", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true)) {

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, 4.f));

        static const char* flags[7] = { "bot", "armor", "money", "scoped", "flashed", "desync", "defusing" };
        static std::string flPreviewVal = "";
        
        // ESP
        {
            ImGui::Text("ESP");
            ImGui::Separator();
            ImGui::Spacing();
        }
        
        ImGui::Checkbox("Enable ESP", &settings::visuals::esp::enable_esp);
        if (settings::visuals::esp::enable_esp) {
            ImGui::SetCursorPosX(20.f);
            ImGui::Checkbox("Name", &settings::visuals::esp::name_esp);
            ImGui::SetCursorPosX(20.f);
            ImGui::Checkbox("Health", &settings::visuals::esp::health_esp);
            ImGui::SetCursorPosX(20.f);
            ImGui::Checkbox("Armor", &settings::visuals::esp::armor_esp);
            ImGui::SetCursorPosX(20.f);
            ImGui::Checkbox("Weapon", &settings::visuals::esp::weapon_esp);
            ImGui::SetCursorPosX(20.f);
            ImGui::Checkbox("Box", &settings::visuals::esp::box_esp);
            ImGui::SetCursorPosX(20.f);
            
            // todo: lav combo om til simpel funktion
            if (ImGui::BeginCombo("flags", flPreviewVal.c_str())) {
                flPreviewVal = "";
                std::vector<std::string> vec;

                for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
                {
                    ImGui::Selectable(flags[i], &settings::visuals::esp::esp_flags[i], ImGuiSelectableFlags_DontClosePopups);

                    if (settings::visuals::esp::esp_flags[i]) {
                        vec.push_back(flags[i]);
                    }
                    
                }

                for (size_t i = 0; i < vec.size(); i++)
                {
                    if (vec.size() == 1)
                        flPreviewVal += vec.at(i);
                    else if (!(i == vec.size() - 1))
                        flPreviewVal += vec.at(i) + ", ";
                    else
                        flPreviewVal += vec.at(i);
                }

                ImGui::EndCombo();
            }
            
        }
        
        // GLOW
        {
            ImGui::Text("GLOW");
            ImGui::Separator();
            ImGui::Spacing();
        }

        ImGui::Checkbox("Enable Glow", &settings::visuals::glow::enable_glow);
        if (settings::visuals::glow::enable_glow) {
            
            for (size_t i = 0; i < 5; i++)
            {
                ImGui::Text("...");
            }

        }

        // CHAMS
        {
            ImGui::Text("CHAMS");
            ImGui::Separator();
            ImGui::Spacing();
        }

        ImGui::Checkbox("Enable Chams", &settings::visuals::chams::enable_chams);
        if (settings::visuals::chams::enable_chams) {
            for (size_t i = 0; i < 5; i++)
            {
                ImGui::Text("...");
            }
        }
        
        ImGui::PopStyleVar(); //ImGuiStyleVar_ItemSpacing

    } ImGui::EndChild();

    custom::EndFeatureTab();
   
    ImGui::PopStyleColor();
}

void menu::init_tab::MiscTab() {
    
    custom::BeginFeatureTab("Tab 1", ImVec2(ImGui::GetContentRegionAvail().x / 2, 30.f));
    if (ImGui::BeginChild("#tab1", ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y), true)) {
        
        for (size_t i = 0; i < 20; i++)
        {
            ImGui::Text("Hello, Word!");
        }

        ImGui::EndChild();
    }
    custom::EndFeatureTab();

    {
        ImGui::SameLine(0);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine(0);
    }
    
    custom::BeginFeatureTab("Tab 2", ImVec2(ImGui::GetContentRegionAvail().x, 30.f));
    if (ImGui::BeginChild("#tab2", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2), true)) {

        for (size_t i = 0; i < 20; i++)
        {
            ImGui::Text("Hello, Word!");
        }

        ImGui::EndChild();
    }
    custom::EndFeatureTab();
    
}

void menu::init_tab::SkinsTab() {

    if (ImGui::BeginChild("Test", ImVec2(100.f, 100.f))) {
        ImGui::Checkbox("Enable Skins", &settings::skins::enable_skinchanger);
        ImGui::Button("Dummy");
        
    }ImGui::EndChild();
    
    

}

void menu::init_tab::ConfigsTab() {

}

//
// SECTION: STYLING & RENDERING MENU!
//
void menu::themes::MenuFonts() {
    ImGuiIO& io = ImGui::GetIO();

    menu::fonts::defaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\LeelaUIb.ttf", 15.0f);
    menu::fonts::titleFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\LeelaUIb.ttf", 18.0f);

}

void menu::themes::MenuTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    auto colors = style.Colors;

    // title bar
    style.WindowTitleAlign = ImVec2(0.5, 1.0); // mid-bot
    colors[ImGuiCol_TitleBg] = colors::light_purple;
    colors[ImGuiCol_TitleBgActive] = colors::light_purple;
    colors[ImGuiCol_TitleBgCollapsed] = colors::light_purple_reduced_alpha;

    // border
    style.WindowBorderSize = 1.0f;
    colors[ImGuiCol_Border] = colors::light_purple;

    // window backgrounds
    colors[ImGuiCol_WindowBg] = colors::light_purple;
    colors[ImGuiCol_ChildBg] = colors::dark_purple;

    // text
    colors[ImGuiCol_Text] = colors::text;

    // buttons
    colors[ImGuiCol_Button] = colors::button_inactive;
    colors[ImGuiCol_ButtonActive] = colors::button_active;
    colors[ImGuiCol_ButtonHovered] = colors::button_hovered;

    // check-mark
    colors[ImGuiCol_CheckMark] = ImVec4(0.85f, 0.33f, 0.37f, 0.73f);

    // header
    colors[ImGuiCol_Header] = ImColor(218, 83, 95, 27);
    colors[ImGuiCol_HeaderHovered] = ImColor(218, 83, 95, 62);
    colors[ImGuiCol_HeaderActive] = ImColor(218, 83, 95, 143);

    // popup
    //colors[ImGuiCol_PopupBg] = ImColor(218, 83, 95, 27);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.08f, 0.18f, 1.00f);

    // frame
    colors[ImGuiCol_FrameBg] = ImColor(218, 83, 95, 27);
    colors[ImGuiCol_FrameBgHovered] = ImColor(218, 83, 95, 62);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.33f, 0.37f, 0.24f);
    style.FrameRounding = 2.f;
    style.FramePadding = ImVec2(4.f, 4.f);

    // sliders
    style.GrabMinSize = 10.f;
    style.GrabRounding = 2.f;
    colors[ImGuiCol_SliderGrab] = ImVec4(0.85f, 0.33f, 0.37f, 0.59f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.85f, 0.33f, 0.37f, 0.73f);

    // scrollbar
    style.ScrollbarSize = 9.f;
    style.ScrollbarRounding = 12.f;
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.21f, 0.34f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.21f, 0.34f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.21f, 0.34f, 1.00f);

    // seperator
    //colors[ImGuiCol_Separator] = ImVec4(0.85f, 0.33f, 0.37f, 0.73f);
    colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.21f, 0.34f, 1.00f);
    
    // tabs
    style.TabBorderSize = 1.f;
    style.TabRounding = 2.f;
    colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.08f, 0.18f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.16f, 0.12f, 0.22f, 1.00f);
    
    // load fonts
    menu::themes::MenuFonts();
}

void menu::render::RenderMenu() {
    // ImGui demo window
    //ImGui::ShowDemoWindow();
    ImGui::ShowStyleEditor();

    // window: main menu
    menu::widgets::MainMenu();

    // window: toggle bottom-bar
    menu::widgets::ToggleBottomBar();

    // window: bottom-bar
    if (menu::showBotBar) {
        menu::widgets::InitBottomBar();
    }
}

void custom::BeginFeatureTab(const char* label, const ImVec2& size_arg) {
    /* USAGE GUIDE
    * 1st:  BeginFeatureTab()
    * 2nd:  BeginChild()
    * 3rd:  EndFeatureTab();
    */
    ImGui::BeginGroup();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, -0.1f));

    ImGui::PushStyleColor(ImGuiCol_Button, colors::button_inactive);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors::button_inactive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors::button_inactive);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);

    ImGui::Button(label, size_arg);

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, colors::light_purple);
}

void custom::EndFeatureTab() {
    ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing
    ImGui::PopStyleColor();

    ImGui::EndGroup();
}
