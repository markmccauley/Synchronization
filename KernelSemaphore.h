#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#include "RequestChannel.h"

class KernelSemaphore {
private:

    int semid;

public:

    // KernelSemaphore constructor
    KernelSemaphore(int _val, key_t key);

    // KernelSemaphore destructor
    ~KernelSemaphore();

    void P(); /* Acquire lock */
    void V(); /* Release lock */
};