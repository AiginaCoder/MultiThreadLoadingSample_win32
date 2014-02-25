#include "LoadingThread.h"

//static実体
const char* LoadingThread::kProgressNumMutexName = "ProgressNumMutex";
const char* LoadingThread::kEndThreadFlagMutexName = "EndThreadFlagMutex";
const char* LoadingThread::kThreadHandleName = "ThreadHandle";


LoadingThread::LoadingThread()
{
    progress_num_mutex_ = CreateMutex(NULL, FALSE, kProgressNumMutexName);//progress_num_の排他用Mutexの作成
    end_thread_flag_mutex_ = CreateMutex(NULL, FALSE, kEndThreadFlagMutexName);//end_thread_flag_mutex_の排他用Mutexの作成

    progress_num_ = 0;//進捗具合
    end_thread_flag_ = false;//スレッドを中断させるフラグ（trueなら中断）

    //スレッドの作成
    thread_handle_ = reinterpret_cast<HANDLE>(//戻り値をHANDLE型にキャストしてやる必要がある
        _beginthreadex(NULL, 0,
        LoadingThread::ExecThreadFunc,//実行するスレッド関数
        this,//スレッド関数に渡す引き数、型はvoid*に暗黙的にキャストされます（使わないならNULLでおｋ）
        CREATE_SUSPENDED,//スレッド作成に関する制御フラグを指定します(0で即実行)
        NULL)//スレッドを識別するIDが入る
        );
}

LoadingThread::~LoadingThread(void)
{
    CloseHandle(progress_num_mutex_);//排他用Mutexの削除
    CloseHandle(end_thread_flag_mutex_);//排他用Mutexの削除
    CloseHandle(thread_handle_);//HANDLE削除
}

void LoadingThread::AddRegister(std::function<void()> func)
{
    load_funcs_.push_back(func);//追加登録！
}

void LoadingThread::RunThread()
{
    ResumeThread(thread_handle_);//スレッドの起動
}

void LoadingThread::ExecAllFuncs()
{
    const unsigned int kFuncMaxNum = load_funcs_.size();//登録した読み込み関数の数を取得
    unsigned int func_count = 0;//実行完了した読み込み関数の数を数える

    //イテレータを用いて登録された読み込み関数を１つずつ最初から最後まで実行していく
    for(std::vector<std::function<void()>>::iterator it = load_funcs_.begin();
        it != load_funcs_.end();
        it++){//ここで次の関数へ参照先が変わる

            (*it)();//登録された関数を一つ実行
            func_count++;//実行された読み込み関数をカウント

            unsigned int par = (static_cast<unsigned int>((static_cast<float>(func_count) / kFuncMaxNum) * 100.0f));//進捗具合を％で計算
            
            SetProgressNum(par);//セット！

            
            if( GetEndThreadFlag() ){//終了の指示が他のスレッドからされた場合
                break;//スレッド終了
            }
    }
}

unsigned __stdcall LoadingThread::ExecThreadFunc(void *p)
{
    LoadingThread *my_instance = reinterpret_cast<LoadingThread *>(p);//自分自身にキャスト
    my_instance->ExecAllFuncs();

    return 0;
}