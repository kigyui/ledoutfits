CRGB h1v_to_rgb(byte inh, byte inv)
{
  CRGB rgb;
  unsigned char region, p, q, t;
  unsigned int h, s, v, remainder;

  h = inh;
  v = inv;
  region = h / 43;
  remainder = (h - (region * 43)) * 6;
  // we know s is always full, so just cheat save some cpu
  p = 0;
  q = (v * (255 - remainder)) >> 8;
  t = (v * remainder) >> 8;

  switch (region)  {
    case 0:
      rgb.r = v;            rgb.g = t;            rgb.b = p;
      break;
    case 1:
      rgb.r = q;            rgb.g = v;            rgb.b = p;
      break;
    case 2:
      rgb.r = p;            rgb.g = v;            rgb.b = t;
      break;
    case 3:
      rgb.r = p;            rgb.g = q;            rgb.b = v;
      break;
    case 4:
      rgb.r = t;            rgb.g = p;            rgb.b = v;
      break;
    default:
      rgb.r = v;            rgb.g = p;            rgb.b = q;
      break;
  }
  return rgb;
}


