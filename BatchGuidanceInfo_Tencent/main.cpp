#define _CRT_SECURE_NO_WARNINGS

#include"BatchInfo.h"
#include<Windows.h>
#include <tchar.h>



#define URL2LNGLAT "LngLatFile" 
int _server2X(double x)
{
	//处理经度
	return (int)(x / 1.11319490777777778 + .5);
}

int _server2Y(double y)
{
	//处理纬度
	double dblMercatorLat = y / 111319.490777777778;
	return (int)((atan(exp(dblMercatorLat*0.017453292519943295769236907684886)) / 0.0087266462599716478846184538424431 - 90)*100000.0 + .5);
}



int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cerr << "Usage: "<<argv[0]<<" <inputfile>" << endl;
	getchar();
		return 1;
	}

/*	double server_startLng = 0.0;//起点经度
	double server_startLat = 0.0;//起点纬度
	double server_endLng = 0.0;
	double server_endLat = 0.0;
	string LngLatFile;
	ifstream fin;
	fin.open(argv[1]);
	//fin.open("urls.27");
	ofstream fout;
	fout.open(URL2LNGLAT);//应该检查打开状态


	char buff[BUFFERSIZE];
	while (fin.getline(buff, BUFFERSIZE))
	{
		//批量产生经纬度坐标
		string urlstr(buff);
		//cout << buff << endl;
		int pos= urlstr.find("start");
		if (pos != string::npos)
		{
			string str_st=urlstr.substr(pos);
			sscanf(str_st.c_str(), "start=1$$$$%lf,%lf", &server_startLng, &server_startLat);
		//	cout << server_startLng << "   " << server_startLat << endl;
			int destpos = str_st.find("dest");
			if (destpos != string::npos)
			{

				string str_dest = str_st.substr(destpos);
				sscanf(str_dest.c_str(), "dest=1$$$$%lf,%lf", &server_endLng, &server_endLat);
				//都获取成功
				
				char lnglat_buff[BUFFERSIZE];
				sprintf(lnglat_buff, "%f %f %f %f\n",
					_server2X(server_startLng) / 100000.0, _server2Y(server_startLat) / 100000.0, 
					_server2X(server_endLng) / 100000.0, _server2Y(server_endLat) / 100000.0);
			//	cout << server_endLng << "   " << server_endLat << endl;
				fout << lnglat_buff;
				
			}
			else
			{
				continue;
			}
		}
		else
		{
			continue;
		}

	}
	fin.close();
	fout.close();
	*/
	CBatchInfo batchinfo(URL2LNGLAT);
	batchinfo.run();
	return 0;
}