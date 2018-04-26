#ifdef WIN32
#include <Windows.h>
#include <windowsx.h>
#include <assert.h>
#include <stdio.h>
struct console
{
    console( unsigned width, unsigned height )
    {
        SMALL_RECT r;
        COORD      c;
        hConOut = GetStdHandle( STD_OUTPUT_HANDLE );
        if (!GetConsoleScreenBufferInfo( hConOut, &csbi ))
        {
            assert(!"You must be attached to a human." );
        }
        
        r.Left   =
        r.Top    = 0;
        r.Right  = width -1;
        r.Bottom = height -1;
        SetConsoleWindowInfo( hConOut, TRUE, &r );
        
        c.X = width;
        c.Y = height;
        SetConsoleScreenBufferSize( hConOut, c );
    }
    
    ~console()
    {
        SetConsoleTextAttribute(    hConOut,        csbi.wAttributes );
        SetConsoleScreenBufferSize( hConOut,        csbi.dwSize      );
        SetConsoleWindowInfo(       hConOut, TRUE, &csbi.srWindow    );
    }
    
    void color( WORD color = 0x07 )
    {
        SetConsoleTextAttribute( hConOut, color );
    }
    
    HANDLE                     hConOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
};
#elif defined(ANDROID)
#include <unistd.h>
#include <android/log.h>
#include <string>

// Route stdout to logcat D/
static int pfd[2];
static pthread_t thr;
static const char *tag = "myapp";

static void *thread_func(void*)
{
    ssize_t rdsz;
    char buf[128];
    while((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0) {
        if(buf[rdsz - 1] == '\n') --rdsz;
        buf[rdsz] = 0;  /* add null-terminator */
        __android_log_write(ANDROID_LOG_DEBUG, tag, buf);
    }
    return 0;
}
#endif

void TSRedirectStandardOutput()
{
#ifdef WIN32
    AllocConsole();
    SetConsoleTitleA("Debug Output");
    freopen("CONOUT$", "w+", stdout);
    console con( 80, 10000 );
    
    HWND consoleWindowHandle = GetConsoleWindow();
    MoveWindow(consoleWindowHandle, 0, 800, 1280, 180, false);
    ShowWindow(consoleWindowHandle, SW_MINIMIZE);
#elif defined(ANDROID)
    tag = "Debug Output";
    
    /* make stdout line-buffered and stderr unbuffered */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);
    
    /* create the pipe and redirect stdout and stderr */
    pipe(pfd);
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);
    
    /* spawn the logging thread */
    if(pthread_create(&thr, 0, thread_func, 0) == -1)
        return;
    pthread_detach(thr);
#endif
}
