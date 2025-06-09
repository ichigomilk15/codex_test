// codex_test.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "pch.h"
#include "framework.h"
#include "codex_test.h"

#include <cmath>
#include <chrono>

#include "DX11Wrapper.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// imgui_impl_win32.h で宣言がコメントアウトされているため、ここで宣言しておく
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名
DX11Wrapper g_dx;
float g_clearColor[4] = {0.4f, 0.6f, 0.9f, 1.0f};
float g_aspectRatio = 1.0f;
std::chrono::steady_clock::time_point g_startTime;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CODEXTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    g_startTime = std::chrono::steady_clock::now();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CODEXTEST));

    MSG msg{};

    // メイン メッセージ ループ:
    while (msg.message != WM_QUIT)
    {
        // Windows のメッセージ処理
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        // メッセージが無い場合は描画処理を実行
        else if (g_dx.IsInitialized())
        {
            // ImGui フレーム開始
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - g_startTime).count();
            float angle = elapsed;

            // デバッグ用ウィンドウ
            ImGui::Begin("Debug");
            ImGui::ColorEdit4("Clear Color", g_clearColor);
            ImGui::Text("Time: %.2f s", elapsed);
            ImGui::Text("Rotation: %.2f rad", angle);
            ImGui::End();
            g_dx.DrawGUI();

            // 描画処理
            ImGui::Render();

            g_dx.Clear(g_clearColor[0], g_clearColor[1], g_clearColor[2], g_clearColor[3]);
            g_dx.Draw();
            g_dx.RenderImGui();
            g_dx.Present();
        }
    }

    return static_cast<int>(msg.wParam);
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CODEXTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CODEXTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    constexpr UINT WINDOW_WIDTH = 500;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_WIDTH/g_aspectRatio, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   // DirectX11 の初期化
    RECT rc{};
    GetClientRect(hWnd, &rc);    // クライアント領域のサイズ取得
    g_aspectRatio = static_cast<float>(rc.right - rc.left) /
                    static_cast<float>(rc.bottom - rc.top);
    // 取得したサイズで DX11 を初期化
    g_dx.Initialize(hWnd, rc.right - rc.left, rc.bottom - rc.top);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_SIZING:
        {
            RECT* rc = reinterpret_cast<RECT*>(lParam);
            int width = rc->right - rc->left;
            int height = rc->bottom - rc->top;
            if (wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM)
            {
                int newWidth = static_cast<int>(height * g_aspectRatio);
                rc->right = rc->left + newWidth;
            }
            else
            {
                int newHeight = static_cast<int>(width / g_aspectRatio);
                if (wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT)
                    rc->top = rc->bottom - newHeight;
                else
                    rc->bottom = rc->top + newHeight;
            }
            return TRUE;
        }
        break;
    case WM_SIZE:
        if (g_dx.IsInitialized() && wParam != SIZE_MINIMIZED)
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            g_dx.Resize(width, height);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
