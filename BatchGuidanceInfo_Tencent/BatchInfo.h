#ifndef _CBatchInfo_Header
#define _CBatchInfo_Header

#include<iostream>
#include<fstream>
#include<string>

#include<Windows.h>
#include <tchar.h>
using namespace std;


#define OUTFILE "fortest.txt"  
#define BUFFERSIZE 1024

class CBatchInfo
{
public:
	CBatchInfo(string input);
	virtual ~CBatchInfo();
public:
	void run();
private:
	string inputFilename;
};
#endif