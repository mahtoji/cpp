#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <string>
#include <map>

using namespace std;

/*
 * 1. define number of threads
 * 2. define amount of bytes to process
 * 3. track next pointer to next area to process
 * 4. threads that process are consumers (picking & processing work)
 * 5. thread that hands out work (tracks next chunk to process), is the producer
 * 6. we need synchroniztion, so work is handed out in an orderly manner
 * 7. the offset is the sharead resource & needs protection
 * 8. reading of file can be shared as long as independent regions are read
 * 9. mmap of file will make it more efficient since kernel can map regions of file to memory without needing repeated FS system calls
 */
 
int offset = 0;
thread_local map<string, int> kwc = {{"Error", 0}, {"Warning", 0}, {"Critical", 0}, {"Major", 0}, {"Minor", 0}};
vector<map<string, int>> vm;//vector of maps
mutex m;
mutex kwl;
#define MAX_THREADS 4
#define CHUNK_SIZE 1024
int FileSize;


void CountErrorStr(string& s) {
	for (auto& [kw, _] : kwc) {
		int nextpos = 0;
		nextpos = s.find(kw, nextpos);
		while (nextpos != string::npos) {
			kwc[kw] += 1;
			nextpos += kw.size();
			nextpos = s.find(kw, nextpos);
		}
	}
}

void Consumer() {
	fstream f("input.txt");
	string str(CHUNK_SIZE + 1, '\0');
        while (true) {
		{
			lock_guard<mutex> lg(m); 
			auto nextoffset = offset;
			if (nextoffset >= FileSize) break;
			f.seekg(nextoffset);
			offset += CHUNK_SIZE;
		}
		f.read(&str[0], CHUNK_SIZE);
		auto rc = f.gcount();//read count
		if (rc < CHUNK_SIZE) {
			auto s = str;
			str = s.substr(0, rc);
		}
		CountErrorStr(str);
	}
	lock_guard<mutex> lg(kwl);
	vm.push_back(kwc);
}

int main(int argc, char* argv[]) {
	fstream f("input.txt");
	f.seekg(0, ios::end);
	FileSize = f.tellg();
	f.seekg(0, ios::beg);
	vector<thread> v;
	for (int i = 0; i < MAX_THREADS; i++) {
		v.push_back(thread(Consumer));
	}
	for (int i = 0; i < MAX_THREADS; i++) {
		v[i].join();
	}
	//create one final kw map
	map<string, int> kwm;
	for (auto& e : vm) {
		for (auto& [k, v] : e) {
			kwm[k] += v;
		}
	}
	for (auto& [k,v] : kwm) cout << k << " " << v << endl;
}
