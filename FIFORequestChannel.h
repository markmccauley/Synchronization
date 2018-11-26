#include "RequestChannel.h"

class FIFORequestChannel : public RequestChannel {
private:

    int wfd;
	int rfd;

	string pipe_name(Mode _mode);
	void create_pipe (string _pipe_name);
	void open_read_pipe(string _pipe_name);
	void open_write_pipe(string _pipe_name);

public:
    // FIFO Constructor
    FIFORequestChannel(const string _name, const Side _side); 

    // FIFO Destructor
    ~FIFORequestChannel();

    //------ override functions -----
    string cread();

    void cwrite(string msg);

    //----- accessor functions -----
    int read_fd();
	/* Returns the file descriptor used to read from the channel. */

	int write_fd();
	/* Returns the file descriptor used to write to the channel. */
};
