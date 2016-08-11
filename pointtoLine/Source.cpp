#include<iostream>
#include<cmath>
using namespace std;
struct TXRoutePoint
{
	int x;
	int y;
};
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

int main()
{
	double lng1 = 116.313610, lat1 = 39.957400;
	double lng2 = 116.314960, lat2 = 39.957420;
	double lng3 = 116.31496944444444, lat3 = 39.957148333333336;




	TXRoutePoint p1, p2, p3,p4;
	p1.x = (int)(lng1*100000.0+0.5);
	p1.y = (int)(lat1*100000.0 + 0.5);
	p2.x = (int)(lng2*100000.0 + 0.5);
	p2.y = (int)(lat2*100000.0 + 0.5);
	p3.x = (int)(lng3*100000.0 + 0.5);
	p3.y = (int)(lat3*100000.0 + 0.5);
	
	cout << sqrt(minDistanceForLine(p1,p2,p3,&p4)) << endl;
	getchar();
	return 0;
}