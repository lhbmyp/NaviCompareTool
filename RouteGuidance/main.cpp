#include"Guidance.h"
#define GUIDANCECOMPARISON "result"//将比较结果输出到文件中
int main(int argc, char** argv)
{
	if (argc < 6)
	{
		cerr << "Usage: "<<argv[0]<<"<tencent_filename> <gaode_filename> <mapconfigure_filename> <TXRouteFile> <GDRouteFile>"<<endl;
		getchar();
		return 1;
	}
	CGuidance g(argv[1], argv[2], argv[3], argv[4], argv[5], GUIDANCECOMPARISON);//批量输入输出文件需要修改
	//CGuidance g("t8.txt", "g8.txt", "configure.txt", GUIDANCECOMPARISON);//批量输入输出文件需要修改

/*	
	*处理单个诱导信息的对比
	if (g.init())
	{
		g.compareMapGuidance();
		g.showDifference();
		g.ComparisonToJson();
	//	g.ComparisonToGeoJson();//暂时不考虑GeoJson
	}
	*/

	//批量处理诱导信息
	g.findDifference();//待测试
	return 0;
}