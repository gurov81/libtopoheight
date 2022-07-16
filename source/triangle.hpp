#ifndef __TRIANGLE_HPP__
#define __TRIANGLE_HPP__
#include <cmath>


// https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
// https://stackoverflow.com/questions/39908607/how-to-determine-if-a-point-is-in-the-edge-boundaries-of-a-2d-triangle
struct fPoint {
  float x; float y;
  fPoint(float a,float b):x(a),y(b) {}
};
inline float sign(fPoint p1, fPoint p2, fPoint p3) {
  return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}
inline float dist(fPoint p1, fPoint p2) {
  float dx = (p1.x-p2.x), dy = (p1.y-p2.y);
  return sqrtf( dx*dx + dy*dy );
}
bool operator ==(const fPoint& a, const fPoint& b)
{
  return a.x == b.x && a.y == b.y;
}
bool within(double x)
{
  return 0 <= x && x <= 1;
}
inline bool PointInTriangle (fPoint pt, fPoint v1, fPoint v2, fPoint v3, double d_out[3]) {  
#if 0
  float d1, d2, d3;
  bool has_neg, has_pos;
  d1 = sign(pt, v1, v2);
  d2 = sign(pt, v2, v3);
  d3 = sign(pt, v3, v1);
  if(d_out) {
    d_out[0]=d1; d_out[1]=d2; d_out[2]=d3;
  }
  has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
  has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
  return !(has_neg && has_pos);
#endif

//https://stackoverflow.com/questions/25385361/point-within-a-triangle-barycentric-co-ordinates
#if 1
  double det = (v2.y - v3.y)*(v1.x - v3.x) + (v3.x - v2.x)*(v1.y - v3.y);
  double factor_alpha = (v2.y - v3.y)*(pt.x - v3.x) + (v3.x - v2.x)*(pt.y - v3.y);
  double factor_beta = (v3.y - v1.y)*(pt.x - v3.x) + (v1.x - v3.x)*(pt.y - v3.y);  
  double alpha = factor_alpha / det;  
  double beta = factor_beta / det;
  double gamma = 1.0 - alpha - beta;
  return pt == v1 || pt == v2 || pt == v3 || (within(alpha) && within(beta) && within(gamma));
#endif
}

#endif //__TRIANGLE_HPP__
