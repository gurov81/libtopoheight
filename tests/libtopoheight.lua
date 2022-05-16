local ffi = require 'ffi'

ffi.cdef [[

struct context;

typedef unsigned int (*get_color_cb)(double);

struct context* libtopoheight_create();
void libtopoheight_destroy( struct context* ctx );

int libtopoheight_load_file(struct context* ctx, const char* filename, const char* propname);
int libtopoheight_load_buffer(struct context* ctx, const char* buf, size_t size, const char* propname);

int libtopoheight_triangulate(struct context* ctx);

int libtopoheight_get_alt(struct context* ctx,const double coord[2], double out_alt[1]);
int libtopoheight_get_heightmap(struct context* ctx, const double rect[4],int width,int height,const char* filename,get_color_cb);

void libtopoheight_debug_get_counts(struct context* ctx,size_t counts[3]);
void libtopoheight_debug_get_coords(struct context* ctx,size_t index, double coord[2]);
void libtopoheight_debug_get_altitude(struct context* ctx,size_t index, double altitude[1]);

]]

local lib = assert(ffi.load("../build/topoheight.so"),"topoheight.so not found")

local mt = {
  create      = lib.libtopoheight_create,
  destroy     = lib.libtopoheight_destroy,
  load_file   = function(self,file,prop)
    return lib.libtopoheight_load_file(self,file,prop)
  end,
  load_buffer = function(self,buf,size,prop)
    size = size or #buf
    return lib.libtopoheight_load_buffer(self,buf,size,prop)
  end,
  triangulate = lib.libtopoheight_triangulate,
  get_alt = function(self,coord_x,coord_y)
    local tmp = ffi.new('double[2]')
    local out = ffi.new('double[1]')
    tmp[0],tmp[1] = coord_x,coord_y
    local rc = lib.libtopoheight_get_alt(self,tmp,out)
    return rc, tonumber(out[0])
  end,
  get_heightmap = function(self,rect,width,height,path,get_color_cb)
    local tmp = ffi.new("double[4]")
    tmp[0],tmp[1],tmp[2],tmp[3] = rect[1],rect[2],rect[3],rect[4]
    local rc = lib.libtopoheight_get_heightmap(self,tmp,width,height,path,get_color_cb)
    return rc
  end,
  debug_get_counts = function(self)
    local tmp = ffi.new('size_t[3]')
    lib.libtopoheight_debug_get_counts(self, tmp)
    local coords, altitudes, triangles = tonumber(tmp[0]), tonumber(tmp[1]), tonumber(tmp[2])
    return coords, altitudes, triangles
  end,
  debug_get_coords = function(self,index)
    local tmp = ffi.new('double[2]')
    lib.libtopoheight_debug_get_coords(self,index,tmp)
    return tonumber(tmp[0]), tonumber(tmp[1])
  end,
  debug_get_altitude = function(self,index)
    local tmp = ffi.new('double[1]')
    lib.libtopoheight_debug_get_altitude(self,index,tmp)
    return tonumber(tmp[0])
  end,
}
function mt.new()
  local ctx = mt.create()
  if ctx == nil then return end
  return ctx
end

mt.__index = mt
ffi.metatype("struct context",mt)

return mt
