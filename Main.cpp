#include <Windows.h>
#include <iostream>
#include <functional>
#include <string>
#include <conio.h>

#include "LoadingThread.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OutConsoleSet(void);//�R���\�[����\�����A�W���o�͂����̃R���\�[���ɂ���
void LoadImage(const std::string path);//�^���ǂݍ��݊֐�
void LoadSound(const std::string path);//�^���ǂݍ��݊֐�

LoadingThread load_thread;


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
    OutConsoleSet();//�R���\�[���\��

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
    static const UINT kTimerId = 100;//�^�C�}�[��ID
    static const int kTimerInterval = 1000;//�^�C�}�[���s�Ԋu

    switch(uMsg){
    case WM_CREATE:
        //�ǂݍ��݊֐���o�^���Ă���
        load_thread.AddRegister([](){
            LoadImage("���.png");
        });

        load_thread.AddRegister([](){
            LoadImage("�݂���.png");
        });

        load_thread.AddRegister([](){
            LoadSound("BGM.ogg");
        });

        
        SetTimer( hWnd, kTimerId, kTimerInterval, NULL );//�^�C�}���쐬����

        load_thread.RunThread();//�X���b�h�J�n�I
        break;
    case WM_TIMER:
        if(wParam == kTimerId){

            std::cout << "����" << load_thread.GetProgressNum() <<  "%" << std::endl;

            
        }
        break;
    case WM_DESTROY:
        if(load_thread.IsActiveThread()){//�ǂݍ��݃X���b�h���܂��I�����Ă��Ȃ�������
            load_thread.SetEndThreadFlag(true);//�I���v���I
            WaitForSingleObject(load_thread.GetHandle(), INFINITE);//��̓X���b�h�I���҂̂�
        }

        
        KillTimer( hWnd, kTimerId );//�^�C�}��j������
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void OutConsoleSet(void)
{
    FILE *fp = stdout;
    AllocConsole();//�R���\�[���\��
    freopen_s(&fp, "CONOUT$", "w", stdout); //�W���o�͂��R���\�[���ɂ���
}

void LoadImage(const std::string path)
{
    Sleep(2000);//�^���ǂݍ��ݎ��ԉ҂�
    std::cout << "LoadImage��" << path << "��ǂݍ��݂܂���" << std::endl;
}

void LoadSound(const std::string path)
{
    Sleep(2000);//�^���ǂݍ��ݎ��ԉ҂�
    std::cout << "LoadSound��" << path << "��ǂݍ��݂܂���" << std::endl;
}