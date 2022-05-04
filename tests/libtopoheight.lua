local ffi = require 'ffi'

ffi.cdef [[

struct context;

struct context* libtopoheight_create();
void libtopoheight_destroy( struct context* ctx );

int libtopoheight_load_file(struct context* ctx, const char* filename);
int libtopoheight_load_buffer(struct context* ctx, const char* buf, size_t size);

int libtopoheight_triangulate(struct context* ctx);

void libtopoheight_debug_get_counts(struct context* ctx,size_t counts[3]);
void libtopoheight_debug_get_coords(struct context* ctx,size_t index, double coord[2]);
void libtopoheight_debug_get_altitude(struct context* ctx,size_t index, double altitude[1]);

]]

local lib = assert(ffi.load("../build/topoheight.so"),"topoheight.so not found")

local mt = {
  create      = lib.libtopoheight_create,
  destroy     = lib.libtopoheight_destroy,
  load_file   = lib.libtopoheight_load_file,
  load_buffer = function(self,buf,size)
    size = size or #buf
    return lib.libtopoheight_load_buffer(self,buf,size)
  end,
  triangulate = lib.libtopoheight_triangulate,
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
