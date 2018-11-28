#include "KernelSemaphore.h"

KernelSemaphore::KernelSemaphore(int _val, key_t key){
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid != -1) { // prevent _val from being 0
        sembuf sb = {0, _val, 0}; // if _val is 0 semop will lock
        semop(semid, &sb, 1);
    } else {
        semid = semget(key, 1, IPC_CREAT | 0666);
    }
}

KernelSemaphore::~KernelSemaphore(){
    semctl(semid, IPC_RMID, 0);
}

void KernelSemaphore::P(){
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

void KernelSemaphore::V(){
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}