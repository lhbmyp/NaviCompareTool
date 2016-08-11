#define _CRT_SECURE_NO_WARNINGS
#include"BatchInfo.h"


CBatchInfo::CBatchInfo(string input)
{
	inputFilename = input;//读取起始点经纬度坐标

}
CBatchInfo::~CBatchInfo()
{

}

	
void CBatchInfo::run()
{
	//读取起始点经纬度，并写入OUTFILE文件并调用RouteGuidanceTest，并重命名输出文件成对比文件。

	ifstream fin;
	fin.open(inputFilename);
	char buff[BUFFERSIZE];
	int count = 0;//读取的记录数
	
	while (fin.getline(buff, BUFFERSIZE))
	{
		++ count;
		double startLongitude = 0.0;
		double startLatitude = 0.0;
		double endLongitude = 0.0;
		double endLatitude = 0.0;
		int ret = sscanf(buff, "%lf %lf %lf %lf", &startLongitude, &startLatitude, &endLongitude, &endLatitude);
		if (ret == 4)
		{
			
			ofstream  fout;
			fout.open(OUTFILE);
			char str[BUFFERSIZE];
			sprintf(str, "%.7lf, %.7lf, 5.000000, 169.453125, 6.886998, 1403094622.500106", startLongitude, startLatitude);
			cout << str << endl;
			fout << str << endl;
			sprintf(str, "%.7lf, %.7lf, 5.000000, 169.453125, 6.886998, 1403094622.500106", endLongitude, endLatitude);
			cout << str << endl;
			fout << str << endl;
			fout.close();

		//用新进程代替system可以进行进程间的同步处理。
			TCHAR szCommandLine[] = TEXT("RouteGuidanceTest.exe");
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));
			//创建一个新进程  
			if (!CreateProcess(
				NULL,   //  指向一个NULL结尾的、用来指定可执行模块的宽字节字符串  
				szCommandLine, // 命令行字符串  
				NULL, //    指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。  
				NULL, //    如果lpProcessAttributes参数为空（NULL），那么句柄不能被继承。<同上>  
				false,//    指示新进程是否从调用进程处继承了句柄。   
				0,  //  指定附加的、用来控制优先类和进程的创建的标  
				//  CREATE_NEW_CONSOLE  新控制台打开子进程  
				//  CREATE_SUSPENDED    子进程创建后挂起，直到调用ResumeThread函数  
				NULL, //    指向一个新进程的环境块。如果此参数为空，新进程使用调用进程的环境  
				NULL, //    指定子进程的工作路径  
				&si, // 决定新进程的主窗体如何显示的STARTUPINFO结构体  
				&pi  // 接收新进程的识别信息的PROCESS_INFORMATION结构体  
				))
			{
				return;
			}

			// Wait until child process exits.
			WaitForSingleObject(pi.hProcess, INFINITE);

			// Close process and thread handles.
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);


			/*
			char buff[512];
			sprintf(buff,"t%d.txt",count);
			string filename(buff);
			command = "copy t.txt " + filename;
			system(command.c_str());//重命名输出文件*/
			
			
		}
		else
		{
			cout << "读取错误" << endl;
			fin.close();
			return;
		}
		
	}
	cout << "处理的记录数" << count << endl;
	fin.close();
	return ;
	
}
