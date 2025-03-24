/*
Student No.: 110550080
Student Name: Hsiao-Hsien Ho
Email: lydiaho.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not 
supposed to be posted to a public server, such as a 
public GitHub repository or a public web page
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <bits/stdc++.h>
#include <vector>
using namespace std;

struct Node {
    int val;
    int freq;
    int seq;
    int idx;
};

struct lru {
    int number;
    lru *prev;
    lru *next;
};

#define hash_num 10485

int main(int argc, char *argv[]) {
    //Read file
    if (argc != 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        return 1;
    }
    FILE *file;
    struct timeval time1, time2;
    int sec, usec;

    //LFU
    gettimeofday(&time1, 0);
    cout<<"LFU policy:"<<endl;
    cout<<"Frame\tHit\t\tMiss\t\tPage fault ratio"<<endl;
    for (int frame = 64 ; frame <= 512 ; frame*=2){
        vector <Node*> table[hash_num]; 
        vector <Node*> f_table;
        f_table.resize(frame);
        Node *cur = NULL;   
        int hit = 0, miss = 0, input, table_size = 0, total_num = 0;
        file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Error opening file!\n");
            return 1;
        }
        while (fscanf(file, "%d", &input) == 1) {
            total_num++;
            int idx = input % hash_num;
            bool find = false;
            for(int i=0; i<table[idx].size(); i++){   
                if(input == table[idx][i] -> val){
                    //cout<<idx<<" "<<i<<endl;
                    f_table[table[idx][i] -> idx] -> freq ++;
                    f_table[table[idx][i] -> idx] -> seq = total_num;
                    hit++;
                    find = true;
                    break;
                }
            }
            if(!find){
                miss++; 
                cur = new Node; 
                if(table_size < frame){
                    //have space
                    cur -> val = input; 
                    cur -> freq = 1;
                    cur -> seq = total_num;
                    cur -> idx = table_size; 
                    f_table[table_size] = cur;
                    table[idx].push_back(cur);
                    table_size++;       
                }
                else{
                    //full delete
                    int min = 0; 
                    for (int i = 1; i < frame; ++i) {
                        if (f_table[i] -> freq == f_table[min] -> freq){
                            if (f_table[i] -> seq < f_table[min] -> seq){
                                min = i;
                            }
                        }else if(f_table[i] -> freq < f_table[min] -> freq){
                            min = i;
                        }
                        
                    }
                    int rm = f_table[min] -> val % hash_num;
                    Node *tail = f_table[min];
                    table[rm].erase(remove_if(table[rm].begin(), table[rm].end(),[tail](Node* ptr) {return ptr == tail;}),table[rm].end());
                    cur -> val = input; 
                    cur -> freq = 1;
                    cur -> seq = total_num;
                    cur -> idx = min; 
                    f_table[min] = cur;
                    table[idx].push_back(cur);
 
                }
            }
            
        }
        fclose(file);
        printf("%d\t%d\t\t%d\t\t%.10f\n", frame, hit, miss, (miss*1.0/total_num));    
    }
    gettimeofday(&time2, 0);
    sec = time2.tv_sec - time1.tv_sec;
    usec = time2.tv_usec - time1.tv_usec;
    printf("Total  elapsed time %.4f sec\n\n", (sec)+(usec/1000000.0));


    //LRU
    gettimeofday(&time1, 0);
    cout<<"LRU policy:"<<endl;
    cout<<"Frame\tHit\t\tMiss\t\tPage fault ratio"<<endl;
    for (int frame = 64 ; frame <= 512 ; frame*=2){
        vector <lru*> rtable[hash_num];
        lru *head = NULL, *tail = NULL, *cur = NULL;
        int hit = 0, miss = 0, input, table_size = 0, total_num = 0;
        file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Error opening file!\n");
            return 1;
        }
        while (fscanf(file, "%d", &input) == 1) {
            total_num++;
            int idx = input % hash_num;
            //cout<<idx<<"\t";
            bool find = false;
            for(int i=0; i<rtable[idx].size(); i++){
                //cout<<i;
                cur = NULL;    
                if(input == rtable[idx][i] -> number){
                    //cout<<idx<<" "<<i<<endl;
                    hit++;
                    find = true;
                    cur = rtable[idx][i];
                    if(cur == head) {
                        break;
                    }else{
                        cur -> prev -> next = cur -> next;
                        //if it is tail 
                        if(cur == tail){
                            tail = tail -> prev;
                            tail -> next = NULL;
                        }else {
                            cur -> next -> prev = cur -> prev;
                        }
                        //move to head
                        cur -> prev = NULL;
                        cur -> next = head;
                        head -> prev = cur;
                        head = cur;
                        break;
                    }      
                }
            }
            if(!find){
                miss++;
                cur = new lru; 
                if(table_size < frame){
                    //have space
                    table_size++; 
                    cur -> number = input;
                    cur -> prev = NULL;
                    cur -> next = head;
                    if(head!=NULL)  head -> prev = cur;
                    head = cur;  
                    rtable[idx].push_back(cur);  
                    if(table_size == 1) {tail = cur; tail -> next = NULL;}
                    if(table_size == 2) {tail -> prev = head;}     
                }
                else{
                    //full delete 
                    int rm = (tail -> number) % hash_num;
                    rtable[rm].erase(remove_if(rtable[rm].begin(), rtable[rm].end(),[tail](lru* ptr) { return ptr == tail; }),rtable[rm].end());
                    tail = tail -> prev;
                    tail -> next = NULL;
                    //add
                    cur -> number = input;
                    cur -> prev = NULL;
                    cur -> next = head;
                    head -> prev = cur;
                    head = cur;  
                    rtable[idx].push_back(cur);  
                }
            }
            
        }
        fclose(file);
        printf("%d\t%d\t\t%d\t\t%.10f\n", frame, hit, miss, (miss*1.0/total_num));    
    }
    gettimeofday(&time2, 0);
    sec = time2.tv_sec - time1.tv_sec;
    usec = time2.tv_usec - time1.tv_usec;
    printf("Total  elapsed time %.4f sec\n", (sec)+(usec/1000000.0));
   
}
