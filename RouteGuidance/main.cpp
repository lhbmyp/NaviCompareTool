#include"Guidance.h"
#define GUIDANCECOMPARISON "result"//���ȽϽ��������ļ���
int main(int argc, char** argv)
{
	if (argc < 6)
	{
		cerr << "Usage: "<<argv[0]<<"<tencent_filename> <gaode_filename> <mapconfigure_filename> <TXRouteFile> <GDRouteFile>"<<endl;
		getchar();
		return 1;
	}
	CGuidance g(argv[1], argv[2], argv[3], argv[4], argv[5], GUIDANCECOMPARISON);//������������ļ���Ҫ�޸�
	//CGuidance g("t8.txt", "g8.txt", "configure.txt", GUIDANCECOMPARISON);//������������ļ���Ҫ�޸�

/*	
	*�������յ���Ϣ�ĶԱ�
	if (g.init())
	{
		g.compareMapGuidance();
		g.showDifference();
		g.ComparisonToJson();
	//	g.ComparisonToGeoJson();//��ʱ������GeoJson
	}
	*/

	//���������յ���Ϣ
	g.findDifference();//������
	return 0;
}