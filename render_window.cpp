#include "window_container.h"
#include "render_window.h"


bool RenderWindow::Initialize(WindowContainer * pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
    this->hInstance_ = hInstance;
    this->width_ = width;
    this->height_ = height;
    this->window_title_ = std::move(window_title);
    this->window_title_wide_ = StringConverter::StringToWide(this->window_title_);
    this->window_class_ = std::move(window_class);
    this->window_class_wide_ = StringConverter::StringToWide(this->window_class_); //wide string representation of class string (used for registering class and creating window)

    this->RegisterWindowClass();

    this->handle_ = CreateWindowEx(0, //Extended Windows style - we are using the default. For other options, see: https://msdn.microsoft.com/en-us/library/windows/desktop/ff700543(v=vs.85).aspx
                                  this->window_class_wide_.c_str(), //Window class name
                                  this->window_title_wide_.c_str(), //Window Title
                                  WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, //Windows style - See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632600(v=vs.85).aspx
                                  0, //Window X Position
                                  0, //Window Y Position
                                  this->width_, //Window Width
                                  this->height_, //Window Height
                                  nullptr, //Handle to parent of this window. Since this is the first window, it has no parent window.
                                  nullptr, //Handle to menu or child window identifier. Can be set to NULL and use menu in WindowClassEx if a menu is desired to be used.
                                  this->hInstance_, //Handle to the instance of module to be used with this window
                                  pWindowContainer); //Param to create window

    if (this->handle_ == nullptr)
    {
        ErrorLogger::Log(GetLastError(), "CreateWindowEX Failed for window: " + this->window_title_);
        return false;
    }

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(this->handle_, SW_SHOW);
    SetForegroundWindow(this->handle_);
    SetFocus(this->handle_);

    return true;
}

bool RenderWindow::ProcessMessages()
{
    // Handle the windows messages.
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG)); // Initialize the message structure.

    while (PeekMessage(&msg, //Where to store message (if one exists) See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644943(v=vs.85).aspx
                    this->handle_, //Handle to window we are checking messages for
                    0,    //Minimum Filter Msg Value - We are not filtering for specific messages, but the min/max could be used to filter only mouse messages for example.
                    0,    //Maximum Filter Msg Value
                    PM_REMOVE))//Remove message after capturing it via PeekMessage. For more argument options, see: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644943(v=vs.85).aspx
    {
        TranslateMessage(&msg); //Translate message from virtual key messages into character messages so we can dispatch the message. See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644955(v=vs.85).aspx
        DispatchMessage(&msg); //Dispatch message to our Window Proc for this window. See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644934(v=vs.85).aspx
    }

    // Check if the window was closed
    if (msg.message == WM_NULL)
    {
        if (!IsWindow(this->handle_))
        {
            this->handle_ = nullptr; //Message processing loop takes care of destroying this window
            UnregisterClass(this->window_class_wide_.c_str(), this->hInstance_);
            return false;
        }
    }
    return true;
}

RenderWindow::~RenderWindow()
{
    if (this->handle_ != nullptr)
    {
        UnregisterClass(this->window_class_wide_.c_str(), this->hInstance_);
        DestroyWindow(handle_);
    }
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        // all other messages
        case WM_CLOSE: {
            DestroyWindow(hwnd);
            return 0;
        }
        default: {
            // retrieve ptr to window class
            auto *const pWindow = reinterpret_cast<WindowContainer *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            // forward message to window class handler
            return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_NCCREATE:
        {
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            auto * pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
            if (pWindow == nullptr) //Sanity check
            {
                ErrorLogger::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
                exit(-1);
            }
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
            return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void RenderWindow::RegisterWindowClass()
{
    WNDCLASSEX wc; //Our Window Class (This has to be filled before our window can be created) See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633577(v=vs.85).aspx
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //Flags [Redraw on width/height change from resize/movement] See: https://msdn.microsoft.com/en-us/library/windows/desktop/ff729176(v=vs.85).aspx
    wc.lpfnWndProc = HandleMessageSetup; //Pointer to Window Proc function for handling messages from this window
    wc.cbClsExtra = 0; //# of extra bytes to allocate following the window-class structure. We are not currently using this.
    wc.cbWndExtra = 0; //# of extra bytes to allocate following the window instance. We are not currently using this.
    wc.hInstance = this->hInstance_; //Handle to the instance that contains the Window Procedure
    wc.hIcon = nullptr;   //Handle to the class icon. Must be a handle to an icon resource. We are not currently assigning an icon, so this is null.
    wc.hIconSm = nullptr; //Handle to small icon for this class. We are not currently assigning an icon, so this is null.
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); //Default Cursor - If we leave this null, we have to explicitly set the cursor's shape each time it enters the window.
    wc.hbrBackground = nullptr; //Handle to the class background brush for the window's background color - we will leave this blank for now and later set this to black. For stock brushes, see: https://msdn.microsoft.com/en-us/library/windows/desktop/dd144925(v=vs.85).aspx
    wc.lpszMenuName = nullptr; //Pointer to a null terminated character string for the menu. We are not using a menu yet, so this will be NULL.
    wc.lpszClassName = this->window_class_wide_.c_str(); //Pointer to null terminated string of our class name for this window.
    wc.cbSize = sizeof(WNDCLASSEX); //Need to fill in the size of our struct for cbSize
    RegisterClassEx(&wc); // Register the class so that it is usable.
}

HWND RenderWindow::GetHWND() const {
    return this->handle_;
}
