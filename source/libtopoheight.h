#ifdef __LIBTOPOHEIGHT_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

struct context;

struct context* libtopoheight_create();
void libtopoheight_destroy( struct context* ctx );

int libtopoheight_load_file(struct context* ctx, const char* filename, const char* propname);
int libtopoheight_load_buffer(struct context* ctx, const char* buf, size_t size, const char* propname);

int libtopoheight_triangulate(struct context* ctx);

void libtopoheight_debug_get_counts(struct context* ctx,size_t counts[3]);
void libtopoheight_debug_get_coords(struct context* ctx,size_t index, double coord[2]);
void libtopoheight_debug_get_altitude(struct context* ctx,size_t index, double altitude[1]);

#ifdef __cplusplus
}
#endif

#endif //__LIBTOPOHEIGHT_H__INCLUDED__
