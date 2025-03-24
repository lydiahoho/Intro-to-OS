/*
Student No.: 110550080
Student Name: Lydia Ho
Email: lydiaho.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <sys/wait.h>
using namespace std;

void Exe(vector<string> line) {
    char* args[line.size() + 1];
    for (int i = 0; i<line.size(); i++) {
        args[i] = strdup(line[i].c_str());
    }
    args[line.size()] = nullptr; 
    //cout<<line[0]<<endl;
    execvp(args[0], args);
    perror("execvp");
    exit(0);
}

int main(){
    
    while(true){
        cout<<">";
        //dup2(STDOUT_FILENO,STDOUT_FILENO );
        string command,word;
        int type=0 ,i = 0;
        getline(cin, command);
        int sdout = dup(STDOUT_FILENO);
        int sdin = dup(STDIN_FILENO);
        vector<string> vec[2];
        stringstream ss(command); 

        while (getline(ss, word, ' ')){
            if(word == "&"){
                type = 1;
                continue;}
            else if(word == ">" ) {
                type = 2;
                i = 1;
                continue;}
            else if (word == "|" ) {
                type = 3;
                i = 1;
                continue;}
            else{vec[i].push_back(word);}
        }
        //cout<<22<<type<<endl;
        pid_t pid1, pid2;
        switch(type) {
            case 1:    //&
                signal (SIGCHLD,SIG_IGN);
                pid1 = fork();
                if (pid1 == 0) { /* child process */
                    Exe(vec[0]);}
                break;
            case 2:    //I/O redirection
                pid1 = fork();  
                if (pid1 == 0){
                    const char* file = vec[1][0].c_str();
                    int f = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
                    close(STDOUT_FILENO);
                    dup2(f, STDOUT_FILENO); 
                    close(f);
                    Exe(vec[0]);
                    }    
                else{
                    wait(NULL);    
                }
                break;

            case 3:    //pipe
                int fd[2];
                pipe(fd);   
                pid1 = fork();
                if (pid1 == 0) { /* child1 */
                    close(fd[0]);
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[1]);
                    Exe(vec[0]);}
                    
                else { /* child2 process */
                    pid2 = fork();
                    if (pid2 == 0) { 
                        close(fd[1]);
                        dup2(fd[ 0], STDIN_FILENO);
                        close(fd[0]);
                        Exe(vec[1]); }   
                    else { /* parent process */
                    close(fd[0]);
                    close(fd[1]);
                    wait(NULL);}
                }
                break;

            default:    //normal
                pid1 = fork();
                if (pid1 < 0) { /* error occurred */
                    fprintf(stderr, "Fork Failed");
                    exit(-1);}
                else if (pid1 == 0) { /* child process */
                    Exe(vec[0]);}
                else { /* parent process */
                    wait(NULL);}
                break;
        }
        //vec[0].clear();
        //vec[1].clear();
        dup2(sdin, STDIN_FILENO);
        dup2(sdout, STDOUT_FILENO);
        close(sdin);
        close(sdout);
        
    }   
}