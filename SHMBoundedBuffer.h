#include "KernelSemaphore.h"

class SHMBoundedBuffer {
private:

    int shmid;
    char *buf;
    string my_name;
    KernelSemaphore *full;
    KernelSemaphore *empty;

public:

    // SHMBB Constructor
    SHMBoundedBuffer(string name);

    // SHMBB Destructor
    ~SHMBoundedBuffer();

    void push(string msg);
    string pop();
};