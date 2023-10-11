#include "../include/iomanager.h"
#include "../include/log.h"
#include "../include/tcp_server.h"
frb::Logger::ptr g_logger = GET_LOG_ROOT();

void run() {
    auto addr = frb::Address::LookupAny("0.0.0.0:8033");
    
    std::vector<frb::Address::ptr> addrs;
    addrs.push_back(addr);
    

    frb::TcpServer::ptr tcp_server(new frb::TcpServer);
    std::vector<frb::Address::ptr> fails;
    while(!tcp_server->bind(addrs, fails)) {
        sleep(2);
    }
    tcp_server->start();
    
}
int main(int argc, char** argv) {
    frb::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
