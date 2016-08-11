#include<iostream>
#include<fstream>
#include<iomanip>
#include<cstdlib>
#include<ctime>

using namespace std;

#define OUTFILE "RANDLNGLAT"

static int sd = 0;
void getLngLat(double& lng,double& lat)
{
	while (1)
	{
		//srand((unsigned int)time(NULL) *rand());
		srand(sd++);
		lng = (double)rand() / RAND_MAX + 116.1090088;
		if (lng > 116.6267395)
		{
			continue;
		}
		else
		{
			lat = (double)rand() / RAND_MAX + 39.6934508;
			while (lat > 40.1505379)
			{
				lat = (double)rand() / RAND_MAX + 39.6934508;

			}
			//¶¼·ûºÏ
			return;
		}
	}
	

}
int main(int argc,char** argv)
{
	ofstream fout(OUTFILE);
	int i = 0;
	while (i++<10000)
	{
		double st_lng=0.0, st_lat=0.0, end_lng=0.0, end_lat=0.0;
		getLngLat(st_lng, st_lat);
		getLngLat(end_lng, end_lat);
		fout << setprecision(9) << st_lng << " " << st_lat << " " << end_lng<<" "<< end_lat<<endl;
		
	}
	fout.close();

	
	return 0;
}


