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
* google maps�Ľű������
*/
const double EARTH_RADIUS = 6378.137;
#define PI  3.14159265359	

#define PRECISION 3.0
struct GuidanceInfo
{
	int ID;
	double Longitude;//����
	double Latitude;//γ��
	
};
struct point
{
	double Longitude;//����
	double Latitude;//γ��
};
struct TXRoutePoint
{
	int x;//����*100000
	int y;//γ��*100000
};
class CGuidance
{
	private:
		vector<GuidanceInfo> v_tencentGuidanceInfo;//IN �����Ѷ��ͼ���յ�
		vector<GuidanceInfo> v_gaodeGuidanceInfo;//IN ��Ÿߵµ�ͼ���յ�
		multimap<int, int> m_GuidnaceMap;//������ͼ�յ���Ϣ��ӳ��
		string m_tencentFile;//���������ļ�·�� IN
		string m_gaodeFile;	//IN 
		string m_mapconfigFile;//ӳ�������ļ� IN
		string m_outFile;//�ȽϽ��������ļ��� OUT

		string m_tencentRouteFile;//·����Ϣ�ļ� IN
		string m_gaodeRouteFile;
	private:
		string mStartLatLng_t;//��Ѷ�յ���Ϣ����㾭γ��
		string mEndLatLng_t;//�յ㾭γ��
		string mStartLatLng_g;
		string mEndLatLng_g;

		
		
	private:
		vector<GuidanceInfo> v_tecentMoreInfo;//OUT ����ڸߵ£���Ѷ������յ�ͼ��ID
		vector<GuidanceInfo> v_gaodeMoreInfo;//OUT �������Ѷ,�ߵ¶�����յ�ͼ��ID

		vector<GuidanceInfo> v_tecentDiffInfo;//OUT ����ڸߵ£���Ѷ������յ�ͼ��ID
		vector<GuidanceInfo> v_gaodeDiffInfo;//OUT �������Ѷ,�ߵ¶�����յ�ͼ��ID

	public:
		CGuidance(string tencentFilename, string X, string configureFilename, string Route_T, string Route_G, string resultFilename);
		~CGuidance();
	public:
		bool init();//��������ͼ���յ���Ϣ���뵽�����С�
		void compareMapGuidance();//�Ƚ�������ͼ���յ���Ϣ�Ĳ�ͬ
		double rad(double d);//�������Ĺ��ߺ���
		double GetDistance(double lat1, double lng1, double lat2, double lng2);//����������ľ���
		bool isSameEvent(double lat1, double lng1, double lat2, double lng2);//�ж��¼����Ƿ�һ��

		string gettextFromTencentByID(int guidanceid);
		string gettextFromGaodeByID(int guidanceid);

		bool getGuidanceInfo(string file, vector<GuidanceInfo>& v);//��ʼ���������øú�����ȡ������ͼ���յ���Ϣ
		void ComparisonToJson();//�������ϢJSON��
		void statisticsToJson();//���ܵ�ͳ����Ϣ
		void ComparisonToGeoJson();//���Ƚ���Ϣ�����GeoJson
		void showDifference();//����Ա���Ϣ
public:

		//���������ļ�ʱ����ӵĺ�����
	void clearInit();
	void readGuidanceInfo(ifstream& fin, vector<GuidanceInfo>& v, string& StartLatLng, string&EndLatLng);//��ȡ�յ���Ϣ
	bool loadMapConfig();//��ȡ������Ϣ
	void findDifference();//���������յ���Ϣ

	double HausdorffDistance(vector<point> v1, vector<point> v2);//��˹������
	bool getRouteInfo(string RouteFile, string startLngLat, string endLngLat, vector<point>& v);
	bool isSameRoute(string startLngLat, string endLngLat);//����·��һ��
private:
	//����ÿ��·�ߵ�ͳ��
	int mSameGuidanceCount;//�յ���Ϣ��ͬ�ĸ���
	int mDifferenceGuidanceCount;
	double mMissRate;//ȱʧ��=���ߵ��յ���-��ͬ���յ���-��ͬ���յ�����/�ߵ��յ���
	double mDifferenceRate;//������=��ͬ���յ���/�ߵ��յ���
	double mExtraRate;//������=����յ���/�ߵ��յ���
private:
	//��������ͳ��
	
	long mRoutesNumber;//·������
	long mCrossNumber;//·���������յ���Ϣ����

	long mMissCross;//ȱʧ·����
	long mDifferenceCross;//����·����
	long mExtraCross;//����·����

	long mMissRoute;//ȱʧ·����
	long mDifferenceRoute;//����·����
	long mExtraRoute;//����·����

};


#endif