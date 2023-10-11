#include "../include/fd_manager.h"
#include "../include/hook.h"
#include "../include/log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace frb{

FdCtx::FdCtx(int fd)
    :m_isInit(false)
    ,m_isSocket(false)
    ,m_sysNonblock(false)
    ,m_userNonblock(false)
    ,m_isClosed(false)
    ,m_fd(fd)
    ,m_recvTimeout(-1)
    ,m_sendTimeout(-1) {
    init();
}

FdCtx::~FdCtx() {
}

bool FdCtx::init(){
    if(m_isInit) {
        return true;
    }
    
    //获取fd的属性
    struct stat fd_stat;
    //失败
    if(-1 == fstat(m_fd, &fd_stat)) {
        m_isInit = false;
        m_isSocket = false;
    } else {
        m_isInit = true;
        //是否是socket
        m_isSocket = S_ISSOCK(fd_stat.st_mode);  
    }

    if(m_isSocket){
        //是socket，获得fd的状态标志，第三个参数为void
        int flags = fcntl_f(m_fd, F_GETFL, 0);
        //如果fd是阻塞的，修改为非阻塞的
        if(!(flags & O_NONBLOCK)){
            fcntl_f(m_fd, F_SETFL, flags | O_NONBLOCK);
        }
        m_sysNonblock = true;
    } else {
        m_sysNonblock = false;
    }

    m_userNonblock = false;
    m_isClosed = false;
    return m_isInit;
}


void FdCtx::setTimeout(int type, uint64_t v) {
    if(type == SO_RCVTIMEO) {
        m_recvTimeout = v;
    } else {
        m_sendTimeout = v;
    }
}

uint64_t FdCtx::getTimeout(int type) {
    if(type == SO_RCVTIMEO) {
        return m_recvTimeout;
    } else {
        return m_sendTimeout;
    }
}

//fdmanager具有初始大小
FdManager::FdManager() {
    m_datas.resize(64);
}

FdCtx::ptr FdManager::get(int fd, bool auto_create) {
    if(fd == -1) {
        return nullptr;
    }

    RWMutexType::ReadLock lock(m_mutex);
    
    //vector的容量小于fd，说明，vector里没有该fd，
    //如果auto_Create为false， return null
    if((int)m_datas.size() <= fd) {
        if(auto_create == false) {
            return nullptr;
        } 
    } else {
        //直接返回fd的fdctx，可能为空
        if(m_datas[fd] || !auto_create) {
            return m_datas[fd];
        }
    }

    lock.unlock();
    //自动创建
    RWMutexType::WriteLock lock2(m_mutex);
    FdCtx::ptr ctx(new FdCtx(fd));
    if(fd >= (int)m_datas.size()) {
        m_datas.resize(fd * 1.5);
    }

    m_datas[fd] = ctx;
    return ctx;

}


void FdManager::del(int fd) {
    RWMutexType::WriteLock lock(m_mutex);
    if((int)m_datas.size() <= fd) {
        return;
    }
    m_datas[fd].reset();
}
}