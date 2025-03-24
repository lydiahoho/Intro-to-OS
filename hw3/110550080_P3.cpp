/*
Student No.: 110550080
Student Name: Hsiao-Hsien Ho
Email: lydiaho.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not 
supposed to be posted to a public server, such as a 
public GitHub repository or a public web page
*/
#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <queue>

using namespace std;
float rec[8];
vector<int> visit;
int data[1000015];
int raw_data[1000015];
sem_t job_list_lock;
sem_t visit_lock;
sem_t arrival;
sem_t completion;
struct Arg { //each task
    int id, from, to, mid;
};

Arg args[16];
queue<Arg> job_list;


void bubbleSort(int from, int to) {
    for (int j = 0; j < (to - from); j++) {
        for (int i = from; i < to-j; i++) {
            if (data[i] > data[i + 1]) {
                swap(data[i], data[i + 1]);
            }
        }
    }
    return;
}


void merge(int from, int mid, int to) {
    int leftSize = mid - from + 1;
    int rightSize = to - mid;
    int left[leftSize + 1]; 
    int right[rightSize + 1]; 
    for (int i = 0; i < leftSize; ++i) {
        left[i] = data[from + i];
    }
    for (int i = 0; i < rightSize; ++i) {
        right[i] = data[mid + 1 + i];
    }
    left[leftSize] = INT_MAX; 
    right[rightSize] = INT_MAX; 

    int i = 0, j = 0;
    for (int k = from; k <= to; ++k) {
        if (left[i] <= right[j]) {
            data[k] = left[i++];
        } else {
            data[k] = right[j++];
        }
    }
}


void create_job(int i){
    int id;
    if(i==1){id=9;} else if(i==3){id=10;}
    else if(i==5){id=11;}else if(i==7){id=12;}
    else if(i==9){id=13;}else if(i==11){id=14;}
    else if(i==13){id=15;}
    //cout<<"i="<<i<<" gen "<<id<<endl;
    args[id].id = id;
    args[id].from = args[i].from;
    args[id].to = args[i+1].to;
    args[id].mid = args[i].to;
    sem_wait(&job_list_lock);
    job_list.push(args[id]);
    sem_post(&job_list_lock);
}

void* worker_thread(void* arg) {
    // printf("dooo\n");
    while (true) {
        sem_wait(&job_list_lock);
        if (job_list.empty()) {
            sem_post(&job_list_lock);
            break; 
        }
        Arg task = job_list.front();
        job_list.pop();
        sem_post(&job_list_lock);
        int i = task.id;
        if (task.from < task.to) {
            if (task.id <= 8) {
                bubbleSort(task.from, task.to);
                sem_wait(&visit_lock);
                visit[task.id] = 1; 
            } else {
                merge(task.from, task.mid, task.to);
                sem_wait(&visit_lock);
                visit[task.id] = 1;
            }
            if((i % 2 == 1) && (visit[i] == 1) && (visit[i+1] == 1)){   //1 3 5 7 9...
                visit[i] =0;  visit[i+1] =0;
                sem_post(&visit_lock);
                create_job(i);
                continue;   
            }else if((i % 2 == 0) && (visit[i] == 1) && (visit[i-1] == 1)){  //2 4 6 8 10...
                visit[i] =0;  visit[i-1] =0;
                sem_post(&visit_lock);
                create_job(i-1);
                continue;    
            }else{
                sem_post(&visit_lock);
                continue; 
            }
        }
        
    }
    return nullptr;
    
}

int main(){
    ifstream file("input.txt");
    int num, value, data_index=0;
    file >> num;
    while (file >> value && data_index < 1000000) {
        data[data_index] = value;
        raw_data[data_index] = value;
        data_index++;
    }
    file.close();
    int s = num/8 -1;
    args[0].to = -1;
    for(int i = 1; i <=8; i++){
        args[i].id = i;
        args[i].from = args[i-1].to + 1;
        args[i].to = args[i].from + s;
        if(i==8){args[i].to += num%8;}
    }

    // thtead start
    for(int n_th = 1; n_th <= 8; n_th++){
        struct timeval time1, time2;
        visit = vector<int>(16, 0);
        sem_init(&job_list_lock, 0, 1);
        sem_init(&visit_lock, 0, 1);
        for(int i = 1; i <=8; i++){
            job_list.push(args[i]);
        }
        pthread_t thread[n_th];
        gettimeofday(&time1, 0);
        for (int i = 0; i<n_th; i++) {
            pthread_create(&thread[i], NULL, worker_thread, NULL);
        }
        //pthread_exit(NULL);
        for (int i = 0; i < n_th; i++) {
            pthread_join(thread[i], NULL);
        }
        gettimeofday(&time2, 0);
        sem_destroy(&job_list_lock);
        sem_destroy(&visit_lock);
        int sec = time2.tv_sec - time1.tv_sec;
        int usec = time2.tv_usec - time1.tv_usec;
        printf("worker thread #%d,", n_th);
        printf(" elapsed %f ms\n", (sec*1000)+(usec/1000.0));
        rec[n_th-1] = sec*1000+(usec/1000.0);
        const string filename = "output_" + to_string(n_th) + ".txt";

        ofstream outfile(filename);
        for (size_t i = 0; i < num; ++i) {
            outfile << data[i];
            if (i < num - 1) {
                outfile << " ";
            }
        }
        outfile.close();
       
        memcpy(data, raw_data, sizeof(raw_data));
        while (!job_list.empty()) job_list.pop();
    }
    // for(int i=0; i<8; i++){printf("%f, ", rec[i]);}
}