/*
Student No.: 110550080
Student Name: Hsiao-Hsien Ho
Email: lydiaho.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not 
supposed to be posted to a public server, such as a 
public GitHub repository or a public web page
*/
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>

//typedef int uint32_t;
float rec[16];

int main(){
    int dimension;
    printf("Input the matrix dimension:");
    scanf("%d", &dimension);
    int shmid = shmget(1, dimension*dimension*sizeof(uint32_t),  IPC_CREAT | 0600);
    if (shmid == -1) {
        perror("shmget fail");
        exit(1);
    }
    uint32_t *p = shmat(shmid, NULL, 0);
    if (p == (uint32_t *)-1) {
        perror("shmat fail");
        exit(1);
    }
    uint32_t matrix[dimension][dimension];
    for (int i = 0; i < dimension; i++){
        for (int j = 0; j < dimension; j++){
            matrix[i][j] = i *dimension + j ; 
        }
    }
    struct timeval time1, time2;
    // process
    for(int nf = 1 ; nf <= 16 ; nf++){    // how many processes
        gettimeofday(&time1, 0);
        int num = dimension/nf;
        int big = dimension%nf;
        int start=0, end=-1;
        for(int fi = 1; fi<= nf; fi++){   // the nth part of process  
            pid_t pid = fork();
            if(pid == 0){
                if(big > 0){
                    start = end + 1;
                    end = start + num;
                    big--; }
                else{
                    start = end + 1;
                    end = start + num -1; }
                
                for(int i = start; i<= end ; i++){
                    for(int j=0; j<dimension; j++){
                        int tmp = 0;
                        for(int k=0; k<dimension; k++){
                            tmp += matrix[i][k]* matrix[k][j];
                        }
                        p[i*dimension+j] = tmp;
                    }
                }
                exit(0);
            }
        }
        int wpid,ch;
        while ((wpid = wait(&ch)) > 0);
        gettimeofday(&time2, 0);
        int sec = time2.tv_sec - time1.tv_sec;
        int usec = time2.tv_usec - time1.tv_usec;
        uint32_t cks = 0;
        for(int i=0; i<dimension*dimension; i++){
            cks += p[i];
        }
        rec[nf-1] = sec+(usec/1000000.0);
        if(nf ==1 )
            printf("Multiplying matrices using %d process\n", nf);
        else
            printf("Multiplying matrices using %d processes\n", nf);
        printf("elapsed %f sec, Checksum: %u \n", sec+(usec/1000000.0), cks);
    }
    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    //for(int i=0; i<16; i++){printf("%f, ", rec[i]);}
    
}