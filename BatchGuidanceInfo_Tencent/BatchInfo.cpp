#define _CRT_SECURE_NO_WARNINGS
#include"BatchInfo.h"


CBatchInfo::CBatchInfo(string input)
{
	inputFilename = input;//��ȡ��ʼ�㾭γ������

}
CBatchInfo::~CBatchInfo()
{

}

	
void CBatchInfo::run()
{
	//��ȡ��ʼ�㾭γ�ȣ���д��OUTFILE�ļ�������RouteGuidanceTest��������������ļ��ɶԱ��ļ���

	ifstream fin;
	fin.open(inputFilename);
	char buff[BUFFERSIZE];
	int count = 0;//��ȡ�ļ�¼��
	
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

		//���½��̴���system���Խ��н��̼��ͬ������
			TCHAR szCommandLine[] = TEXT("RouteGuidanceTest.exe");
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));
			//����һ���½���  
			if (!CreateProcess(
				NULL,   //  ָ��һ��NULL��β�ġ�����ָ����ִ��ģ��Ŀ��ֽ��ַ���  
				szCommandLine, // �������ַ���  
				NULL, //    ָ��һ��SECURITY_ATTRIBUTES�ṹ�壬����ṹ������Ƿ񷵻صľ�����Ա��ӽ��̼̳С�  
				NULL, //    ���lpProcessAttributes����Ϊ�գ�NULL������ô������ܱ��̳С�<ͬ��>  
				false,//    ָʾ�½����Ƿ�ӵ��ý��̴��̳��˾����   
				0,  //  ָ�����ӵġ���������������ͽ��̵Ĵ����ı�  
				//  CREATE_NEW_CONSOLE  �¿���̨���ӽ���  
				//  CREATE_SUSPENDED    �ӽ��̴��������ֱ������ResumeThread����  
				NULL, //    ָ��һ���½��̵Ļ����顣����˲���Ϊ�գ��½���ʹ�õ��ý��̵Ļ���  
				NULL, //    ָ���ӽ��̵Ĺ���·��  
				&si, // �����½��̵������������ʾ��STARTUPINFO�ṹ��  
				&pi  // �����½��̵�ʶ����Ϣ��PROCESS_INFORMATION�ṹ��  
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
			system(command.c_str());//����������ļ�*/
			
			
		}
		else
		{
			cout << "��ȡ����" << endl;
			fin.close();
			return;
		}
		
	}
	cout << "����ļ�¼��" << count << endl;
	fin.close();
	return ;
	
}
