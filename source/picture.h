#ifndef __PICTURE_H__INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

struct picture_t;
typedef struct picture_t picture_t;

struct picture_t* picture_create(int width,int height);
void picture_destroy(struct picture_t* self);
/*const*/ char* picture_get(struct picture_t* self,int type,int* size);

void picture_write_png(struct picture_t* self,unsigned int* data,const char* path);


#ifdef __cplusplus
}
#endif

#endif //__PICTURE_H__INCLUDED

