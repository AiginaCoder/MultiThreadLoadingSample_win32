#include <Windows.h>
#include <iostream>
#include <functional>
#include <string>
#include <conio.h>

#include "LoadingThread.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OutConsoleSet(void);//コンソールを表示し、標準出力をそのコンソールにする
void LoadImage(const std::string path);//疑似読み込み関数
void LoadSound(const std::string path);//疑似読み込み関数

LoadingThread load_thread;


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
    OutConsoleSet();//コンソール表示

    MSG msg;
    WNDCLASS wc;
    HWND hWnd;

    wc.style		= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc	= WndProc;
    wc.cbClsExtra	= 0;
    wc.cbWndExtra	= 0;
    wc.hInstance	= hInst;
    wc.hIcon		= NULL;
    wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground= (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName	= (LPSTR)NULL;
    wc.lpszClassName= "Test";
    RegisterClass(&wc);

    hWnd = CreateWindow("Test", "TITLE", WS_OVERLAPPEDWINDOW, 10, 50, 800, 600, NULL, NULL, hInst, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    
    
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static const UINT kTimerId = 100;//タイマーのID
    static const int kTimerInterval = 1000;//タイマー実行間隔

    switch(uMsg){
    case WM_CREATE:
        //読み込み関数を登録していく
        load_thread.AddRegister([](){
            LoadImage("りんご.png");
        });

        load_thread.AddRegister([](){
            LoadImage("みかん.png");
        });

        load_thread.AddRegister([](){
            LoadSound("BGM.ogg");
        });

        
        SetTimer( hWnd, kTimerId, kTimerInterval, NULL );//タイマを作成する

        load_thread.RunThread();//スレッド開始！
        break;
    case WM_TIMER:
        if(wParam == kTimerId){

            std::cout << "現在" << load_thread.GetProgressNum() <<  "%" << std::endl;

            
        }
        break;
    case WM_DESTROY:
        if(load_thread.IsActiveThread()){//読み込みスレッドがまだ終了していなかったら
            load_thread.SetEndThreadFlag(true);//終了要請！
            WaitForSingleObject(load_thread.GetHandle(), INFINITE);//後はスレッド終了待つのみ
        }

        
        KillTimer( hWnd, kTimerId );//タイマを破棄する
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void OutConsoleSet(void)
{
    FILE *fp = stdout;
    AllocConsole();//コンソール表示
    freopen_s(&fp, "CONOUT$", "w", stdout); //標準出力をコンソールにする
}

void LoadImage(const std::string path)
{
    Sleep(2000);//疑似読み込み時間稼ぎ
    std::cout << "LoadImageで" << path << "を読み込みました" << std::endl;
}

void LoadSound(const std::string path)
{
    Sleep(2000);//疑似読み込み時間稼ぎ
    std::cout << "LoadSoundで" << path << "を読み込みました" << std::endl;
}