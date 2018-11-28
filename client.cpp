/*
    Based on original assignment by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#include <sys/time.h>
#include <cassert>
#include <assert.h>

#include <cmath>
#include <numeric>
#include <algorithm>

#include <list>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "FIFORequestChannel.h"
#include "MQRequestChannel.h"
#include "SHMRequestChannel.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include <chrono>
#include <csignal>

using namespace std;

// Global variables
Histogram hist;

// create signal handler
void signal_handler(int) {
    signal(SIGALRM, signal_handler);
    system("clear"); 
    hist.print();
    alarm(2);
}

// structs to hold arguments
struct Request {
    int n;
    string req; // string for requests
    BoundedBuffer* request_buf; // a pointer to data in a buffer
};

struct Worker {
    BoundedBuffer* request_buf; 
    BoundedBuffer* response_buf1; // buffer for John
    BoundedBuffer* response_buf2; // buffer for Jane
    BoundedBuffer* response_buf3; // buffer for Joe
    RequestChannel* workerChannel; // channel for worker threads
};

struct Stat {
    int n; 
    string req; // string for requests
    BoundedBuffer* response_buf; 
    Histogram *hist; // argument for histogram    
};

// thread functions
void* request_thread_function(void *arg) { 

    Request *args = (Request *) arg;

    string data = args->req;
	for(int i = 0; i < args->n; i++) {
        args->request_buf->push(data); // push requests into buffer       
	}
    pthread_exit(NULL);
}

void* worker_thread_function(void* arg) { 

    Worker *args = (Worker *) arg;

    while(true) {
        string request = args->request_buf->pop();
		args->workerChannel->cwrite(request);

		if (request == "quit") { // check for quit
			delete args->workerChannel; // properly handle worker channel
            break;
        } else {
			string response = args->workerChannel->cread(); // sort requests into respective buffers
            if (request == "data John Smith"){ // check for John
                args->response_buf1->push(response);
            }
            else if (request == "data Jane Smith"){ // check for Jane
                args->response_buf2->push(response);
            }
            else if (request == "data Joe Smith"){ // check for Joe
                args->response_buf3->push(response);
            }
		}
    }
    pthread_exit(NULL);
}

void* stat_thread_function(void* arg) {

    Stat *args = (Stat *) arg;

    for(int i = 0; i < args->n; i++){
        string response = args->response_buf->pop(); // pop response
	    args->hist->update(args->req, response); // update the histogram
    }
    pthread_exit(NULL);
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 100; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int b = 1; // default capacity for BoundedBuffer
    string i = "f"; // default ipc method is FIFO
    int opt = 0;

    // register the custom signal handler for SIGALRM
    signal(SIGALRM, signal_handler);
    alarm(2);

    // Commandline args parsing
    while ((opt = getopt(argc, argv, "n:w:b:i:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'i':
                i = strdup(optarg);
                break;
			}
    }

    int pid = fork();
	if (pid == 0){
        // Pass request channel type as argument
		execl("dataserver", "dataserver", "-i", i.c_str(), (char*) NULL);
	}
	else {

        cout << "n == " << n << endl;
        cout << "w == " << w << endl;
        cout << "b == " << b << endl;
        cout << "i == " << i << endl;

        cout << "CLIENT STARTED:" << endl;
        cout << "Establishing control channel... " << flush;
        RequestChannel *chan = nullptr;
        if (i == "f") {
            chan = new FIFORequestChannel("control", RequestChannel::CLIENT_SIDE);
        }
        else if (i == "q") {
            chan = new MQRequestChannel("control", RequestChannel::CLIENT_SIDE);
        } 
        else if (i == "s") {
            chan = new SHMRequestChannel("control", RequestChannel::CLIENT_SIDE);
        } else {
            chan = new FIFORequestChannel("control", RequestChannel::CLIENT_SIDE);
        }
        cout << "done." << endl<< flush;

        pthread_t request_threads[3]; // initialize threads
        pthread_t worker_threads[w];
        pthread_t stat_threads[3];

        Request requests[3]; // initialize argument storage
        Worker workers[w];
        Stat stats[3];

        // initialize buffers with capacity
		BoundedBuffer request_buf(b);
        BoundedBuffer request_buf1(ceil(b/3)), request_buf2(ceil(b/3)), request_buf3(ceil(b/3));

        // array of patients requests
        string patients[3] = {"data John Smith", "data Jane Smith", "data Joe Smith"};

        for (int i = 0; i < 3; i++) { // create request threads
            requests[i].n = n; // assign arguments
            requests[i].req = patients[i];
            requests[i].request_buf = &request_buf;
            pthread_create(&request_threads[i], NULL, request_thread_function, (void *) &requests[i]);
        } 

       // start timer
        struct timeval begin, end;
        gettimeofday(&begin, NULL);

        for (int i = 0; i < w; ++i) { // create worker threads
            workers[i].request_buf = &request_buf; // assign arguments
            workers[i].response_buf1 = &request_buf1;
            workers[i].response_buf2 = &request_buf2;
            workers[i].response_buf3 = &request_buf3;
            chan->cwrite("newchannel");
		    string s = chan->cread();

            RequestChannel *workerChannel = NULL;
            if (chan->get_method() == 'f') {
                workerChannel = new FIFORequestChannel(s, RequestChannel::CLIENT_SIDE);
            }
            else if (chan->get_method() == 'q') {
                workerChannel = new MQRequestChannel(s, RequestChannel::CLIENT_SIDE);
            }
            else if (chan->get_method() == 's') {
                workerChannel = new SHMRequestChannel(s, RequestChannel::CLIENT_SIDE);
            } else {
                workerChannel = new FIFORequestChannel(s, RequestChannel::CLIENT_SIDE);
            }
            workers[i].workerChannel = workerChannel;
            pthread_create(&worker_threads[i], NULL, worker_thread_function, (void *) &workers[i]);
        }


        stats[0].response_buf = &request_buf1; // assign respective buffers
        stats[1].response_buf = &request_buf2;
        stats[2].response_buf = &request_buf3;

        for(int i = 0; i < 3; i++){
            stats[i].req = patients[i]; // assign arguments
            stats[i].hist = &hist;
            stats[i].n = n;
            pthread_create(&stat_threads[i], NULL, stat_thread_function, (void *) &stats[i]); 
        }

        // join threads
        for(int i = 0; i < 3; ++i){
            pthread_join(request_threads[i], NULL);
        }
        for(int i = 0; i < w; ++i) {
            request_buf.push("quit");
        }
        for(int i = 0; i < w; ++i){
            pthread_join(worker_threads[i], NULL);
        }
        for(int i = 0; i < 3; ++i){
            pthread_join(stat_threads[i], NULL);
        }

        // End timer
        gettimeofday(&end, NULL);
        int64_t start = begin.tv_sec * 1000000L + begin.tv_usec;
        int64_t finish = end.tv_sec * 1000000L + end.tv_usec;
        int64_t time = finish - start;

        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl;

        system("clear");
		hist.print ();
        cout << time << "Microseconds" << endl;
    }
}
