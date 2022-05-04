#include <iostream>
#include "utils.hpp"
#include "libtopoheight.h"
#include <delaunator.hpp>

struct context {
  utils::Relief relief;
  delaunator::Delaunator* delaunator;
  context() : delaunator(NULL) {};
};

#ifdef __cplusplus
extern "C" {
#endif

void* libtopoheight_create() {
  struct context* ctx = new context;
  return ctx;
}

void libtopoheight_destroy( struct context* ctx ) {
  if(ctx->delaunator) delete ctx->delaunator;
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

int libtopoheight_triangulate(struct context* ctx) {
  if( !ctx->relief.coords.size() )
    return 1;
  if( ctx->delaunator ) delete ctx->delaunator;
  ctx->delaunator = NULL;
  try {
    ctx->delaunator = new delaunator::Delaunator(ctx->relief.coords);
  }
  catch(std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    return 2;
  }
  return 0;
}

void libtopoheight_debug_get_counts(struct context* ctx,size_t counts[3]) {
  counts[0] = ctx->relief.coords.size();
  counts[1] = ctx->relief.altitudes.size();
  counts[2] = ctx->delaunator ? ctx->delaunator->triangles.size() : 0;
}

void libtopoheight_debug_get_coords(struct context* ctx,size_t index, double coord[2]) {
  coord[0] = ctx->relief.coords[2*index];
  coord[1] = ctx->relief.coords[2*index+1];
}

void libtopoheight_debug_get_altitude(struct context* ctx,size_t index, double altitude[1]) {
  altitude[0] = ctx->relief.altitudes[index];
}


#ifdef __cplusplus
}
#endif
