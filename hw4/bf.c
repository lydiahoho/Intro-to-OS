/*
Student No.: 110550080
Student Name: Hsiao-Hsien Ho
Email: lydiaho.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not 
supposed to be posted to a public server, such as a 
public GitHub repository or a public web page
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>


struct block {  //size 32
    size_t size;
    int free;
    struct block *prev;
    struct block *next;
};

void *pointer = NULL;   
struct block *head;

void *mem = NULL;  


void *malloc(size_t size){
    if(pointer == NULL){
        pointer = mmap(NULL, 20000, PROT_READ | PROT_WRITE, MAP_ANON| MAP_PRIVATE, -1, 0);
        if(pointer == MAP_FAILED){
            perror("MMAP Allocation fail.");
            exit(1);
        }
        head = pointer;
        head -> size = 20000-32;
        head -> free = 1;
        head -> prev = NULL;
        head -> next = NULL;
    }
    struct block *cur, *best;
    
    if(size == 0){
        cur = head;
        size_t max = 0;
        while ( cur != NULL){
            if (cur -> free) {
                if (cur -> size > max) {
                    max = cur -> size;
                }
            }
            cur = cur -> next;
        }

        char output[40]; 
        sprintf(output, "Max Free Chunk Size = %ld\n", max);
        write(STDOUT_FILENO, output, strlen(output));
        munmap(pointer, 20000);
		return 0;
    }
    
    int n = size / 32;
    if (size % 32) {n++;}
    size_t need = 32* n, bs = 20000;
    cur = head;

    while ( cur != NULL){
        if (cur -> free) {
            if(cur -> size == need ){
                cur -> free = 0;
                return (void*)((char*)cur + 32);
            }
            else if (cur -> size > need){
                if((cur -> size) < bs){
                    best = cur;
                    bs = cur -> size;   
                }
            }
        }
        cur = cur -> next;
    }
    
    if (bs == 20000) return 0;
    
    struct block *nxt; 
    nxt = (struct block *)((char*)best + 32 + need); 
    nxt -> free = 1;
    nxt -> size = best -> size - 32 - need;
    nxt -> prev = best ; 
    nxt -> next = best -> next ; 
    if (best->next){
        best -> next -> prev = nxt;
    }
    best -> free = 0;
    best -> size = need;
    best -> next = nxt;
    return (void*)((char*)best + 32) ; 

}


void free(void *ptr){
    struct block * cur;
    cur = ptr;
    //cur = cur - 1;
    cur = (struct block *)(ptr - sizeof(struct block));
    cur -> free = 1;
    if(cur -> prev != NULL && cur -> prev -> free){
        cur -> prev -> next = cur -> next; 
        cur -> prev -> size += cur -> size + 32;  
        cur = cur -> prev;
        if(cur -> next != NULL){    //12 30.
            cur -> next -> prev = cur;
        }

    }
    else if(cur -> next != NULL && cur -> next -> free){   //3 free or 2 free
        cur -> next -> prev = cur -> prev; 
        cur -> next -> size += cur -> size + 32;
        cur = cur -> next; 
        if(cur -> prev != NULL){    //prev in use
            cur -> prev -> next = cur;
        }
    }

}

