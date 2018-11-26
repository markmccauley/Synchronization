#include "MQRequestChannel.h"

struct msg_buf {
    long mtype;
    char mtext[100];
};

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS  F I F O R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

MQRequestChannel::MQRequestChannel(const std::string _name, const Side _side) : RequestChannel(_name, _side) {
    if (_side == SERVER_SIDE) {
        send_id = 2;
        receive_id = 1;
    } else {
        send_id = 1;
        receive_id = 2;
    }
    string file = "mq_" + _name;
    creat(file.c_str(), 0644);
    key = ftok(file.c_str(), 1);
    msqid = msgget(key, 0644 | IPC_CREAT);

    RequestChannel::method = 'q';
}

MQRequestChannel::~MQRequestChannel() {
    msgctl(msqid, IPC_RMID, NULL);
    string file = "mq_" + my_name;
    remove(file.c_str());
}

string MQRequestChannel::cread() {
    msg_buf buf;
    int rcv = get_receive_id();
    msgrcv(msqid, &buf, sizeof(buf.mtext), rcv, 0);
    string msg = buf.mtext;
    return msg;
}

void MQRequestChannel::cwrite(string _msg) {
    msg_buf buf;
    strncpy(buf.mtext, _msg.c_str(), _msg.size()+1);
    buf.mtype = get_send_id();
    if (msgsnd(msqid, &buf, _msg.size()+1, 0) == -1) {
        perror("send failed");
    }
}

int MQRequestChannel::get_send_id() {
    return send_id;
}

int MQRequestChannel::get_receive_id() {
    return receive_id;
}