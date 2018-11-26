#include "FIFORequestChannel.h"
#include "unistd.h"

const int MAX_MESSAGE = 255;

std::string FIFORequestChannel::pipe_name(Mode _mode) {
	std::string pname = "fifo_" + my_name;

	if (my_side == CLIENT_SIDE) {
		if (_mode == READ_MODE)
			pname += "1";
		else
			pname += "2";
	}
	else {
	/* SERVER_SIDE */
		if (_mode == READ_MODE)
			pname += "2";
		else
			pname += "1";
	}
	return pname;
}

void FIFORequestChannel::create_pipe (string _pipe_name){
	mkfifo(_pipe_name.c_str(), 0600) < 0; //{
	//	EXITONERROR (_pipe_name);
	//}
}

void FIFORequestChannel::open_write_pipe(string _pipe_name) {
	
	//if (my_side == SERVER_SIDE)
		create_pipe (_pipe_name);

	wfd = open(_pipe_name.c_str(), O_WRONLY);
	if (wfd < 0) {
		EXITONERROR (_pipe_name);
	}
}

void FIFORequestChannel::open_read_pipe(string _pipe_name) {

	//if (my_side == SERVER_SIDE)
		create_pipe (_pipe_name);
	rfd = open(_pipe_name.c_str (), O_RDONLY);
	if (rfd < 0) {
		perror ("");
		exit (0);
	}
}

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS  F I F O R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

FIFORequestChannel::FIFORequestChannel(const string _name, const Side _side) : RequestChannel(_name, _side){
	if (_side == SERVER_SIDE) {
		open_write_pipe(pipe_name(WRITE_MODE).c_str());
		open_read_pipe(pipe_name(READ_MODE).c_str());
	}
	else {
		open_read_pipe(pipe_name(READ_MODE).c_str());
		open_write_pipe(pipe_name(WRITE_MODE).c_str());
	}
    RequestChannel::method = 'f';
}

FIFORequestChannel::~FIFORequestChannel(){

   	close(wfd);
	close(rfd);
	//if (my_side == SERVER_SIDE) {
		remove(pipe_name(READ_MODE).c_str());
		remove(pipe_name(WRITE_MODE).c_str());
	//}
 
}

string FIFORequestChannel::cread(){
    char buf [MAX_MESSAGE];
	if (read(rfd, buf, MAX_MESSAGE) <= 0) {
		EXITONERROR ("cread");
	}
	string s = buf;
	return s;
};

void FIFORequestChannel::cwrite(string msg){
    if (msg.size() > MAX_MESSAGE) {
		EXITONERROR ("cwrite");
	}
	if (write(wfd, msg.c_str(), msg.size()+1) < 0) { // msg.size() + 1 to include the NULL byte
		EXITONERROR ("cwrite");
	}
}

int FIFORequestChannel::read_fd() {
	return rfd;
}

int FIFORequestChannel::write_fd() {
	return wfd;
}
