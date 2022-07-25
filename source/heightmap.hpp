#ifndef __HEIGHTMAP_HPP__
#define __HEIGHTMAP_HPP__

inline unsigned int get_altitude_color(double alt, double maxAlt) {
  int r=0,g=0,b=0;

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
