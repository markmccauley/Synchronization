#include "SHMRequestChannel.h"

SHMRequestChannel::SHMRequestChannel(const std::string _name, const Side _side) : RequestChannel(_name, _side){
        string name = _name + "2";

        // create a buffer for server and for client
        server_buffer = new SHMBoundedBuffer(_name);
        client_buffer = new SHMBoundedBuffer(name);

        RequestChannel::method = 's'; // set ipc method 
}

SHMRequestChannel::~SHMRequestChannel(){
    delete server_buffer;
    delete client_buffer;
}

string SHMRequestChannel::cread(){
    if(RequestChannel::my_side == SERVER_SIDE){
        return server_buffer->pop();
    }
    else {
        return client_buffer->pop();
    }
}

void SHMRequestChannel::cwrite(string _msg){
    if(RequestChannel::my_side == SERVER_SIDE){
        client_buffer->push(_msg);
    }
    else {
        server_buffer->push(_msg);
    }
}   
