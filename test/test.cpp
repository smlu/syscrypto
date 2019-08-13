#include <syscrypto/syscrypto.h>
#include <syscrypto/bytes.h>
#include <syscrypto/bytes_span.h>
#include <syscrypto/platform.h>
#include <syscrypto/sec_allocator.h>
#include <syscrypto/skc.h>

#include <assert.h>
#include <array>
#include <cstddef>
#include <iostream>
#include <vector>




int main()
{
    syscrypto::sec_bytes sec_bytes{ std::byte(0x22), std::byte(0x41), std::byte(0x41) };
    auto pd_cdata = syscrypto::pde::encrypt(sec_bytes);
    auto sec_ddata = syscrypto::pde::decrypt(pd_cdata);
    assert(sec_ddata == sec_bytes);

    /*std::array bytes2{ std::byte(0x22), std::byte(0x41), std::byte(0x42) };
    syscrypto::bytes_span bytes_span(sec_bytes);
    bytes_span = bytes_span.subspan(2);

    zeromem(bytes_span);
    std::cout <<"view char: " << int(bytes_span[0]) << std::endl;

    {
        syscrypto::sec_bytes bytes{ std::byte(0x44), std::byte(0x41), std::byte(0x41) };
    }

    syscrypto::sec_fixed_bytes<3> sfb{ { std::byte(0x10), std::byte(0x21), std::byte(0x52) } };
    */
    

    return 0;
}

#ifdef SC_PLATFORM_OS_WINDOWS

#include <syscrypto/impl/windows/win_utils.h>

#include <WinUser.h>

constexpr int ID_BTN_ENCRYPT = 0;
HWND hwndBtnEncrypt;
HWND g_hwnd;
constexpr char g_szClassName[] = "myWindowClass";


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        hwndBtnEncrypt = CreateWindow(TEXT("button"), TEXT("Encrypt"),
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 100, 50,
            hwnd, (HMENU)ID_BTN_ENCRYPT, NULL, NULL);

        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
            case ID_BTN_ENCRYPT:
            {
                auto hwnd = syscrypto::windows::get_top_hwnd();
                int i = 0;

            } break;
        }
    } break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR   pCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    MSG Msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }


    g_hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "SysCrypto Test",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, hInstance, NULL);

    if (g_hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);


    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return (int)Msg.wParam;
}

#endif