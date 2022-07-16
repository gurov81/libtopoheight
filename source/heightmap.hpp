#ifndef __HEIGHTMAP_HPP__
#define __HEIGHTMAP_HPP__

inline unsigned int get_altitude_color(double alt, double maxAlt) {
  int r=0,g=0,b=0;

#if 0
  //https://github.com/Viglino/ol-ext/blob/master/src/util/imagesLoader.js#L279
  int h = (int)(alt*1000 + 1200000);
  r = (h >> 16) & 0xff;
  g = ((h % 65536) >> 8) & 0xff;
  b = h % 256;
#endif

#if 0
  h = alt/200*65536;
  r = (h >> 8) & 0xff;
  g = h & 0xff;
  b = 0;
#endif

#if 0
  if(alt<0) r=0;
  else if(alt>1000) r=0xff;
  else r = 0xff*(alt/1000.0);
#endif

//r = (int)(((double)alt/(256*5))*0xff) & 0xff;

// 16 бит вариант
#if 1
  return (int)(((double)alt/maxAlt)*65536);
#endif

// 24бит вариант
#if 0
  if (alt < (maxAlt/3)){
    r = (int)(((double)alt*3/maxAlt)*255);
  }
  if (alt >= (maxAlt/3) && alt < (2*maxAlt/3)){
    g = (int)(((double)alt*3/(maxAlt*2))*255);
  }
  if (alt >= (2*maxAlt/3)){
    b = (int)(((double)alt/maxAlt)*255);
  }
  
  /*	
  const unsigned long blue_mask  = 0x000000ff;
  const unsigned long green_mask = 0x0000ff00;
  const unsigned long red_mask   = 0x00ff0000;
  */
  unsigned int pixel = b | (g<<8) | (r<<16); //0xAARRGGBB
  return pixel;
#endif
}

#endif //__HEIGHTMAP_HPP__
