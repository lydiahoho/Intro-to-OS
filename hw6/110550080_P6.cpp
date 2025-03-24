/*
Student No.: 110550080
Student Name: Hsiao-Hsien Ho
Email: lydiaho.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not 
supposed to be posted to a public server, such as a 
public GitHub repository or a public web page
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <openssl/sha.h>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <iomanip>

using namespace std;
unordered_map<string, string> hashToFileMap;
int cnt =0 ;

string calculateSHA1(const string& filepath) {
    ifstream file(filepath, ios::binary);
    if (!file.is_open()) {
        return ""; // Unable to open file
    }

    SHA_CTX sha1Context;
    SHA1_Init(&sha1Context);

    const size_t bufSize = 1024;
    char buffer[bufSize];
    while (!file.eof()) {
        file.read(buffer, bufSize);
        SHA1_Update(&sha1Context, buffer, file.gcount());
    }
    
    file.close();
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1_Final(hash, &sha1Context);

    stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }
    // cout<<ss.str()<<endl;
    return ss.str();
}


void deduplicateFiles(const string& directory) {

    DIR* dir = opendir(directory.c_str());
    if (dir == nullptr) {
        cerr << "Error opening directory.\n";
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;
        string filepath = directory + "/" + filename;
       
        struct stat fileStat;
        if (stat(filepath.c_str(), &fileStat) < 0) {
            continue;
        }

        if (!S_ISREG(fileStat.st_mode)) {
            if (filename != "." && filename != "..") {
                string subDirectory = directory + "/" + filename; 
                deduplicateFiles(subDirectory); 
            }
            continue; 
        }

        string hash = calculateSHA1(filepath);
     

        if (!hash.empty()) {
            if (hashToFileMap[hash].empty()) {
                hashToFileMap[hash] = filepath;
            } else {
                const string& existingFilePath = hashToFileMap[hash];
                if (access(existingFilePath.c_str(), F_OK) != -1) {
                    if (unlink(filepath.c_str()) != 0) {
                        cout<<"unlink failed"<<endl;
                    }   
                }
                int result = link(existingFilePath.c_str(), filepath.c_str()); // Create hard link
                //cout<<filepath<<" "<<hash<<endl;
                if (result != 0) {
                    cerr << "Link failed between " << existingFilePath << " and " << filepath << endl;
                }
            }
        }
    }
    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <directory_path>\n";
        return 1;
    }
    string directory = argv[1];
    struct timeval time1, time2;
    gettimeofday(&time1, 0);
    deduplicateFiles(directory);
    gettimeofday(&time2, 0);
    int sec = time2.tv_sec - time1.tv_sec;
    int usec = time2.tv_usec - time1.tv_usec;
    printf("Elapsed %f sec,\n", sec+(usec/1000000.0));
    return 0;
}
