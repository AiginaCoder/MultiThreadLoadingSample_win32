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
    void AddRegister(std::function<void()> func);//�ǂݍ��݊֐��ǉ��o�^
    void RunThread();//�X���b�h�J�n�I

    //�Q�b�^�[
    unsigned int GetProgressNum(){
        unsigned int ret;
        HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, kProgressNumMutexName);

        WaitForSingleObject(mutex, INFINITE);//�A�N�Z�X���̊l��
        ret = progress_num_;
        ReleaseMutex(mutex);//�A�N�Z�X���̉��
        return ret;
    }

    unsigned int GetEndThreadFlag(){
        bool ret;
        HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, kEndThreadFlagMutexName);

        WaitForSingleObject(mutex, INFINITE);//�A�N�Z�X���̊l��
        ret = end_thread_flag_;
        ReleaseMutex(mutex);//�A�N�Z�X���̉��
        return ret;
    }

    bool IsActiveThread() const{//�X���b�h�������Ă��邩�ǂ���
        DWORD active_flag;
        GetExitCodeThread(thread_handle_, &active_flag);//�X���b�h�����m�F
        if(active_flag == STILL_ACTIVE){//�X���b�h�������Ă�����
            return true;
        }else{
            return false;
        }
    }

    HANDLE GetHandle() const{
        return thread_handle_;//�X���b�h�n���h��
    }


    //�Z�b�^�[
    void SetProgressNum(unsigned int num){
        HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, kProgressNumMutexName);
        DWORD ret = WaitForSingleObject(mutex, INFINITE);//�A�N�Z�X���̊l��
        progress_num_ = num;
        ReleaseMutex(mutex);//�A�N�Z�X���̉��
    }

    void SetEndThreadFlag(bool flag){
        HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, kEndThreadFlagMutexName);
        WaitForSingleObject(mutex, INFINITE);//�A�N�Z�X���̊l��
        end_thread_flag_ = flag;
        ReleaseMutex(mutex);//�A�N�Z�X���̉��
    }
private:
    //�n���h���̖��O
    static const char* kProgressNumMutexName;
    static const char* kEndThreadFlagMutexName;
    static const char* kThreadHandleName;


    std::vector<std::function<void()>>load_funcs_;//�ǂݍ��݊֐��i�[
    unsigned int progress_num_;//�i���
    bool end_thread_flag_;//�X���b�h�𒆒f������t���O�itrue�Ȃ璆�f�j

    HANDLE progress_num_mutex_;//progress_num_�̔r���pMutex
    HANDLE end_thread_flag_mutex_;//end_thread_flag_�̔r���pMutex
    HANDLE thread_handle_;//�X���b�h�n���h��


    void ExecAllFuncs();//load_functions_�ɓo�^�����֐���S�Ď��s�I
    static unsigned __stdcall ExecThreadFunc(void *p);//���s������X���b�h�֐��istatic�łȂ��Ƃ����Ȃ��j
};

