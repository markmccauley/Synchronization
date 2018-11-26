#ifndef _FIFORequestChannel_H_                   
#define _FIFORequestChannel_H_

#include "RequestChannel.h"

class MQRequestChannel : public RequestChannel {
private:

    int msqid;
    int send_id;
    int receive_id;
    key_t key;

public:

    // MQ Constructor
    MQRequestChannel(const string _name, const Side _side);

    // MQ Destructor
    ~MQRequestChannel();

    //----- override functions -----
    string cread();
    void cwrite(string _msg);

    //----- accessor functions -----
    int get_send_id();
    /* ??? */

    int get_receive_id();
    /* ??? */
};

#endif