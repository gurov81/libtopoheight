#include <iostream>
#include "libtopoheight.h"
#include "utils.hpp"
#include "rtree.h"
#include "picture.h"
#include "rectangle.hpp"
#include "triangle.hpp"
#include "heightmap.hpp"
#include "CDT.h"

struct context {
  utils::Relief relief;
  CDT::Triangulation<double>* cdt;
  struct rtree *rtree;
  context() : cdt(NULL),rtree(NULL) {};
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
  if(ctx->cdt) delete ctx->cdt;
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
  CDT::TriangleVec& triangles = ctx->cdt->triangles;
  if(points) {  
    points[0] = ctx->relief.points[triangles[i].vertices[0]].x;	
    points[1] = ctx->relief.points[triangles[i].vertices[0]].y;		
    points[2] = ctx->relief.points[triangles[i].vertices[1]].x;	
    points[3] = ctx->relief.points[triangles[i].vertices[1]].y;	
    points[4] = ctx->relief.points[triangles[i].vertices[2]].x;	
    points[5] = ctx->relief.points[triangles[i].vertices[2]].y;
  }
  if(alts) {
    alts[0] = ctx->relief.points[triangles[i].vertices[0]].z;
    alts[1] = ctx->relief.points[triangles[i].vertices[1]].z;
    alts[2] = ctx->relief.points[triangles[i].vertices[2]].z;
  }
}

//#define log printf
#define log(...)

struct CustomPoint2D
{
    double data[2];
};

int libtopoheight_triangulate(struct context* ctx) {
  if( !ctx->relief.points.size() )
    return 1;
  if( ctx->cdt ) delete ctx->cdt;
  ctx->cdt = NULL;
  //выполнение триангуляции
  try {    
    CDT::RemoveDuplicatesAndRemapEdges(ctx->relief.points,ctx->relief.edges);    
    ctx->cdt = new CDT::Triangulation<double>;   
    ctx->cdt->insertVertices(ctx->relief.points);    
    ctx->cdt->insertEdges(ctx->relief.edges);    
    ctx->cdt->eraseSuperTriangle();
  }
  catch(std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    return 2;
  }
  //вставка в R-дерево
  double points[6];
  double rect[4];
  double alts[3];
  const CDT::TriangleVec& triangles = ctx->cdt->triangles;
  for(size_t i=0; i<triangles.size(); i++) {
    //получение координат и высот в вершинах треугольника
    get_triangle(ctx, i, points, alts);
    //расчет прямоугольника, содержащего треугольник
    get_bounding_rect(points,rect);
    //вставка прямоугольника в R-дерево с сохранением индекса связанного с ним треугольника
    bool rc = rtree_insert(ctx->rtree,rect,&i);
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
 
  double det = (t2.y - t3.y)*(t1.x - t3.x) + (t3.x - t2.x)*(t1.y - t3.y);
  double factor_alpha = (t2.y - t3.y)*(pt.x - t3.x) + (t3.x - t2.x)*(pt.y - t3.y);
  double factor_beta = (t3.y - t1.y)*(pt.x - t3.x) + (t1.x - t3.x)*(pt.y - t3.y);  
  double alpha = factor_alpha / det;  
  double beta = factor_beta / det;
  double gamma = 1.0 - alpha - beta;  
  if (!(pt == t1 || pt == t2 || pt == t3 || (within(alpha) && within(beta) && within(gamma)))) {
  return true; }
  //треугольник найден
  ud->index = index;
  ud->alt = -1;
  const double a = (alpha*alt[0]+beta*alt[1]+gamma*alt[2])/(gamma+alpha+beta);
  ud->alt = a;
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
        data[i] = cb ? cb(alt) : get_altitude_color(alt,ctx->relief.maxAlt);
      }
    }
  }
  picture_write_png(pic,data,filename);
  picture_destroy(pic);
  free(data);
}

void libtopoheight_debug_get_counts(struct context* ctx,size_t counts[3]) {
  counts[0] = ctx->relief.points.size()*2;
  counts[1] = ctx->relief.points.size();
  counts[2] = ctx->cdt ? ctx->cdt->triangles.size()*3 : 0;
}

void libtopoheight_debug_get_coords(struct context* ctx,size_t index, double coord[2]) {
  coord[0] = ctx->relief.points[index].x;
  coord[1] = ctx->relief.points[index].y;
}

void libtopoheight_debug_get_altitude(struct context* ctx,size_t index, double altitude[1]) {
  altitude[0] = ctx->relief.points[index].z;
}
#ifdef __cplusplus
}
#endif
