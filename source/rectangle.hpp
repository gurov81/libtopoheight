#ifndef __RECTANGLE_HPP__
#define __RECTANGLE_HPP__

// расчет прямоугольника, содержащего треугольник (описанного вокруг)
inline void get_bounding_rect(const double points[6], double rect[4] ) {
  const double xmin = std::min(points[0],points[2]);
  const double xmax = std::max(points[0],points[2]);
  const double ymin = std::min(points[1],points[3]);
  const double ymax = std::max(points[1],points[3]);
  rect[0] = std::min(xmin,points[4]);
  rect[1] = std::min(ymin,points[5]);
  rect[2] = std::max(xmax,points[4]);
  rect[3] = std::max(ymax,points[5]);
}

#endif //__RECTANGLE_HPP__
