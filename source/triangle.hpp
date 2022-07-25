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
#endif //__TRIANGLE_HPP__
