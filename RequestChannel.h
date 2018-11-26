#ifndef _RequestChannel_H_                   
#define _RequestChannel_H_

#include <iostream>
#include <fstream>
#include <exception>
#include <string>

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

using namespace std;

void EXITONERROR (string msg);

class RequestChannel {

public:

	typedef enum {SERVER_SIDE, CLIENT_SIDE} Side;

	typedef enum {READ_MODE, WRITE_MODE} Mode;

//private:
protected:

	string   my_name = "";
	string side_name = "";
	Side     my_side;
    char     method;

public:

	/* -- CONSTRUCTOR/DESTRUCTOR */

	RequestChannel(const string _name, const Side _side);
	/* Creates a "local copy" of the channel specified by the given name. 
	 If the channel does not exist, the associated IPC mechanisms are 
	 created. If the channel exists already, this object is associated with the channel.
	 The channel has two ends, which are conveniently called "SERVER_SIDE" and "CLIENT_SIDE".
	 If two processes connect through a channel, one has to connect on the server side 
	 and the other on the client side. Otherwise the results are unpredictable.

	 NOTE: If the creation of the request channel fails (typically happens when too many
	 request channels are being created) and error message is displayed, and the program
	 unceremoniously exits.

	 NOTE: It is easy to open too many request channels in parallel. In most systems,
	 limits on the number of open files per process limit the number of established
	 request channels to 125.
	*/

	virtual ~RequestChannel();
	/* Destructor of the local copy of the bus. By default, the Server Side deletes any IPC 
	 mechanisms associated with the channel. */

	virtual string cread() = 0;
	/* Blocking read of data from the channel. Returns a string of characters
	 read from the channel. Returns NULL if read failed. */

	virtual void cwrite(string _msg) = 0;
	/* Write the data to the channel. The function returns the number of characters written
	 to the channel. */

	string name();
	/* Returns the name of the request channel. */

    char get_method();
	/* Returns the method chosen by the user */
};

#endif