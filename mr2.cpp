#include <iostream>
#include <thread>


/*
 * 1. read chunks of data from file
 * 	have an offset to read from
 * 	this offset is sharead across threads & needs to be protected
 * 	thread uses this offset to read next chunk,
 * 	and updates the offset for the next thread to pick data
 * 2. search for Error 
 * 	search for the string error in the buffer and returns it's count
 * 	each thread can add it's count to it's own count
 * 	we can add all counts at the end to a global shared counter (protected)
 * 3. do this in parallel, until end is reached
 */

int nextOffset = 0;
int fileSize = 0;
mutex m;
vector<int> cnts;
fstream f;

#define READSIZE 1024
#define MAXTHREADS 4

int getOffset() {
	lock_guard<mutex> lg{m};
	//update the offset to read from
	auto offset = nextOffset;
	nextOffset += READSIZE;
	return offset;
}

int CountErrors(string s) {
	int cnt = 0;
	int nextpos = s.find("Error");
	while (nextpos != string::npos) {
		cnt += 1;
		nextpos = s.find("Error", nextpos + 1);
	}	
	return cnt;
}

void reader() {
	string s(READSIZE, '\0');
	int errCnt = 0;
	int myoffset = 0;
	while ((myoffset = getOffset()) < fileSize) {
		{
			lock_guard<mutex> lg{m};
			f.seekg((myoffset == 0) ? myoffset : myoffset - (string("Error").size() - 1));
			f.read(s.data(), (myoffset == 0) ? READSIZE : READSIZE + (string("Error").size() - 1));
		}
		s.resize(f.gcount());
		errCnt += CountErrors(s);
		s.resize(READSIZE);
	}
	lock_guard<mutex> lg{m};
	cnts.push_back(errCnt);
}

int main(int argc, char* argv[]) {
	//find file size
	f.open("input.txt");
	f.seekg(0, ios::end);
	fileSize = f.tellg();
	vector<thread> vt;

	for (int i = 0; i < MAXTHREADS; i++) {
		vt.emplace_back(thread(reader));
	}


	for (int i = 0; i < MAXTHREADS; i++) {
		vt[i].join();
	}

	auto total = accumulate(cnts.begin(), cnts.end(), 0);
}
