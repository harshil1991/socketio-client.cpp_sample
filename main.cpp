#include "sio_client.h"

#include <unistd.h>
#include <functional>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#define HIGHLIGHT(__O__) std::cout<<"\e[1;31m"<<__O__<<"\e[0m"<<std::endl
#define EM(__O__) std::cout<<"\e[1;30;1m"<<__O__<<"\e[0m"<<std::endl

#define MAIN_FUNC int main(int argc ,const char* args[])

using namespace sio;
using namespace std;
std::mutex _lock;

std::condition_variable_any _cond;
bool connect_finish = false;

class connection_listener
{
    sio::client &handler;

public:
    
    connection_listener(sio::client& h):
    handler(h)
    {
    }
    

    void on_connected()
    {
        _lock.lock();
        _cond.notify_all();
        connect_finish = true;
        _lock.unlock();
    }
    void on_close(client::close_reason const& reason)
    {
        std::cout<<"sio closed "<<std::endl;
        exit(0);
    }
    
    void on_fail()
    {
        std::cout<<"sio failed "<<std::endl;
        exit(0);
    }
};

socket::ptr current_socket;
void bind_events()
{
	current_socket->on("server", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                           _lock.lock();
			  cout << name << endl;
			  cout << data->get_string() << endl;
                          _lock.unlock();
                       }));
    
}

MAIN_FUNC
{

    sio::client h;
    connection_listener l(h);
    
    h.set_open_listener(std::bind(&connection_listener::on_connected, &l));
    h.set_close_listener(std::bind(&connection_listener::on_close, &l,std::placeholders::_1));
    h.set_fail_listener(std::bind(&connection_listener::on_fail, &l));
    h.connect("http://127.0.0.1:8081");
    _lock.lock();
    if(!connect_finish)
    {
	cout << "wait\n";
        _cond.wait(_lock);
    }

    _lock.unlock();
    current_socket = h.socket();
    string nickname;
    while (nickname.length() == 0) {
        HIGHLIGHT("Type your nickname:");
        
        getline(cin, nickname);
    }
    current_socket->emit("message", nickname);
    bind_events();
    sleep(10);
    h.sync_close();
    h.clear_con_listeners();
    return 0;
}

