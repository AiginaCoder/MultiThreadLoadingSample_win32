#include "LoadingThread.h"

//static����
const char* LoadingThread::kProgressNumMutexName = "ProgressNumMutex";
const char* LoadingThread::kEndThreadFlagMutexName = "EndThreadFlagMutex";
const char* LoadingThread::kThreadHandleName = "ThreadHandle";


LoadingThread::LoadingThread()
{
    progress_num_mutex_ = CreateMutex(NULL, FALSE, kProgressNumMutexName);//progress_num_�̔r���pMutex�̍쐬
    end_thread_flag_mutex_ = CreateMutex(NULL, FALSE, kEndThreadFlagMutexName);//end_thread_flag_mutex_�̔r���pMutex�̍쐬

    progress_num_ = 0;//�i���
    end_thread_flag_ = false;//�X���b�h�𒆒f������t���O�itrue�Ȃ璆�f�j

    //�X���b�h�̍쐬
    thread_handle_ = reinterpret_cast<HANDLE>(//�߂�l��HANDLE�^�ɃL���X�g���Ă��K�v������
        _beginthreadex(NULL, 0,
        LoadingThread::ExecThreadFunc,//���s����X���b�h�֐�
        this,//�X���b�h�֐��ɓn���������A�^��void*�ɈÖٓI�ɃL���X�g����܂��i�g��Ȃ��Ȃ�NULL�ł����j
        CREATE_SUSPENDED,//�X���b�h�쐬�Ɋւ��鐧��t���O���w�肵�܂�(0�ő����s)
        NULL)//�X���b�h�����ʂ���ID������
        );
}

LoadingThread::~LoadingThread(void)
{
    CloseHandle(progress_num_mutex_);//�r���pMutex�̍폜
    CloseHandle(end_thread_flag_mutex_);//�r���pMutex�̍폜
    CloseHandle(thread_handle_);//HANDLE�폜
}

void LoadingThread::AddRegister(std::function<void()> func)
{
    load_funcs_.push_back(func);//�ǉ��o�^�I
}

void LoadingThread::RunThread()
{
    ResumeThread(thread_handle_);//�X���b�h�̋N��
}

void LoadingThread::ExecAllFuncs()
{
    const unsigned int kFuncMaxNum = load_funcs_.size();//�o�^�����ǂݍ��݊֐��̐����擾
    unsigned int func_count = 0;//���s���������ǂݍ��݊֐��̐��𐔂���

    //�C�e���[�^��p���ēo�^���ꂽ�ǂݍ��݊֐����P���ŏ�����Ō�܂Ŏ��s���Ă���
    for(std::vector<std::function<void()>>::iterator it = load_funcs_.begin();
        it != load_funcs_.end();
        it++){//�����Ŏ��̊֐��֎Q�Ɛ悪�ς��

            (*it)();//�o�^���ꂽ�֐�������s
            func_count++;//���s���ꂽ�ǂݍ��݊֐����J�E���g

            unsigned int par = (static_cast<unsigned int>((static_cast<float>(func_count) / kFuncMaxNum) * 100.0f));//�i��������Ōv�Z
            
            SetProgressNum(par);//�Z�b�g�I

            
            if( GetEndThreadFlag() ){//�I���̎w�������̃X���b�h���炳�ꂽ�ꍇ
                break;//�X���b�h�I��
            }
    }
}

unsigned __stdcall LoadingThread::ExecThreadFunc(void *p)
{
    LoadingThread *my_instance = reinterpret_cast<LoadingThread *>(p);//�������g�ɃL���X�g
    my_instance->ExecAllFuncs();

    return 0;
}