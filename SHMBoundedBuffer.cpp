#include "SHMBoundedBuffer.h"

SHMBoundedBuffer::SHMBoundedBuffer(string name){
    my_name = "shm_" + name;
    creat(my_name.c_str(), 0666); // create a file for buffer

    // generate unique keys
    key_t key = ftok(my_name.c_str(), 0); 
    key_t key_full = ftok(my_name.c_str(), 1);
    key_t key_empty = ftok(my_name.c_str(), 2);

    shmid = shmget(key, 1024, 0666|IPC_CREAT);
    buf = (char*) shmat(shmid, 0, 0); 

    full = new KernelSemaphore(0, key_full);
    empty = new KernelSemaphore(1, key_empty);
}

SHMBoundedBuffer::~SHMBoundedBuffer(){
    delete full;
    delete empty;
    shmdt(buf);
    shmctl(shmid, IPC_RMID, 0);
    remove(my_name.c_str());
}

void SHMBoundedBuffer::push(string msg) { // synchronize push with semaphore
    empty->P();
    strncpy(buf, msg.c_str(), msg.size()+1);
    full->V();
}

string SHMBoundedBuffer::pop() { // synchronize pop with semaphore
    full->P();
    string data_string = buf;
    empty->V();
    return data_string;
}