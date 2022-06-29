#include <iostream>
#include "libtopoheight.h"
#include "utils.hpp"
#include "rtree.h"
#include "picture.h"
#include "rectangle.hpp"
#include "triangle.hpp"
#include "heightmap.hpp"
#include "ctl.h"

struct context {
  utils::Relief relief;
  ctl::DelaunayTriangulation* dt;
  struct rtree *rtree;
  context() : dt(NULL),rtree(NULL) {};
};

#ifdef __cplusplus
extern "C" {
#endif

struct context* libtopoheight_create() {
  struct context* ctx = new context;
  ctx->rtree = rtree_new( sizeof(size_t), 2 );
  return ctx;
}

void libtopoheight_destroy( struct context* ctx ) {
  if(ctx->dt) delete ctx->dt;
  if(ctx->rtree) rtree_free(ctx->rtree);
  delete ctx;
}

int libtopoheight_load_file(struct context* ctx, const char* filename, const char* propname) {
  std::string json;
  const std::string prop = propname ? std::string(propname) : "";
  try {
    json = utils::read_file(filename);
  }
  catch(...) {
    return 1;
  }
  try {
    ctx->relief = utils::get_geo_json_points(json,prop);
  }
  catch(...) {
    return 2;
  }
  return 0;
}

int libtopoheight_load_buffer(struct context* ctx, const char* buf, size_t size, const char* propname) {
  const std::string json(buf,size);
  const std::string prop = propname ? std::string(propname) : "";
  try {
    ctx->relief = utils::get_geo_json_points(json, prop);
  }
  catch(std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    return 1;
  }
  return 0;
}

/* получение параметров i-го треугольника
   если указан параметр points, возвращает координаты вершин треугольника
   если указан параметр alts, возвращает высоты в вершинах треугольника
*/
static void get_triangle(struct context* ctx, int i, double points[6], double alts[3]) {
  //CDT::TriangleVec& triangles = ctx->cdt->triangles;
  ctl::TIN tin(ctx->dt);
  if(points) {  
    points[0] = tin.verts[tin.triangles[i + 0]].x;	
    points[1] = tin.verts[tin.triangles[i + 0]].y;		
    points[2] = tin.verts[tin.triangles[i + 1]].x;	
    points[3] = tin.verts[tin.triangles[i + 1]].y;	
    points[4] = tin.verts[tin.triangles[i + 2]].x;	
    points[5] = tin.verts[tin.triangles[i + 2]].y;
  }
  if(alts) {
    alts[0] = tin.verts[tin.triangles[i + 0]].z;
    alts[1] = tin.verts[tin.triangles[i + 1]].z;
    alts[2] = tin.verts[tin.triangles[i + 2]].z;
  }
}

//#define log printf
#define log(...)

int libtopoheight_triangulate(struct context* ctx) {
  if( !ctx->relief.points.size() && !ctx->relief.lineStrings.size() && !ctx->relief.polygons.size() )
    return 1;
  if( ctx->dt ) delete ctx->dt;
  ctx->dt = NULL;
  //выполнение триангуляции
  try {
    double minX = -100;
    double minY = -100;
    double maxX = 100;
    double maxY = 100;
    ctl::PointList boundary;
    boundary.push_back(ctl::Point(minX, minY));
    boundary.push_back(ctl::Point(maxX, minY));
    boundary.push_back(ctl::Point(maxX, maxY));
    boundary.push_back(ctl::Point(minX, maxY));

    ctl::DelaunayTriangulation *dt = new ctl::DelaunayTriangulation(boundary);
    ctx->dt = new ctl::DelaunayTriangulation(boundary);
   
    for (int i; i < ctx->relief.points.size(); i++){
      ctx->dt->InsertConstrainedPoint(ctx->relief.points[i]);
    }
    for (int i; i < ctx->relief.lineStrings.size(); i++){
      ctx->dt->InsertConstrainedLineString(ctx->relief.lineStrings[i]);
    }
    for (int i; i < ctx->relief.polygons.size(); i++){
      ctx->dt->InsertConstrainedPolygon(ctx->relief.polygons[i]);
    }
    
    
  }
  catch(std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    return 2;
  }
  //вставка в R-дерево
  double points[6];
  double rect[4]; //lon,lat,lon,lat
  double alts[3];
  const ctl::TIN tin(ctx->dt);
  for(size_t i=0; i<tin.triangles.size(); i++) {
    //получение координат и высот в вершинах треугольника
    get_triangle(ctx, i, points, alts);

#if 0
    std::cout << "\n" << ctx->cdt->vertices[triangles[i].vertices[0]].x;	
    std::cout << "\n" << triangles[i].vertices[0];	
    std::cout << "\n" << triangles[i].vertices[1];
    std::cout << "\n" << triangles[i].vertices[2];
    
 printf("\nTRIANGLE %d: (%3.3f,%3.3f) (%3.3f,%3.3f) (%3.3f,%3.3f) => (%3.3f,%3.3f,%3.3f)",
      i,points[0],points[1],points[2],points[3],points[4],points[5],
      alts[0],alts[1],alts[2]
    );
    
    log("TRIANGLE %d: (%3.3f,%3.3f) (%3.3f,%3.3f) (%3.3f,%3.3f) => (%3.3f,%3.3f,%3.3f)\n",
      i,points[0],points[1],points[2],points[3],points[4],points[5],
      alts[0],alts[1],alts[2]
    );
#endif
    //расчет прямоугольника, содержащего треугольник
    get_bounding_rect(points,rect);
    //вставка прямоугольника в R-дерево с сохранением индекса связанного с ним треугольника
    bool rc = rtree_insert(ctx->rtree,rect,&i);
    if(!rc) {
      return(3);
    }
  }
  return 0;
}

//структура, хранящая контекст поиска в R-дереве
struct userdata_t {
  struct context* ctx;
  size_t index;        //индекс треугольника, евли найден (-1 если не найден)
  double point[2];     //координаты точки, для которой проводится поиск
  double alt;          //искомое значение высоты в заданной точке
};

//функция-колбек в алгоритм поиска, см. примеры в https://github.com/tidwall/rtree.c/blob/master/README.md#example
static bool search_iter(const double *rect, const void *item, void /*struct userdata_t*/ *udata) {
  userdata_t* ud = (userdata_t*)udata;
  //получение треугольника, связанного с текущим прямоугольником
  int index = *(int*)item;
  double points[6]; //координаты вершин
  double alt[3]; //высоты в вершинах
  get_triangle(ud->ctx, index, points, alt);
  //определение, попадает ли заданная точка в треугольник
  fPoint pt(ud->point[0],ud->point[1]);
  fPoint t1(points[0],points[1]);
  fPoint t2(points[2],points[3]);
  fPoint t3(points[4],points[5]);
  double dd[3]; //v1-v2,v2-v3,v1-v3
  bool rc = PointInTriangle(pt,t1,t2,t3,dd);
  if(rc){
    log("search... index=%d pt=(%3.3f,%3.3f) in_triangle:(%3.3f,%3.3f),(%3.3f,%3.3f),(%3.3f,%3.3f)=%d alts=%3.3f,%3.3f,%3.3f\n",
      *index,pt.x,pt.y,t1.x,t1.y,t2.x,t2.y,t3.x,t3.y,rc,
      alt[0],alt[1],alt[2]
    );
  }
  if(!rc) return true;
  //треугольник найден
  ud->index = index;
  ud->alt = -1;
  //расчет высоты в заданной точке по информации о треугольнике
  const double EPS = 0.0001;
  //в вершинах треугольника возвращаем высоту вершины
  if(      dist(pt,t1) < EPS ) {ud->alt=alt[0]; log("ALT=alt1 => %3.3f\n",ud->alt);}
  else if( dist(pt,t2) < EPS ) {ud->alt=alt[1]; log("ALT=alt2 => %3.3f\n",ud->alt);}
  else if( dist(pt,t3) < EPS ) {ud->alt=alt[2]; log("ALT=alt3 => %3.3f\n",ud->alt);}
#if 1
  //на сторонах треугольника возвращаем максимум из высот соответствующих вершин
  //см https://stackoverflow.com/questions/39908607/how-to-determine-if-a-point-is-in-the-edge-boundaries-of-a-2d-triangle
  else if( std::abs(dd[0]) < EPS ) {ud->alt=std::max(alt[0],alt[1]); log("ALT=alt12 => %3.3f (%3.3f,%3.3f)\n",ud->alt,alt[0],alt[1]);}
  else if( std::abs(dd[1]) < EPS ) {ud->alt=std::max(alt[1],alt[2]); log("ALT=alt23 => %3.3f (%3.3f,%3.3f)\n",ud->alt,alt[1],alt[2]);}
  else if( std::abs(dd[2]) < EPS ) {ud->alt=std::max(alt[0],alt[2]); log("ALT=alt13 => %3.3f (%3.3f,%3.3f)\n",ud->alt,alt[0],alt[2]);}
#endif
  else
  {
    //точка внутри треугольника
#if 0
    //максимум из высот трех вершин
    ud->alt = std::max( alt[0],alt[1] );
    ud->alt = std::max( ud->alt, alt[3] );
#endif
#if 1
    //тупой вериант, учитывающий удаление заданной точки от каждой вершины
    // https://codeplea.com/triangular-interpolation
    const double w1 = 1./dist(pt,t1);
    const double w2 = 1./dist(pt,t2);
    const double w3 = 1./dist(pt,t3);
    const double a = (w1*alt[0]+w2*alt[1]+w3*alt[2])/(w1+w2+w3);
    ud->alt = a;
#endif
  }
  return false;
}

int libtopoheight_get_alt(struct context* ctx,const double coord[2], double out_alt[1]) {
  double rect[4] = {coord[0],coord[1],coord[0],coord[1]};
  //контекст поиска
  userdata_t ud;
  ud.ctx = ctx;
  ud.index = -1;
  ud.point[0] = coord[0];
  ud.point[1] = coord[1];
  bool rc = rtree_search(ctx->rtree, rect, search_iter, &ud);

  if(ud.index >= 0) {
    out_alt[0] = ud.alt;
  }
  //printf("search: rc=%d index=%d => %3.3f\n",rc,ud.index,ud.alt );
  return 0;
}

int libtopoheight_get_heightmap(struct context* ctx, const double rect[4],int width,int height,const char* filename,get_color_cb cb) {
  struct picture_t* pic = picture_create(width,height);
  if(!pic) return 1;
  int size = width*height*4;
  unsigned int* data = (unsigned int*)malloc(size); //argb
  memset(data,0,size);

  //формирование изображения
  for(int y=0;y<height;++y) {
    for(int x=0;x<width;++x) {
      //получение высоты
      double coord[2] = {
        rect[0] + (rect[2]-rect[0])*x/width,
        rect[1] + (rect[3]-rect[1])*(height-y)/height,
      };
      double alt=0;
      int rc = libtopoheight_get_alt(ctx,coord,&alt);
      //заполнение цвета пикселя
      int i = x+y*width;
      data[i] = 0;
      if(rc==0) {
        data[i] = cb ? cb(alt) : get_altitude_color(alt);
      }
      //printf("HEIGHTMAP: %d,%d rc=%d alt=%3.3f\n",x,y,rc,alt);
    }
  }
  picture_write_png(pic,data,filename);
  picture_destroy(pic);
  free(data);
}

void libtopoheight_debug_get_counts(struct context* ctx,size_t counts[3]) {
  /*counts[0] = ctx->relief.coords.size();
  counts[1] = ctx->relief.altitudes.size();
  ctl::TIN tin(ctx->dt);
  counts[2] = ctx->dt ? tin.triangles.size()*3 : 0;*/
  counts[0] = 0;
  counts[1] = 0;
  counts[2] = 0; 
}

void libtopoheight_debug_get_coords(struct context* ctx,size_t index, double coord[2]) {
  /*coord[0] = ctx->relief.coords[2*index];
  coord[1] = ctx->relief.coords[2*index+1];*/
  coord[0] = 0;
  coord[1] = 0;
}

void libtopoheight_debug_get_altitude(struct context* ctx,size_t index, double altitude[1]) {
  altitude[0] = 0;
  //altitude[0] = ctx->relief.altitudes[index];
}


#ifdef __cplusplus
}
#endif
