#include "stdafx.h"

#include <windows.h>
#include <exception>
#include <iostream>
#include <ctime>
#include <fstream>

#include "FreeType.h"
#include "OpenGl.h"

#include "Font.h"
#include "FontRenderer.h"

using namespace FIG;

static FontRenderer* renderer;
std::clock_t start;

void InitFont()
{
    static auto font = Font("/Windows/Fonts/times.ttf");
    renderer = font.CreateRenderer(24);
}

void DrawFont()
{
    if (!start)
        start = std::clock();

    GLint m_viewport[4];
    glGetIntegerv(GL_VIEWPORT, m_viewport);

    float left = (float)m_viewport[0];
    float top = (float)m_viewport[1] - 100.f;
    float right = (float)m_viewport[2];
    float bottom = (float)m_viewport[3] - 100.f;
    float nearVal = -1.0;
    float farVal = 1.0;

    float X = 2 / (right - left),
          Y = 2 / (top - bottom),
          Z = -2 / (farVal - nearVal),
          Tx = -(right + left) / (right - left),
          Ty = -(top + bottom) / (top - bottom),
          Tz = -(farVal + nearVal) / (farVal - nearVal);
    
    float matrix[16] = {
        X, 0, 0, Tx,
        0, Y, 0, Ty,
        0, 0, Z, Tz,
        0, 0, 0, 1
    };

    float fgColor[4] = { 1.0, 0.0, 1.0, 1.0 };

    static int c = 0;
    c++;

    std::clock_t current = std::clock();
    double duration = (current - start) / (double)CLOCKS_PER_SEC;
    std::cout << c / duration << std::endl;

    //renderer->Draw(matrix, fgColor, "Hello, World! %d", c);
}

/////////////////////////////////////////////////////
// After this sets up a sandbox OpenGl Environment //
/////////////////////////////////////////////////////

GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
GLvoid KillGLWindow(GLvoid);
int InitGL(GLvoid);

HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

GLuint	base;				// Base Display List For The Font Set
GLfloat	cnt1;				// 1st Counter Used To Move Text & For Coloring
GLfloat	cnt2;				// 2nd Counter Used To Move Text & For Coloring

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active = TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

BOOL CreateGLWindow(LPCWSTR title, int width, int height, int bits)
{
    GLuint		PixelFormat;			// Holds The Results After Searching For A Match
    WNDCLASS	wc;						// Windows Class Structure
    DWORD		dwExStyle;				// Window Extended Style
    DWORD		dwStyle;				// Window Style
    RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
    WindowRect.left = (long)0;			// Set Left Value To 0
    WindowRect.right = (long)width;		// Set Right Value To Requested Width
    WindowRect.top = (long)0;				// Set Top Value To 0
    WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

    hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
    wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
    wc.cbClsExtra = 0;									// No Extra Window Data
    wc.cbWndExtra = 0;									// No Extra Window Data
    wc.hInstance = hInstance;							// Set The Instance
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
    wc.hbrBackground = NULL;									// No Background Required For GL
    wc.lpszMenuName = NULL;									// We Don't Want A Menu
    wc.lpszClassName = L"OpenGL";								// Set The Class Name

    if (!RegisterClass(&wc))									// Attempt To Register The Window Class
    {
        MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;											// Return FALSE
    }

    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
    dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style

    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

                                                                    // Create The Window
    if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
        L"OpenGL",							// Class Name
        title,								// Window Title
        dwStyle |							// Defined Window Style
        WS_CLIPSIBLINGS |					// Required Window Style
        WS_CLIPCHILDREN,					// Required Window Style
        0, 0,								// Window Position
        WindowRect.right - WindowRect.left,	// Calculate Window Width
        WindowRect.bottom - WindowRect.top,	// Calculate Window Height
        NULL,								// No Parent Window
        NULL,								// No Menu
        hInstance,							// Instance
        NULL)))								// Dont Pass Anything To WM_CREATE
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
        1,											// Version Number
        PFD_DRAW_TO_WINDOW |						// Format Must Support Window
        PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
        PFD_DOUBLEBUFFER,							// Must Support Double Buffering
        PFD_TYPE_RGBA,								// Request An RGBA Format
        bits,										// Select Our Color Depth
        0, 0, 0, 0, 0, 0,							// Color Bits Ignored
        0,											// No Alpha Buffer
        0,											// Shift Bit Ignored
        0,											// No Accumulation Buffer
        0, 0, 0, 0,									// Accumulation Bits Ignored
        16,											// 16Bit Z-Buffer (Depth Buffer)  
        0,											// No Stencil Buffer
        0,											// No Auxiliary Buffer
        PFD_MAIN_PLANE,								// Main Drawing Layer
        0,											// Reserved
        0, 0, 0										// Layer Masks Ignored
    };

    if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    ShowWindow(hWnd, SW_SHOW);						// Show The Window
    SetForegroundWindow(hWnd);						// Slightly Higher Priority
    SetFocus(hWnd);									// Sets Keyboard Focus To The Window
    ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

    if (!InitGL())									// Initialize Our Newly Created GL Window
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, L"Initialization Failed.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    return TRUE;									// Success
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
    if (fullscreen)										// Are We In Fullscreen Mode?
    {
        ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
        ShowCursor(TRUE);								// Show Mouse Pointer
    }

    if (hRC)											// Do We Have A Rendering Context?
    {
        if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
        {
            MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        }

        if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
        {
            MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        }
        hRC = NULL;										// Set RC To NULL
    }

    if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
    {
        MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        hDC = NULL;										// Set DC To NULL
    }

    if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
    {
        MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        hWnd = NULL;										// Set hWnd To NULL
    }

    if (!UnregisterClass(L"OpenGL", hInstance))			// Are We Able To Unregister Class
    {
        MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        hInstance = NULL;									// Set hInstance To NULL
    }

    //KillFont();
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

    glewExperimental = TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "GLEW error: " << glewGetErrorString(err) << std::endl;
        return false;
    }
                                                        // Build The Font
    InitFont();

    return TRUE;										// Initialization Went OK
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
    if (height == 0)										// Prevent A Divide By Zero By
    {
        height = 1;										// Making Height Equal One
    }

    glViewport(0, 0, width, height);						// Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix

                                                        // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glLoadIdentity();									// Reset The Modelview Matrix
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

    DrawFont();

    cnt1 += 0.051f;										// Increase The First Counter
    cnt2 += 0.005f;										// Increase The First Counter
    return TRUE;										// Everything Went OK
}

LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
    UINT	uMsg,			// Message For This Window
    WPARAM	wParam,			// Additional Message Information
    LPARAM	lParam)			// Additional Message Information
{
    switch (uMsg)									// Check For Windows Messages
    {
    case WM_ACTIVATE:							// Watch For Window Activate Message
    {
        if (!HIWORD(wParam))					// Check Minimization State
        {
            active = TRUE;						// Program Is Active
        }
        else
        {
            active = FALSE;						// Program Is No Longer Active
        }

        return 0;								// Return To The Message Loop
    }

    case WM_SYSCOMMAND:							// Intercept System Commands
    {
        switch (wParam)							// Check System Calls
        {
        case SC_SCREENSAVE:					// Screensaver Trying To Start?
        case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
            return 0;							// Prevent From Happening
        }
        break;									// Exit
    }

    case WM_CLOSE:								// Did We Receive A Close Message?
    {
        PostQuitMessage(0);						// Send A Quit Message
        return 0;								// Jump Back
    }

    case WM_KEYDOWN:							// Is A Key Being Held Down?
    {
        keys[wParam] = TRUE;					// If So, Mark It As TRUE
        return 0;								// Jump Back
    }

    case WM_KEYUP:								// Has A Key Been Released?
    {
        keys[wParam] = FALSE;					// If So, Mark It As FALSE
        return 0;								// Jump Back
    }

    case WM_SIZE:								// Resize The OpenGL Window
    {
        ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
        return 0;								// Jump Back
    }
    }

    // Pass All Unhandled Messages To DefWindowProc
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
    HINSTANCE	hPrevInstance,		// Previous Instance
    LPSTR		lpCmdLine,			// Command Line Parameters
    int			nCmdShow)			// Window Show State
{

    MSG		msg;									// Windows Message Structure
    BOOL	done = FALSE;								// Bool Variable To Exit Loop

    //redirect std::cout to file
    std::ofstream file;
    file.open("cout.txt");
    std::streambuf* sbuf = std::cout.rdbuf();
    std::cout.rdbuf(file.rdbuf());

    try {											// Use exception handling 
    
        if (!CreateGLWindow(L"Fonts In Gl!", 640, 480, 16))
        {
            return 0;									// Quit If Window Was Not Created
        }

        while (!done)									// Loop That Runs While done=FALSE
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
            {
                if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
                {
                    done = TRUE;							// If So done=TRUE
                }
                else									// If Not, Deal With Window Messages
                {
                    TranslateMessage(&msg);				// Translate The Message
                    DispatchMessage(&msg);				// Dispatch The Message
                }
            }
            else										// If There Are No Messages
            {
                // Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
                if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
                {
                    done = TRUE;							// ESC or DrawGLScene Signalled A Quit
                }
                else									// Not Time To Quit, Update Screen
                {
                    SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
                }
            }
        }

        // Shutdown
        KillGLWindow();									// Kill The Window

                                                        //catch any exceptions that where thrown
    }
    catch (std::exception &) {
        //    MessageBox(NULL, e.what(), L"CAUGHT AN EXCEPTION", MB_OK | MB_ICONINFORMATION);
    }

    return (msg.wParam);							// Exit The Program
}
