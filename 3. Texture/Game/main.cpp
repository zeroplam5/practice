#include "Common.h"
#include "Game.h"

INT WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ INT nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
    {   
        return -1;
    }


    if (FAILED(InitDevice()))
    {
        CleanupDevice(); // ← 실패했을 때도 안전하게 정리
        return 0;
    }

    MSG msg = {};
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render(); // GPU에게 그려달라고 매 프레임 요청
        }
    }
    CleanupDevice();

    return static_cast<INT>(msg.wParam);
}