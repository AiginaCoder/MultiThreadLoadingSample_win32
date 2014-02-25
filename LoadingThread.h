#pragma once
#include <Windows.h>
#include <process.h>
#include <vector>
#include <functional>
#include <Windows.h>


class LoadingThread
{
public:
    LoadingThread();
    ~LoadingThread();
    void AddRegister(std::function<void()> func);//読み込み関数追加登録
    void RunThread();//スレッド開始！

    //ゲッター
    unsigned int GetProgressNum(){
        unsigned int ret;
        HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, kProgressNumMutexName);

        WaitForSingleObject(mutex, INFINITE);//アクセス権の獲得
        ret = progress_num_;
        ReleaseMutex(mutex);//アクセス権の解放
        return ret;
    }

    unsigned int GetEndThreadFlag(){
        bool ret;
        HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, kEndThreadFlagMutexName);

        WaitForSingleObject(mutex, INFINITE);//アクセス権の獲得
        ret = end_thread_flag_;
        ReleaseMutex(mutex);//アクセス権の解放
        return ret;
    }

    bool IsActiveThread() const{//スレッドが生きているかどうか
        DWORD active_flag;
        GetExitCodeThread(thread_handle_, &active_flag);//スレッド生存確認
        if(active_flag == STILL_ACTIVE){//スレッドが生きていたら
            return true;
        }else{
            return false;
        }
    }

    HANDLE GetHandle() const{
        return thread_handle_;//スレッドハンドル
    }


    //セッター
    void SetProgressNum(unsigned int num){
        HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, kProgressNumMutexName);
        DWORD ret = WaitForSingleObject(mutex, INFINITE);//アクセス権の獲得
        progress_num_ = num;
        ReleaseMutex(mutex);//アクセス権の解放
    }

    void SetEndThreadFlag(bool flag){
        HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, kEndThreadFlagMutexName);
        WaitForSingleObject(mutex, INFINITE);//アクセス権の獲得
        end_thread_flag_ = flag;
        ReleaseMutex(mutex);//アクセス権の解放
    }
private:
    //ハンドルの名前
    static const char* kProgressNumMutexName;
    static const char* kEndThreadFlagMutexName;
    static const char* kThreadHandleName;


    std::vector<std::function<void()>>load_funcs_;//読み込み関数格納
    unsigned int progress_num_;//進捗具合
    bool end_thread_flag_;//スレッドを中断させるフラグ（trueなら中断）

    HANDLE progress_num_mutex_;//progress_num_の排他用Mutex
    HANDLE end_thread_flag_mutex_;//end_thread_flag_の排他用Mutex
    HANDLE thread_handle_;//スレッドハンドル


    void ExecAllFuncs();//load_functions_に登録した関数を全て実行！
    static unsigned __stdcall ExecThreadFunc(void *p);//実行させるスレッド関数（staticでないといけない）
};

