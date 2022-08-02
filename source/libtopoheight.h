#ifndef __LIBTOPOHEIGHT_H__INCLUDED__
#define __LIBTOPOHEIGHT_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

struct context;

//функция-колбек получения пикселя по значению высоты
typedef unsigned int (*get_color_cb)(double);

//создание и удаление контекста
struct context* libtopoheight_create();
void libtopoheight_destroy( struct context* ctx );

//загрузка GeoJSON данных (из файла и из буфера в памяти)
//propname - если задано, имя свойства, откуда брать значение высоты; если не задано, берется 3-я координата
int libtopoheight_load_file(struct context* ctx, const char* filename, const char* propname);
int libtopoheight_load_buffer(struct context* ctx, const char* buf, size_t size, const char* propname);

//выполнение триангуляции и вставка данных в R-дерево
int libtopoheight_triangulate(struct context* ctx);

//получение значения высоты
int libtopoheight_get_alt(struct context* ctx,const double coord[2], double out_alt[1]);

//генерация heightmap
int libtopoheight_get_heightmap(struct context* ctx, const double rect[4],int width,int height,const char* filename,get_color_cb cb);

//получение вертикального профиля
int libtopoheight_get_vertical_profile(struct context* ctx, const double trajectory[],const int size,const int accuracy,const double deviation);

//отладочные функции
void libtopoheight_debug_get_counts(struct context* ctx,size_t counts[3]);
void libtopoheight_debug_get_coords(struct context* ctx,size_t index, double coord[2]);
void libtopoheight_debug_get_altitude(struct context* ctx,size_t index, double altitude[1]);

#ifdef __cplusplus
}
#endif

#endif //__LIBTOPOHEIGHT_H__INCLUDED__
