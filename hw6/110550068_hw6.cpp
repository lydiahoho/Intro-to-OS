/*
Student No.: 110550068
Student Name: 王振倫
Email: asd4504230311@gmail.com
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not supposed to be posted to a 
public server, such as a public GitHub repository or a public web page. 
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <openssl/sha.h>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <utility>
#include <sstream>
#include <string>
using namespace std;
vector<pair<string, string>> vec;
string calculateSHA1(const string& filepath) {
    ifstream file(filepath, ios::binary);

    SHA_CTX shaContext;
    SHA1_Init(&shaContext);

    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    while (!file.eof()) {
        file.read(buffer, bufferSize);
        SHA1_Update(&shaContext, buffer, file.gcount());
    }
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1_Final(hash, &shaContext);

    ostringstream sha1Stream;
    sha1Stream << hex << setfill('0');
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        sha1Stream << setw(2) << static_cast<unsigned>(hash[i]);
    }
    return sha1Stream.str();
}

void processDirectory(const string& directoryPath) {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directoryPath.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            if (ent->d_type == DT_REG) {
                string filePath = directoryPath + "/" + ent->d_name;
                string sha1 = calculateSHA1(filePath);
                // cout << "File: " << filePath << ", SHA-1: " << sha1 << endl;
                bool flag = true;
                for (auto it = vec.begin(); it != vec.end(); ++it) {
                    if (sha1 == it->first) {
                        flag = false;
                        unlink(filePath.c_str());
                        link(it->second.c_str(), filePath.c_str());
                        break;
                    }
                }if(flag) vec.push_back({sha1, filePath});
            } else if (ent->d_type == DT_DIR && std::strcmp(ent->d_name, ".") != 0 && std::strcmp(ent->d_name, "..") != 0) {
                string subdirectoryPath = directoryPath + "/" + ent->d_name;
                processDirectory(subdirectoryPath);
            }
        }
        closedir(dir);
    }
}

int main(int argc, char *argv[]) {
    const string directoryPath = argv[1];
    processDirectory(directoryPath);
    return 0;
}
