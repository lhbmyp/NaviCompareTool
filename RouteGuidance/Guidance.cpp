/*
*	腾讯地图与高德地图诱导信息对比类CGuidance的实现
*	taterzhang 2016/7/15
*/
#define _CRT_SECURE_NO_WARNINGS
#include"Guidance.h"



CGuidance::CGuidance(string tencentFilename, string gaodeFilename, string configureFilename, string Route_t, string	Route_g, string resultFilename)
{
	//CGuidance类的构造函数
	m_tencentFile = tencentFilename;
	m_gaodeFile = gaodeFilename;
	m_mapconfigFile = configureFilename;
	m_outFile = resultFilename;

	//路线信息文件
	m_tencentRouteFile = Route_t;
	m_gaodeRouteFile = Route_g;

	mSameGuidanceCount = 0;//相同的诱导信息个数
	mDifferenceGuidanceCount = 0;
	mMissRate = 0.0;//缺失率=（高德诱导数-相同的诱导数-不同的诱导数）/高德诱导数
	mDifferenceRate = 0.0;//相异率=不同的诱导数/高德诱导数
	mExtraRate = 0.0;//超额率=多出诱导数/高德诱导数


	//用于最终的统计数
	 mRoutesNumber=0;//路线总数
	 mCrossNumber=0;//路口总数（诱导信息数）

	 mMissCross = 0;//缺失路口数
	 mDifferenceCross = 0;//相异路口数
	 mExtraCross = 0;//附加路口数

	 mMissRoute = 0;//缺失路线数
	 mDifferenceRoute = 0;//相异路线数
	 mExtraRoute = 0;//附加路线数

}
CGuidance::~CGuidance()
{
	//CGuidance类的析构函数
}


bool CGuidance::loadMapConfig()
{
	//读取映射配置文件，建立映射关系
	ifstream fin;
	char str[LINE_LENGTH];
	fin.open(m_mapconfigFile);
	while (fin.getline(str, LINE_LENGTH))
	{
		int guidanceID1 = 0;//高德的诱导图标ID
		int guidanceID2 = 0;//腾讯的诱导图标ID
		char *tokenPtr = strtok(str, " ");
		while (tokenPtr != NULL)
		{
			if (!(guidanceID1 = atoi(tokenPtr)))
			{
				return false;
			}
			tokenPtr = strtok(NULL, " ");
			if (tokenPtr == NULL)
			{
				return false;
			}
			if (!(guidanceID2 = atoi(tokenPtr)))
			{
				return false;
			}
			break;
		}
		m_GuidnaceMap.insert(make_pair(guidanceID1, guidanceID2));//插入映射信息，高德的ID为key，腾讯的为value

	}
	fin.close();
	return true;
}

void CGuidance::clearInit()
{
	v_tencentGuidanceInfo.clear();//清空处理完的记录
	v_gaodeGuidanceInfo.clear();
	v_tecentMoreInfo.clear();//OUT 相比于高德，腾讯多出的诱导图标ID
	v_gaodeMoreInfo.clear();//OUT 相比于腾讯,高德多出的诱导图标ID
	v_tecentDiffInfo.clear();//OUT 相比于高德，腾讯多出的诱导图标ID
	v_gaodeDiffInfo.clear();//OUT 相比于腾讯,高德多出的诱导图标ID
	mSameGuidanceCount = 0;//相同的诱导信息个数
	mDifferenceGuidanceCount = 0;
	mMissRate = 0.0;//缺失率=（高德诱导数-相同的诱导数-不同的诱导数）/高德诱导数
	mDifferenceRate = 0.0;//相异率=不同的诱导数/高德诱导数
	mExtraRate = 0.0;//超额率=多出诱导数/高德诱导数
}



void CGuidance::readGuidanceInfo(ifstream& fin, vector<GuidanceInfo>& v, string& StartLatLng, string& EndLatLng)//获取一条路线的诱导信息
{
	char buffer[MAXSIZE];
	fin.getline(buffer, MAXSIZE);
	do
	{
		//cout << buffer << endl;
		if (!strcmp(buffer, "GuidanceInfo start"))
		{
			//去读起点终点的经纬度
			fin.getline(buffer, MAXSIZE);
			
			if (!strcmp(buffer, "GuidanceInfo end"))
			{//防止为空记录
				break;
			}
			else
			{
				//读起点终点取经纬度
				StartLatLng = buffer;
				
				fin.getline(buffer, MAXSIZE);
				EndLatLng = buffer;
				
			
			}
			continue;
		}
		else if (!strcmp(buffer, "GuidanceInfo end"))
		{
			return ;
		}
		else
		{
			GuidanceInfo info;
			int ret = sscanf(buffer, "%d %lf %lf", &info.ID, &info.Longitude, &info.Latitude);
			if (ret != 3)
			{
				cout << "读取:" << buffer << endl;
				cerr << "读取诱导信息失败" << endl;
				return;
			}
			
			v.push_back(info);
		}

	} while (!fin.eof() && fin.getline(buffer, MAXSIZE));
	return;
}
void CGuidance::findDifference()
{
	
	if (loadMapConfig())
	{
		//读取配置文件成功
		ifstream t_fin, g_fin;
		t_fin.open(m_tencentFile.c_str(), ios::in);
		g_fin.open(m_gaodeFile.c_str(), ios::in);
		int cn = 0;
		while (!t_fin.eof() || !g_fin.eof())//循环读取记录
		{
			readGuidanceInfo(t_fin, v_tencentGuidanceInfo,mStartLatLng_t, mEndLatLng_t);//读取腾讯信息
			readGuidanceInfo(g_fin, v_gaodeGuidanceInfo, mStartLatLng_g, mEndLatLng_g);//读取高德诱导信息
			if (isSameRoute(mStartLatLng_t, mEndLatLng_t))
			{
				//路线一致，然后对比
				mRoutesNumber++;//读取了一条可以比较的路线
				cout << "路线:"<<cn++ << endl;
				compareMapGuidance();
			//	showDifference();
				ComparisonToJson();//输出比较的结果
			}
		

			clearInit();

		}
		statisticsToJson();//汇总统计信息并输出到文件

		t_fin.close();
		g_fin.close();
		
	}
	else
	{
		cerr << "读取诱导对应的配置文件失败" << endl;
		return;
	}
	
	return;
}

bool CGuidance::getGuidanceInfo(string file, vector<GuidanceInfo>& v)
{
	//输入：file 诱导信息文件，v是保存诱导信息的容器
	//功能：获取要比较的地图的诱导信息
	//返回值：成功读取就返回true。
	ifstream fin;
	fin.open(file);
	char str[LINE_LENGTH];

	//读取诱导信息
	while (fin.getline(str, LINE_LENGTH))
	{

		GuidanceInfo info;
		int ret = sscanf(str, "%d %lf %lf", &info.ID, &info.Longitude, &info.Latitude);
		if (ret != 3)
		{
			cerr << "读取腾讯诱导信息失败" << endl;
			return false;
		}
		v.push_back(info);
	}
	fin.close();
	return true;
}
bool CGuidance::init()
{
	//输入：两个地图诱导信息的输出文件
	//功能：将两个地图的诱导信息读入到容器中。
	//返回值：如果文件内容读取成功返回true，否则false。

	//读取映射配置文件，建立映射关系
	if (loadMapConfig())
	{
		//读取腾讯诱导信息
		if (getGuidanceInfo(m_tencentFile, v_tencentGuidanceInfo))
		{
			//读取高德诱导信息
			if (getGuidanceInfo(m_gaodeFile, v_gaodeGuidanceInfo))
			{
				return true;
			}

		}
	}
	return false;
	
}
/**
* google maps的脚本里代码
*/
double CGuidance::rad(double d)
{
	return (d * PI) / 180.0;
}

/**
* 根据两点间经纬度坐标（double值），计算两点间距离，单位为千米
*/
double CGuidance::GetDistance(double lat1, double lng1, double lat2, double lng2)
{
	double radLat1 = rad(lat1);
	double radLat2 = rad(lat2);
	double a = radLat1 - radLat2;
	double b = rad(lng1) - rad(lng2);
	double s = 2 * asin(sqrt(pow(sin(a / 2), 2) +cos(radLat1)*cos(radLat2)*pow(sin(b / 2), 2)));
	s = s * EARTH_RADIUS;
	s = (s * 10000) / 10000;
	return s;
}


/*待测试  开始*/
static double fastSquareDistanceBetween(TXRoutePoint p0, TXRoutePoint p1)
{
	double dx = p0.x - p1.x, dy = p0.y - p1.y;
	return dx*dx + dy*dy;
}

static double minDistanceForLine(TXRoutePoint p0, TXRoutePoint p1, TXRoutePoint p, TXRoutePoint *pp)
{
	double dx = p1.x - p0.x, dy = p1.y - p0.y;

	double lensqrt = dx*dx + dy*dy;
	if (lensqrt == 0.0) {
		*pp = p0;
		return fastSquareDistanceBetween(p0, p);
	}

	double dpx = p.x - p0.x, dpy = p.y - p0.y;

	double t = (dpx*dx + dpy*dy) / lensqrt;
	if (t <= 0.0) {
		*pp = p0;
		return fastSquareDistanceBetween(p0, p);
	}
	else if (t>1.0) {
		*pp = p1;
		return fastSquareDistanceBetween(p1, p);
	}

	double px = p0.x + t*dx, py = p0.y + t*dy;

	pp->x = (int)(px + .5); pp->y = (int)(py + .5);

	return fastSquareDistanceBetween(*pp, p);
}



double CGuidance::HausdorffDistance(vector<point> v1, vector<point> v2)
{
	double h = 0.0;
	for (vector<point>::iterator it_v1 = v1.begin(); it_v1 != v1.end(); ++it_v1)
	{
		double shorthaus = DBL_MAX;
		for (vector<point>::iterator it_v2 = v2.begin(); it_v2 != v2.end(); ++it_v2)
		{
			TXRoutePoint p0, p1, p, p3;
			p.x = it_v1->Longitude*100000.0;
			p.y = it_v1->Latitude*100000.0;
			p1.x = it_v2->Longitude*100000.0;
			p1.y = it_v2->Latitude*100000.0;
			if (it_v2 == v2.begin())//taterzhang,获取确定直线的两个点
			{
				p0.x = it_v2->Longitude*100000.0;
				p0.y = it_v2->Latitude*100000.0;
			}
			else
			{
				p0.x = (it_v2-1)->Longitude*100000.0;
				p0.y = (it_v2-1)->Latitude*100000.0;
			}
			double dis = minDistanceForLine(p0,p1,p,&p3);//获取点到直线的距离
			if (dis < shorthaus)
			{//最小值
				shorthaus = dis;
			}
		}
		if (shorthaus>h)
		{//最大值
			h = shorthaus;
		}
	}
	return sqrt(h);
}

bool CGuidance::getRouteInfo(string RouteFile, string startLngLat, string endLngLat, vector<point>& v)
{
	ifstream fin;
	char buffer[MAXSIZE];
	fin.open(RouteFile.c_str());
	//读取路线经纬度
	fin.getline(buffer, MAXSIZE);
	//cout << "起点" << startLngLat << endl;
	while (!fin.eof() && strcmp(buffer, startLngLat.c_str()))//RoyMa:只是找第一个起点，如果之前有相同的起点，找的不是理想的起点，就直接返回false了
	{
		//找起点经纬度
		fin.getline(buffer, MAXSIZE);
	}

	if (!fin.eof())
	{//找到起点
		fin.getline(buffer, MAXSIZE);
		if (!fin.eof() && !strcmp(buffer, endLngLat.c_str()))
		{	//读取到终点

			fin.getline(buffer, MAXSIZE);//读取第一个
			while (!fin.eof() && strcmp(buffer, "RouteInfo end"))
			{//读取路线经纬度
				point p;
				sscanf(buffer,"%lf %lf",&p.Longitude,&p.Latitude);
				v.push_back(p);
				fin.getline(buffer, MAXSIZE);//读取下一个
			}
			return true;
		}
		
	}
	
	
	fin.close();
	return false;
}
bool CGuidance::isSameRoute(string startLngLat,string endLngLat)
{
	vector<point> tv, gv;
	//读取路线经纬度
	if (false == getRouteInfo(m_tencentRouteFile, startLngLat, endLngLat, tv))
	{
		return false;
	}
	if (false == getRouteInfo(m_gaodeRouteFile, startLngLat, endLngLat, gv))
	{
		return false;
	}

	//判断是否一致
	double hsd = HausdorffDistance(tv, gv);

	if (hsd< 30)
	{
		return true;
	}
	else
	{
		return false;
	}
}
/*		 待测试     结束*/


bool CGuidance::isSameEvent(double lat1, double lng1, double lat2, double lng2)
{
	
		//	这个事件点的经纬度判断需要验证？？？？！！！！
	double dis = GetDistance(lat1, lng1, lat2, lng2)*1000.0;
	//cout << "距离" <<dis<< endl;
	if (dis< 30.0)
	{
		//精度在25mi则认为是同一个EVENT
		return true;
	}
	return false;
}
void CGuidance::compareMapGuidance()
{
	//功能：比较两个地图的诱导信息的不同。
	int nGaodeGuidanceCount = v_gaodeGuidanceInfo.size();//获取高德的诱导数
	mCrossNumber += nGaodeGuidanceCount;//路口数增加，以高德诱导为基准

	for (vector<GuidanceInfo>::iterator it = v_gaodeGuidanceInfo.begin(); it != v_gaodeGuidanceInfo.end(); ++it)
	{
		int gaodeID = it->ID;
		double gaode_Lat = it->Latitude;//经度
		double gaode_Lng = it->Longitude;//纬度
		bool isfind = false;
		
		for (vector<GuidanceInfo>::iterator te_it = v_tencentGuidanceInfo.begin(); te_it != v_tencentGuidanceInfo.end(); ++te_it)
		{
			//cout << "GID:"<<it->ID<<"TID:"<<te_it->ID << endl;
			if (isSameEvent(gaode_Lat, gaode_Lng,te_it->Latitude,te_it->Longitude))
			{
				//找到相近的坐标，待测试
				int mapTencentID;
				multimap<int, int>::iterator pos=m_GuidnaceMap.find(gaodeID);
				if (pos != m_GuidnaceMap.end())
				{
					mapTencentID = pos->second;
				}
				else
				{
					//没有标示的高德诱导图标
					v_tecentDiffInfo.push_back(*te_it);//该诱导信息是腾讯不同与高德的
					v_gaodeDiffInfo.push_back(*it);//与腾讯不同的诱导信息
					v_tencentGuidanceInfo.erase(te_it);//在腾讯诱导集合中删除该点
					isfind = true;
					break;//处理下一个高德诱导信息
					
				}
			
				//找到对应的目标诱导ID
				if ((mapTencentID == te_it->ID) || (gaodeID == 7 && 40 <= te_it->ID && te_it->ID <= 41)
					|| (gaodeID == 12 && 51 <= te_it->ID && te_it->ID <= 57) || (gaodeID == 15 && 60 <= te_it->ID && te_it->ID <= 62))
				{
					//在相同的事件点（按经纬度相近判断），诱导信息相同，
					mSameGuidanceCount++;
					v_tencentGuidanceInfo.erase(te_it);//在腾讯诱导集合中删除该点
					isfind = true;
					break;
					
				}
				else
				{
					//经纬度相近，但对应的诱导信息不对应
					v_tecentDiffInfo.push_back(*te_it);//该诱导信息是腾讯不同与高德的
					v_gaodeDiffInfo.push_back(*it);//与腾讯不同的诱导信息
					v_tencentGuidanceInfo.erase(te_it);//在腾讯诱导集合中删除该点
					isfind = true;
					break;//处理下一个高德诱导信息
					
				}	
				
			}
		}
		if (!isfind)
		{
			//找高德多出的部分
			v_gaodeMoreInfo.push_back(*it);
		}
		
	}
	//比较完高德的数据，剩下的就是腾讯地图多出的诱导信息
	for (vector<GuidanceInfo>::iterator te_it = v_tencentGuidanceInfo.begin(); te_it != v_tencentGuidanceInfo.end(); ++te_it)
	{
		//找腾讯多出的部分
		v_tecentMoreInfo.push_back(*te_it);
	}

	mDifferenceGuidanceCount = v_tecentDiffInfo.size();
	mMissRate = ((double)(nGaodeGuidanceCount - mSameGuidanceCount - mDifferenceGuidanceCount)) / nGaodeGuidanceCount;//单条路线的缺失率
	mDifferenceRate = ((double)mDifferenceGuidanceCount) / nGaodeGuidanceCount;
	mExtraRate = ((double)v_tecentMoreInfo.size()) / nGaodeGuidanceCount;

	//统计汇总
	if (mMissRate > 0.0)
	{
		mMissRoute++;
	}
	if (mDifferenceRate > 0.0)
	{
		mDifferenceRoute++;
	}
	if (mExtraRate > 0.0)
	{
		mExtraRoute++;
	}

	mMissCross += (nGaodeGuidanceCount - mSameGuidanceCount - mDifferenceGuidanceCount);
	mDifferenceCross += (mDifferenceGuidanceCount);
	mExtraCross += (v_tecentMoreInfo.size());
	return;
}

void CGuidance::showDifference()
{
	//功能：输出对比信息

	//测试信息输出
	cout << setiosflags(ios::fixed) << setprecision(6);
	/*for (vector<GuidanceInfo>::iterator it = v_tencentGuidanceInfo.begin(); it != v_tencentGuidanceInfo.end(); ++it)
	{
	cout << "ID:" << it->ID << "经度:"  << it->Latitude << "纬度:" << it->Longitude << endl;
	}
	for (vector<GuidanceInfo>::iterator it = v_gaodeGuidanceInfo.begin(); it != v_gaodeGuidanceInfo.end(); ++it)
	{
	cout << "ID:" << it->ID << "经度:" << it->Latitude << "纬度:" << it->Longitude << endl;
	}
	*/

	cout << "导航起点经纬度:(" << mStartLatLng_t << ")" << endl;
	cout << "导航终点经经度:(" << mEndLatLng_t << ")" << endl;
	cout << "缺失率" << mMissRate << endl;
	cout << "相异率" << mDifferenceRate <<endl;
	cout << "附加率" << mExtraRate <<endl;
	cout << "\n腾讯地图相对于高德多出的诱导信息:" << endl;
	for (vector<GuidanceInfo>::iterator it = v_tecentMoreInfo.begin(); it != v_tecentMoreInfo.end(); ++it)
	{
		cout << "诱导信息:" << gettextFromTencentByID(it->ID) << ";  该点经纬度:（" << it->Longitude << "，" << it->Latitude << ")" << endl;
	}


	cout << "\n腾讯地图相对于高德缺少的诱导信息:" << endl;
	for (vector<GuidanceInfo>::iterator it = v_gaodeMoreInfo.begin(); it != v_gaodeMoreInfo.end(); ++it)
	{//高德多出的信息
		cout << "诱导信息:" << gettextFromGaodeByID(it->ID) << ";  该点经纬度:（" << it->Longitude << "，" << it->Latitude << ")" << endl;
	}

	cout << "\n腾讯地图与高德不同的诱导信息:" << endl;
	for (vector<GuidanceInfo>::iterator it = v_tecentDiffInfo.begin(), g_it = v_gaodeDiffInfo.begin(); it != v_tecentDiffInfo.end(); ++g_it, ++it)
	{

		cout << "腾讯的诱导信息:" << gettextFromTencentByID(it->ID) << ";  该点经纬度:（" << it->Longitude << "，" << it->Latitude << ")" << endl;
		cout << "此事件点高德的诱导信息:" << gettextFromGaodeByID(g_it->ID) << ";  该点经纬度:（" << g_it->Longitude << "，" << g_it->Latitude << ")" << endl;
	}



}


void CGuidance::statisticsToJson()
{
	ofstream out;
	out.open(m_outFile, ios::app);//输出对比结果到文件中
	out << "[" << endl;
	out << "\"RouteMissRate\":\"" << (double)mMissRoute / mRoutesNumber << "\"," << endl;
	out << "\"RouteDifferenceRate\":\"" << (double)mDifferenceRoute / mRoutesNumber << "\"," << endl;
	out << "\"RouteExtraRate\":\"" << (double)mExtraRoute / mRoutesNumber << "\"," << endl;
	out << "\"CrossMissRate\":\"" << (double)mMissCross/mCrossNumber << "\"," << endl;
	out << "\"CrossDifferenceRate\":\"" << (double)mDifferenceCross / mCrossNumber << "\"," << endl;
	out << "\"CrossExtraRate\":\"" << (double)mExtraCross/mCrossNumber << endl;
	out << "]" << endl;
	out.close();
	return;
}
void CGuidance::ComparisonToJson()
{
	ofstream out;
	out.open(m_outFile,ios::app);//输出对比结果到文件中
	out << "[" << endl;

	out << "\"MissRate\":\"" << mMissRate <<"\","<<endl;
	out << "\"DifferenceRate\":\"" << mDifferenceRate << "\"," << endl;
	out << "\"ExtraRate\":\"" << mExtraRate << "\"," << endl;

	out << "{" << endl;
	out << "\"source\":" << "\"Tencent\"," << endl;
	out << "\"EventInfo\":[" << endl;
	for (vector<GuidanceInfo>::iterator it = v_tecentMoreInfo.begin(); it != v_tecentMoreInfo.end(); ++it)
	{

		out << "{" << endl;
		out << "\"IconType\":" << "\"" << it->ID << "\"," << endl;
		out << "\"IconText\":" << "\"" << gettextFromTencentByID(it->ID) << "\"," << endl;
		out << "\"Lat\":" << "\"" << it->Latitude << "\"," << endl;
		out << "\"Lng\":" << "\"" << it->Longitude << "\"" << endl;
		out << "}";
		if (it + 1 != v_tecentMoreInfo.end())
		{
			out << ",";
		}
		out << endl;
	}
	out << "]" << endl;
	out << "}" << endl;


	//处理高德多出的部分
	out << ",{" << endl;
	out << "\"source\":" << "\"Gaode\"," << endl;
	out << "\"EventInfo\":[" << endl;
	for (vector<GuidanceInfo>::iterator it = v_gaodeMoreInfo.begin(); it != v_gaodeMoreInfo.end(); ++it)
	{

		out << "{" << endl;
		out << "\"IconType\":" << "\"" << it->ID << "\"," << endl;
		out << "\"IconText\":" << "\"" << gettextFromGaodeByID(it->ID) << "\"," << endl;
		out << "\"Lat\":" << "\"" << it->Latitude << "\"," << endl;
		out << "\"Lng\":" << "\"" << it->Longitude << "\"" << endl;
		out << "}";
		if (it + 1 != v_gaodeMoreInfo.end())
		{
			out << ",";
		}
		out << endl;
	}
	out << "]" << endl;
	out << "}" << endl;

	//处理诱导不同
	out << ",{" << endl;
	out << "\"source\":" << "\"Tencent_Diff\"," << endl;
	out << "\"EventInfo\":[" << endl;
	for (vector<GuidanceInfo>::iterator it = v_tecentDiffInfo.begin(); it != v_tecentDiffInfo.end(); ++it)
	{

		out << "{" << endl;
		out << "\"IconType\":" << "\"" << it->ID << "\"," << endl;
		out << "\"IconText\":" << "\"" << gettextFromTencentByID(it->ID) << "\"," << endl;
		out << "\"Lat\":" << "\"" << it->Latitude << "\"," << endl;
		out << "\"Lng\":" << "\"" << it->Longitude << "\"" << endl;
		out << "}";
		if (it + 1 != v_tecentDiffInfo.end())
		{
			out << ",";
		}
		out << endl;
	}
	out << "]" << endl;
	out << "}" << endl;

	//Tencent对应的Gaode信息
	out << ",{" << endl;
	out << "\"source\":" << "\"Gaode_Diff\"," << endl;
	out << "\"EventInfo\":[" << endl;
	for (vector<GuidanceInfo>::iterator it = v_gaodeDiffInfo.begin(); it != v_gaodeDiffInfo.end(); ++it)
	{

		out << "{" << endl;
		out << "\"IconType\":" << "\"" << it->ID << "\"," << endl;
		out << "\"IconText\":" << "\"" << gettextFromGaodeByID(it->ID) << "\"," << endl;
		out << "\"Lat\":" << "\"" << it->Latitude << "\"," << endl;
		out << "\"Lng\":" << "\"" << it->Longitude << "\"" << endl;
		out << "}";
		if (it + 1 != v_gaodeDiffInfo.end())
		{
			out << ",";
		}
		out << endl;
	}
	out << "]" << endl;
	out << "}" << endl;
	out << "]" << endl;
	out.close();

	return;
}


void CGuidance::ComparisonToGeoJson()
{
	ofstream out(GUIDANCECOMPARISON_GEOJSON);//输出对比结果到文件中
	out << "{" << endl;
	
	out << "\"type\": \"FeatureCollection\"," << endl;
	out<<"\"features\":[" << endl;

	//腾讯多出的
	for (vector<GuidanceInfo>::iterator it = v_tecentMoreInfo.begin(); it != v_tecentMoreInfo.end(); ++it)
	{

		out << "{" << endl;
		out << " \"type\": \"Feature\"," << endl;
		out << "  \"geometry\": {" <<endl;
		out <<"\"type\": \"Point\","<<endl;
		out << " \"coordinates\": [" << it->Latitude << "," << it->Longitude << "]" << endl;
		out << "}," << endl;
		out << "  \"properties\":{" <<endl;
		out << "\"IconType\": \"" << it->ID << "\"," << endl;
		out << "\"IconText\": \"" << gettextFromTencentByID(it->ID) << "\"," << endl;
		out << "\"source\":\"Tencent\"}" << endl;
		out << "}" << endl;
		if (it + 1 != v_tecentMoreInfo.end())
		{
			out << ",";
		}
		out << endl;
	}

	//高德多出的
	for (vector<GuidanceInfo>::iterator it = v_gaodeMoreInfo.begin(); it != v_gaodeMoreInfo.end(); ++it)
	{

		out << ",{" << endl;
		out << " \"type\": \"Feature\"," << endl;
		out << "  \"geometry\": {" << endl;
		out << "\"type\": \"Point\"," << endl;
		out << " \"coordinates\": [" << it->Latitude << "," << it->Longitude << "]" << endl;
		out << "}," << endl;
		out << "  \"properties\":{" << endl;
		out << "\"IconType\": \"" << it->ID << "\"," << endl;
		out << "\"IconText\": \"" << gettextFromGaodeByID(it->ID) << "\"," << endl;
		out << "\"source\":\"Tencent\"}" << endl;
		out << "}" << endl;
		if (it + 1 != v_gaodeMoreInfo.end())
		{
			out << ",";
		}
		out << endl;
	}

	//诱导不同——腾讯
	for (vector<GuidanceInfo>::iterator it = v_tecentDiffInfo.begin(); it != v_tecentDiffInfo.end(); ++it)
	{

		out << ",{" << endl;
		out << " \"type\": \"Feature\"," << endl;
		out << "  \"geometry\": {" << endl;
		out << "\"type\": \"Point\"," << endl;
		out << " \"coordinates\": [" << it->Latitude << "," << it->Longitude << "]" << endl;
		out << "}," << endl;
		out << "  \"properties\":{" << endl;
		out << "\"IconType\": \"" << it->ID << "\"," << endl;
		out << "\"IconText\": \"" << gettextFromTencentByID(it->ID) << "\"," << endl;
		out << "\"source\":\"Tencent_Diff\"}" << endl;
		out << "}" << endl;
		if (it + 1 != v_tecentDiffInfo.end())
		{
			out << ",";
		}
		out << endl;
	}
	//诱导不同——高德
	for (vector<GuidanceInfo>::iterator it = v_gaodeDiffInfo.begin(); it != v_gaodeDiffInfo.end(); ++it)
	{

		out << ",{" << endl;
		out << " \"type\": \"Feature\"," << endl;
		out << "  \"geometry\": {" << endl;
		out << "\"type\": \"Point\"," << endl;
		out << " \"coordinates\": [" << it->Latitude << "," << it->Longitude << "]" << endl;
		out << "}," << endl;
		out << "  \"properties\":{" << endl;
		out << "\"IconType\": \"" << it->ID << "\"," << endl;
		out << "\"IconText\": \"" << gettextFromGaodeByID(it->ID) << "\"," << endl;
		out << "\"source\":\"Gaode_Diff\"}" << endl;
		out << "}" << endl;
		if (it + 1 != v_gaodeDiffInfo.end())
		{
			out << ",";
		}
		out << endl;
	}
	out << "]" << endl;
	out << "}" << endl;
	out.close();


	return;
}



string CGuidance::gettextFromTencentByID(int guidanceid)
{
	//可以设置string数组直接获取字符串，替代函数
	switch (guidanceid)
	{
	case 1:
	{
			  return "直行图标";
	}
	case 2:
	{
			  return "左转";
	}
	case 3:
	{
			  return "右转";
	}
	case 4:
	{
			  return "左转掉头";
	}
	case 5:
	{
			  return "进入环岛";
	}
	case 6:
	{
			  return "辅路通过平交路口进主路";
	}
	case 7:
	{
			  return "主路通过平交路口进辅路";
	}
	case 8:
	{
			  return "keep导航中";
	}
	case 10:
	case 11:
	case 12:
	case 13:
	{
			   return "偏左转";
	}

	case 20:
	case 21:
	case 22:
	case 23:
	{
			   return "偏右转";
	}
	case 30:
	case 31:
	{
			   return "左后转弯";
	}
	case 32:
	{
			   return "左转到左侧";
	}
	case 33:
	{
			   return "左转到右侧";
	}
	case 34:
	{
			   return "左转到最左侧";
	}
	case 35:
	{
			   return "左转到中间";
	}
	case 36:
	{
			   return "左转到最右侧";
	}
	case 37:
	{
			   return "左前直行";
	}
	case 38:
	{
			   return "左前转弯";
	}

	case 40:
	case 41:
	{
			   return "右后转弯";
	}
	case 42:
	{
			   return "右转到左侧";
	}
	case 43:
	{
			   return "右转到右侧";
	}
	case 44:
	{
			   return "右转到最左侧";
	}
	case 45:
	{
			   return "右转到中间";
	}
	case 46:
	{
			   return "右转到最右侧";
	}
	case 47:
	{
			   return "右前直行";
	}
	case 48:
	{
			   return "右前转弯";
	}
	case 49:
	case 51:
	case 52:
	case 53:
	case 54:
	{
			   return "环岛X出口";
	}
	case 60:
	case 61:
	case 62:
	{
			   return "到达终点";
	}
	case 81:
	{
			   return "靠左继续行驶";
	}
	case 82:
	{
			   return "靠右继续行驶";
	}
	default:
		return "待处理图标类型";
	}

	return "";
}
string CGuidance::gettextFromGaodeByID(int guidanceid)
{

	//可以设置string数组直接获取字符串，替代函数
	switch (guidanceid)
	{
	case 1:
	{
			  return "自车图标";
	}
	case 2:
	{
			  return "左转图标";
	}
	case 3:
	{
			  return "右转图标";
	}
	case 4:
	{
			  return "左前方图标";
	}
	case 5:
	{
			  return "右前方图标";
	}
	case 6:
	{
			  return "左后方图标";
	}
	case 7:
	{
			  return "右后方图标";
	}
	case 8:
	{
			  return "左转掉头图标";
	}
	case 9:
	{
			  return "直行图标";
	}
	case 10:
	{
			   return "到达途经点图标";
	}
	case 11:
	{
			   return "进入环岛图标";
	}
	case 12:
	{
			   return "驶出环岛图标";
	}
	case 13:
	{
			   return "到达服务区图标";
	}
	case 14:
	{
			   return "到达收费站图标";
	}
	case 15:
	{
			   return "到达目的地图标";
	}
	case 16:
	{
			   return "到达隧道图标";
	}
	case 17:
	{
			   return "通过人行横道图标";
	}
	case 18:
	{
			   return "通过过街天桥图标";
	}
	case 19:
	{
			   return "通过地下通道图标";
	}
	case 20:
	{
			   return "通过广场图标";
	}
	case 21:
	{
			   return "通过公园图标";
	}
	case 22:
	{
			   return "通过扶梯图标";
	}
	case 23:
	{
			   return "通过直梯图标";

	}
	case 24:
	{
			   return "通过索道图标";
	}
	case 25:
	{
			   return "通过空中通道图标";
	}
	case 26:
	{
			   return "通过通道、建筑物穿越通道图标";
	}
	case 27:
	{
			   return "通过行人道路图标";
	}
	case 28:
	{
			   return "通过游船路线图标";
	}
	case 29:
	{
			   return "通过观光车路线图标";
	}
	case 30:
	{
			   return "通过滑道图标";
	}
	case 31:
	{
			   return "通过阶梯图标";
	}
	default:
		return "待处理图标类型";
	}
	return "";
}


