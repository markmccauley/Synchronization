#include "SHMBoundedBuffer.h"

class SHMRequestChannel : public RequestChannel {
private:

    SHMBoundedBuffer *server_buffer;
    SHMBoundedBuffer *client_buffer;
    
public:

    // SHM Constructor
    SHMRequestChannel(const string _name, const Side _side);

    // SHM Destructor
    ~SHMRequestChannel();

    // ----- override functions -----
    string cread();
    void cwrite(string _msg);

};