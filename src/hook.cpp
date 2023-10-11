#include "../include/hook.h"
#include "../include/config.h"
#include "../include/log.h"
#include "../include/fiber.h"
#include "../include/iomanager.h"
#include "../include/fd_manager.h"
#include "../include/macro.h"

#include <dlfcn.h>
#include<stdarg.h>


frb::Logger::ptr g_logger = GET_LOG_NAME("system");
namespace frb{

static frb::ConfigVar<int>::ptr g_tcp_connect_timeout =
    frb::Config::Lookup("tcp.connect.timeout", 5000, "tcp connect timeout");


static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(fcntl) \
    XX(ioctl) \
    XX(getsockopt) \
    XX(setsockopt)



void hook_init() {
    static bool is_inited = false;
    if(is_inited) {
        return;
    }
//宏定义，把XX(name)展开 , 获取共享库里的sleep的函数符号，保存到sleep_f 
//sleep_f = (sleep_fun)dlsym(RTLD_NEXT, "sleep");
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
       HOOK_FUN(XX);
#undef XX
}

static uint64_t s_connect_timeout = -1;
struct _HookIniter {
    _HookIniter() {
        hook_init();
        s_connect_timeout = g_tcp_connect_timeout->getValue();

        g_tcp_connect_timeout->addListener([](const int& old_value, const int& new_value){
                LOG_INFO_STREAM(g_logger) << "tcp connect timeout changed from "
                                         << old_value << " to " << new_value;
                s_connect_timeout = new_value;
        });
    }
};

static _HookIniter s_hook_initer;

bool is_hook_enable() {
    return t_hook_enable;
}

void set_hook_enable(bool flag) {
    t_hook_enable = flag;
}

}

/**
* @brief todo
*/
struct timer_info {
    int cancelled = 0;
};


/**
* @param fd fd
* @param fun 系统函数
* @param hook_fun_name hook函数名
* @param event io事件类型
* @param timeout_so io超时时间类型
* @param args 可变参数，以万能引用的类型传入，方便完美转发（传入左值，发送左值；传入右值，发送右值）
*/
template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
        uint32_t event, int timeout_so, Args&&... args) {
    if(!frb::t_hook_enable){
        return fun(fd, std::forward<Args>(args)...);
    }

    //拿到fd的状态
    frb::FdCtx::ptr ctx = frb::FdMgr::GetInstance()->get(fd);
    
    if(!ctx) {
        return fun(fd, std::forward<Args>(args)...);
    }

    if(ctx->isClose()) {
        errno = EBADF;
        return -1;
    }

    //如果是socekt，但不是用户主动设置的非阻塞

    if(!ctx->isSocket() || ctx->getUserNonblock()) {
        return fun(fd, std::forward<Args>(args)...);
    }

    //fd既是socket 又是 非阻塞的

    uint64_t to = ctx->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info);

retry:
    ssize_t n = fun(fd, std::forward<Args>(args)...);

    //IO失败，由中断引起
    while( n == -1 && errno == EINTR) {
        n = fun(fd, std::forward<Args>(args)...);
    }
    //IO失败，读缓冲区空或者写缓冲区满
    if(n == -1 && errno == EAGAIN) {
        frb::IOManager* iom = frb::IOManager::GetThis();
        frb::Timer::ptr timer;

        //timer的回调函数，被放入任务队列，触发时检查条件是否成立
        std::weak_ptr<timer_info> winfo(tinfo);

        //如果有超时时间
        //超时时间到，通过条件判断是否要取消该IO事件（通过定时器实现）
        //条件：
        //1： 条件为空 
        //或者2： 定时器被取消 
        //则说明IO事件被触发
        if(to != (uint64_t)-1) {
            timer = iom->addConditionTimer(to, [winfo, fd, iom, event]() {
                auto t = winfo.lock();
                if(!t || t->cancelled){
                    return ;
                }
                t->cancelled = ETIMEDOUT;
                iom->cancelEvent(fd, (frb::IOManager::Event)(event));
            }, winfo);
        }

        LOG_INFO_STREAM(g_logger) << "iomanager"<<iom;
        int rt = iom->addEvent(fd, (frb::IOManager::Event)(event));
        
        //添加失败
        if(rt) {
            LOG_ERROR_STREAM(g_logger) << hook_fun_name << " addEvent("
                << fd << ", " << event << ")";
            
            if(timer){
                timer->cancel();
            }

            return -1;

        } else {
            //唤醒有两种条件：
            //1.注册的IO事件到达
            //2.IO超时
            frb::Fiber::YieldToHold();
            if(timer) {
                timer->cancel();
            }

            //如果cancelled为真，说明时通过IO超时唤醒的，
            //直接退出io操作，否则继续IO
            if(tinfo->cancelled){
                errno = tinfo->cancelled;
                return -1;
            }

            goto retry;
        }
    }
    return n;
}

extern "C" {
#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX);
#undef XX

//sleep
//添加seconds后的定时器然后挂起，让出执行时间，唤醒定时器时回到该协程
unsigned int sleep(unsigned int seconds) {
    if(!frb::t_hook_enable){
        return sleep_f(seconds);
    }
    frb::Fiber::ptr fiber = frb::Fiber::GetThis();
    frb::IOManager* iom = frb::IOManager::GetThis();

    //BIND模板函数要提前声明类型
    //这里bind的是指向Scheduler成员函数的函数指针
    iom->addTimer(seconds * 1000, std::bind((void(frb::Scheduler::*)
        (frb::Fiber::ptr, int))&frb::IOManager::schedule, iom, fiber, -1));

    frb::Fiber::YieldToHold();
    return 0;

}

int usleep(useconds_t usec) {
    if(!frb::t_hook_enable){
        return usleep_f(usec);
    }
    frb::Fiber::ptr fiber = frb::Fiber::GetThis();
    frb::IOManager* iom = frb::IOManager::GetThis();

    iom->addTimer(usec / 1000, std::bind((void(frb::Scheduler::*)
        (frb::Fiber::ptr, int))&frb::IOManager::schedule, iom, fiber, -1));

    frb::Fiber::YieldToHold();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem){
    if(!frb::t_hook_enable) {
        return nanosleep_f(req, rem);
    }

    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 /1000;
    frb::Fiber::ptr fiber = frb::Fiber::GetThis();
    frb::IOManager* iom = frb::IOManager::GetThis();
    iom->addTimer(timeout_ms, std::bind((void(frb::Scheduler::*)
        (frb::Fiber::ptr, int))&frb::IOManager::schedule, iom, fiber, -1));
    frb::Fiber::YieldToHold();
    return 0;  
}

//socket
//对原来的系统函数socket的封装，创建成功则把fd放如fdmgr
int socket(int domain, int type, int protocol) {
    if(!frb::t_hook_enable) {
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, protocol);
    if(fd == -1) {
        return fd;
    }

    //如果是socket，会设置其为非阻塞
    frb::FdMgr::GetInstance()->get(fd, true);
    return fd;
}

//todo
int connect_with_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout_ms){
    if(!frb::t_hook_enable) {
        return connect_f(fd, addr, addrlen);
    }

    frb::FdCtx::ptr ctx = frb::FdMgr::GetInstance()->get(fd);
    if(!ctx || ctx->isClose()) {
        errno = EBADF;
        return -1;
    }
    
    if(!ctx->isSocket()) {
        return connect_f(fd, addr, addrlen);
    }

    if(ctx->getUserNonblock()) {
        return connect_f(fd, addr, addrlen);
    }
    LOG_INFO_STREAM(g_logger) << "errno"<<errno;
    int n = connect_f(fd, addr, addrlen);
    LOG_INFO_STREAM(g_logger) << "errno"<<errno;
    if(n == 0) {
        return 0;
    } else if(n != -1 || errno != EINPROGRESS) {
        return n;
    }

    frb::IOManager* iom = frb::IOManager::GetThis();
    frb::Timer::ptr timer;
    std::shared_ptr<timer_info> tinfo(new timer_info);
    std::weak_ptr<timer_info> winfo(tinfo);


    //如果设置了超时时间，添加定时器
    //定时器，到时如果满足条件触发
    if(timeout_ms != (uint64_t)-1) {
        timer = iom->addConditionTimer(timeout_ms, [winfo, fd, iom]() {
                auto t = winfo.lock();
                if(!t || t->cancelled) {
                    return;
                }
                t->cancelled = ETIMEDOUT;
                iom->cancelEvent(fd, frb::IOManager::WRITE);
        }, winfo);
    }
    
    int rt = iom->addEvent(fd, frb::IOManager::WRITE);
    if(rt == 0) {
        LOG_INFO_STREAM(g_logger) << "addEvent("<<fd<<", "<< frb::IOManager::WRITE<<")";
        frb::Fiber::YieldToHold();
        //fd上的可写事件发生，回到该协程上
        if(timer) {
            timer->cancel();
        }
        if(tinfo->cancelled) {
            errno = tinfo->cancelled;
            return -1;
        }
    } else {
        if(timer) {
            timer->cancel();
        }
        LOG_ERROR_STREAM(g_logger) << "connect addEvent(" << fd << ", WRITE) error";
    }

    int error = 0;
    socklen_t len = sizeof(int);
    if(-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)) {
        return -1;
    }
    if(!error) {
        return 0;
    } else {
        errno = error;
        return -1;
    }
}

//todo
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return connect_with_timeout(sockfd, addr, addrlen,frb::s_connect_timeout );//;
}

int accept(int s, struct sockaddr *addr, socklen_t *addrlen) {
    int fd = do_io(s, accept_f, "accept", frb::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if(fd >= 0) {
        frb::FdMgr::GetInstance()->get(fd, true);
    }
    return fd;
}

//io

ssize_t read(int fd, void *buf, size_t count) {
    return do_io(fd, read_f, "read", frb::IOManager::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, readv_f, "readv", frb::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", frb::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    return do_io(sockfd, recvfrom_f, "recvfrom", frb::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return do_io(sockfd, recvmsg_f, "recvmsg", frb::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return do_io(fd, write_f, "write", frb::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, writev_f, "writev", frb::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int s, const void *msg, size_t len, int flags) {
    return do_io(s, send_f, "send", frb::IOManager::WRITE, SO_SNDTIMEO, msg, len, flags);
}

ssize_t sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen) {
    return do_io(s, sendto_f, "sendto", frb::IOManager::WRITE, SO_SNDTIMEO, msg, len, flags, to, tolen);
}

ssize_t sendmsg(int s, const struct msghdr *msg, int flags) {
    return do_io(s, sendmsg_f, "sendmsg", frb::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}

int close(int fd) {
    if(!frb::t_hook_enable) {
        return close_f(fd);
    }

    frb::FdCtx::ptr ctx = frb::FdMgr::GetInstance()->get(fd);
    if(ctx) {
        auto iom = frb::IOManager::GetThis();
        if(iom) {
            iom->cancelAll(fd);
        }
        frb::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

///

int fcntl(int fd, int cmd, ...){
    va_list va;
    va_start(va, cmd) ;
    switch(cmd){
        case F_SETFL:
            {
                //arg 是文件的状态
                int arg = va_arg(va, int);
                va_end(va);
                frb::FdCtx::ptr ctx = frb::FdMgr::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()) {
                    return fcntl_f(fd, cmd, arg);
                }

                //arg是fcntl的参数
                //arg: ----0（1）--   
                //如果fd为非阻塞，arg & O_NONBLOCK = true
                //如果fd为阻塞, arg & O_NONBLOCK = fasle
                ctx->setUserNonblock(arg & O_NONBLOCK);
                
                //如果设置系统为非阻塞的
                if(ctx->getSysNonblock()) {
                    arg |= O_NONBLOCK; 
                } else {
                    arg &= ~O_NONBLOCK;
                }

                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETFL:
            {
                va_end(va);
                int arg = fcntl_f(fd, cmd);
                frb::FdCtx::ptr ctx = frb::FdMgr::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()) {
                    return arg;
                }
                if(ctx->getUserNonblock()) {
                    return arg | O_NONBLOCK;
                } else {
                    return arg & ~O_NONBLOCK;
                }
            }
            break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
#ifdef F_SETPIPE_SZ
        case F_SETPIPE_SZ:
#endif
            {
                int arg = va_arg(va, int);
                va_end(va);
                return fcntl_f(fd, cmd, arg); 
            }
            break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
#ifdef F_GETPIPE_SZ
        case F_GETPIPE_SZ:
#endif
            {
                va_end(va);
                return fcntl_f(fd, cmd);
            }
            break;
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                struct flock* arg = va_arg(va, struct flock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        default:
            va_end(va);
            return fcntl_f(fd, cmd);
    }
}

//没看
int ioctl(int d, unsigned long int request, ...) {
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);

    if(FIONBIO == request) {
        bool user_nonblock = !!*(int*)arg;
        frb::FdCtx::ptr ctx = frb::FdMgr::GetInstance()->get(d);
        if(!ctx || ctx->isClose() || !ctx->isSocket()) {
            return ioctl_f(d, request, arg);
        }
        ctx->setUserNonblock(user_nonblock);
    }
    return ioctl_f(d, request, arg);
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if(!frb::t_hook_enable) {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if(level == SOL_SOCKET) {
        if(optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            frb::FdCtx::ptr ctx = frb::FdMgr::GetInstance()->get(sockfd);
            if(ctx) {
                const timeval* v = (const timeval*)optval;
                ctx->setTimeout(optname, v->tv_sec * 1000 + v->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}

}
