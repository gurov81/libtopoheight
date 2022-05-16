#ifndef __TRIANGLE_HPP__
#define __TRIANGLE_HPP__

// https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle

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
inline bool PointInTriangle (fPoint pt, fPoint v1, fPoint v2, fPoint v3, double d_out[3]) {
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
}

#endif //__TRIANGLE_HPP__
