#ifndef _HEADER_GUIDANCE
#define _HEADER_GUIDANCE

#include<iostream>
#include<iomanip>
#include<fstream>
#include<map>
#include<vector>
#include<string>
#include<cmath>
using namespace std;



#define LINE_LENGTH 1000

#define MAXSIZE 1024


#define GUIDANCECOMPARISON_GEOJSON "Geo_result"
/**
* google maps的脚本里代码
*/
const double EARTH_RADIUS = 6378.137;
#define PI  3.14159265359	

#define PRECISION 3.0
struct GuidanceInfo
{
	int ID;
	double Longitude;//经度
	double Latitude;//纬度
	
};
struct point
{
	double Longitude;//经度
	double Latitude;//纬度
};
struct TXRoutePoint
{
	int x;//经度*100000
	int y;//纬度*100000
};
class CGuidance
{
	private:
		vector<GuidanceInfo> v_tencentGuidanceInfo;//IN 存放腾讯地图的诱导
		vector<GuidanceInfo> v_gaodeGuidanceInfo;//IN 存放高德地图的诱导
		multimap<int, int> m_GuidnaceMap;//两个地图诱导信息的映射
		string m_tencentFile;//保存输入文件路径 IN
		string m_gaodeFile;	//IN 
		string m_mapconfigFile;//映射配置文件 IN
		string m_outFile;//比较结果的输出文件名 OUT

		string m_tencentRouteFile;//路线信息文件 IN
		string m_gaodeRouteFile;
	private:
		string mStartLatLng_t;//腾讯诱导信息中起点经纬度
		string mEndLatLng_t;//终点经纬度
		string mStartLatLng_g;
		string mEndLatLng_g;

		
		
	private:
		vector<GuidanceInfo> v_tecentMoreInfo;//OUT 相比于高德，腾讯多出的诱导图标ID
		vector<GuidanceInfo> v_gaodeMoreInfo;//OUT 相比于腾讯,高德多出的诱导图标ID

		vector<GuidanceInfo> v_tecentDiffInfo;//OUT 相比于高德，腾讯多出的诱导图标ID
		vector<GuidanceInfo> v_gaodeDiffInfo;//OUT 相比于腾讯,高德多出的诱导图标ID

	public:
		CGuidance(string tencentFilename, string X, string configureFilename, string Route_T, string Route_G, string resultFilename);
		~CGuidance();
	public:
		bool init();//将两个地图的诱导信息读入到容器中。
		void compareMapGuidance();//比较两个地图的诱导信息的不同
		double rad(double d);//计算距离的工具函数
		double GetDistance(double lat1, double lng1, double lat2, double lng2);//计算两个点的距离
		bool isSameEvent(double lat1, double lng1, double lat2, double lng2);//判断事件点是否一样

		string gettextFromTencentByID(int guidanceid);
		string gettextFromGaodeByID(int guidanceid);

		bool getGuidanceInfo(string file, vector<GuidanceInfo>& v);//初始化函数调用该函数获取各个地图的诱导信息
		void ComparisonToJson();//将输出信息JSON化
		void statisticsToJson();//汇总的统计信息
		void ComparisonToGeoJson();//将比较信息输出成GeoJson
		void showDifference();//输出对比信息
public:

		//批量处理文件时新添加的函数。
	void clearInit();
	void readGuidanceInfo(ifstream& fin, vector<GuidanceInfo>& v, string& StartLatLng, string&EndLatLng);//读取诱导信息
	bool loadMapConfig();//读取配置信息
	void findDifference();//批量处理诱导信息

	double HausdorffDistance(vector<point> v1, vector<point> v2);//豪斯多夫距离
	bool getRouteInfo(string RouteFile, string startLngLat, string endLngLat, vector<point>& v);
	bool isSameRoute(string startLngLat, string endLngLat);//保持路线一致
private:
	//用于每条路线的统计
	int mSameGuidanceCount;//诱导信息相同的个数
	int mDifferenceGuidanceCount;
	double mMissRate;//缺失率=（高德诱导数-相同的诱导数-不同的诱导数）/高德诱导数
	double mDifferenceRate;//相异率=不同的诱导数/高德诱导数
	double mExtraRate;//超额率=多出诱导数/高德诱导数
private:
	//用于最终统计
	
	long mRoutesNumber;//路线总数
	long mCrossNumber;//路口总数（诱导信息数）

	long mMissCross;//缺失路口数
	long mDifferenceCross;//相异路口数
	long mExtraCross;//附加路口数

	long mMissRoute;//缺失路口数
	long mDifferenceRoute;//相异路口数
	long mExtraRoute;//附加路口数

};


#endif