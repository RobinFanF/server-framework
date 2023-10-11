
#include "../include/iomanager.h"
#include "../include/timer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>




frb::Logger::ptr g_logger = GET_LOG_ROOT;

int sock = 0;

void test_fiber() {
    LOG_INFO_STREAM(g_logger) << "test_fiber sock=" << sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "115.239.210.27", &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        LOG_INFO_STREAM(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        frb::IOManager::GetThis()->addEvent(sock, frb::IOManager::READ, [](){
            LOG_INFO_STREAM(g_logger) << "read callback";
        });
        frb::IOManager::GetThis()->addEvent(sock, frb::IOManager::WRITE, [](){
            LOG_INFO_STREAM(g_logger) << "write callback";
            //close(sock);
            frb::IOManager::GetThis()->cancelEvent(sock, frb::IOManager::READ);
            close(sock);
        });
    } else {
        LOG_INFO_STREAM(g_logger) << "else " << errno << " " << strerror(errno);
    }

}

void test1() {
    frb::IOManager iom(2, false);
    iom.schedule(&test_fiber);
}


frb::Timer::ptr s_timer;
void test_timer(){
    frb::IOManager iom(2, false);
    s_timer = iom.addTimer(1000, []()->void{
      static int i = 0;
      LOG_INFO_STREAM(g_logger) << "hello timer i=" << i;
      if(++i == 3) {
        s_timer->reset(2000, true);
        //s_timer->cancel();
      }  
    }, true);

}


int main(int argc, char** argv) {

    //test1();
    test_timer();
    return 0;
}
