#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define FEEP_VOLUME 0

/* Include routines to do parsing */

#include <stdint.h>
#include <cassert>

#include <png.h>


#define FFMIN(a,b) (((a)<(b))?(a):(b))
#define FFMAX(a,b) (((a)>(b))?(a):(b))

struct png_write_context {
    char* buf;
    int size;
    int allocated;
    png_write_context() : buf(0),size(0),allocated(100*1000) {
        buf = (char*)malloc(allocated);
    }
    ~png_write_context() {
        free(buf);
    }
    void reset() { size = 0; }
    static void callback(png_structp  png_ptr, png_bytep data, png_size_t length) {
        png_write_context* self = (png_write_context*)png_get_io_ptr(png_ptr);
        if( self->size+length>self->allocated ) {
            self->allocated = self->size+length*2;
            self->buf = (char*)realloc(self->buf,self->allocated);
        }
        memcpy(self->buf+self->size,data,length);
        self->size+=length;
    }
};

struct picture_t {
    int width;
    int height;
    struct {
        png_write_context* ctx;
        png_bytep row;
    } png;
};


extern "C" picture_t* picture_create(int width,int height) {
    picture_t* self = new picture_t;
    self->width = width;
    self->height = height;
    self->png.ctx = 0;
    self->png.row = (png_bytep) malloc (3 * self->width * sizeof (png_byte));
    return self;
}

extern "C" void picture_destroy(picture_t* self) {
    free(self->png.row);
    delete self->png.ctx;
    delete self;
}

extern "C" /*const*/ char* picture_get(picture_t* self,int type,int* size) {
    (void)type;
     *size = self->png.ctx ? self->png.ctx->size : 0;
     return self->png.ctx ? self->png.ctx->buf : 0;
}

extern "C" void picture_write_png(picture_t* self,unsigned int* data,const char* path)
{
  int code = 0;
  FILE *fp = 0;
  png_structp png_ptr;
  png_infop png_info_ptr;
  png_bytep png_row;
  int width = self->width;
  int height = self->height;

  // Open file
  if(path) {
      fp = fopen (path, "wb");
      if (fp == NULL){
        fprintf (stderr, "Could not open file for writing\n");
        code = 1;
      }
  }
  else {
      if(!self->png.ctx) self->png.ctx = new png_write_context;
  }

  // Initialize write structure
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL){
    fprintf (stderr, "Could not allocate write struct\n");
    code = 1;
  }

  // Initialize info structure
  png_info_ptr = png_create_info_struct (png_ptr);
  if (png_info_ptr == NULL){
    fprintf (stderr, "Could not allocate info struct\n");
    code = 1;
  }

  // Setup Exception handling
  if (setjmp (png_jmpbuf (png_ptr))){
    fprintf(stderr, "Error during png creation\n");
    code = 1;
  }

  if(fp) {
    png_init_io (png_ptr, fp);
  }
  else {
    self->png.ctx->reset();
    png_set_write_fn(png_ptr, self->png.ctx, png_write_context::callback, NULL);
  }

  // Write header (8 bit colour depth)
  png_set_IHDR (png_ptr, png_info_ptr, width, height,
     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // Set title
  char text[] = "Screenshot";
  char key[] = "Title";
  png_text title_text;
  title_text.compression = PNG_TEXT_COMPRESSION_NONE;
  title_text.key = key;
  title_text.text = text;
  png_set_text (png_ptr, png_info_ptr, &title_text, 1);

  png_write_info (png_ptr, png_info_ptr);

  // Allocate memory for one row (3 bytes per pixel - RGB)
  png_row = self->png.row;

  const unsigned long blue_mask  = 0x000000ff;
  const unsigned long green_mask = 0x0000ff00;
  const unsigned long red_mask   = 0x00ff0000;
  // Write image data
  int x, y;
  for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
        unsigned long pixel = data[y*width+x];//XGetPixel (image, x, y);
        unsigned char blue = pixel & blue_mask;
        unsigned char green = (pixel & green_mask) >> 8; 
        unsigned char red = (pixel & red_mask) >> 16;
        png_byte *ptr = &(png_row[x*3]);
        ptr[0] = red;
        ptr[1] = green;
        ptr[2] = blue;
    }
    png_write_row (png_ptr, png_row);
  }

  // End write
  png_write_end (png_ptr, NULL);

  // Free
  if (png_info_ptr != NULL) png_free_data (png_ptr, png_info_ptr, PNG_FREE_ALL, -1);
  if (png_ptr != NULL) png_destroy_write_struct (&png_ptr, (png_infopp)NULL);

  if(fp) {
    fclose (fp);
  }
}
