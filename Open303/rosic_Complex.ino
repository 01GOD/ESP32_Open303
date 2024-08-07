#include "rosic_Complex.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

Complex::Complex()
{
  re = im = 0.0;
}

Complex::Complex(float reInit)
{
  re = reInit;
  im = 0.0;
}

Complex::Complex(float reInit, float imInit)
{
  re = reInit;
  im = imInit;
}

Complex::~Complex()
{

}

//-------------------------------------------------------------------------------------------------
// magnitude, angle, etc.

float Complex::getRadius()
{
  return sqrt(re*re + im*im);
}

float Complex::getAngle()
{
  if((re==0.0) && (im==0))
    return 0.0;
  else
    return atan2(im, re);
}

void Complex::setRadius(float newRadius)
{
  float phi = getAngle();
  sinCos(phi, &im, &re);
  re *= newRadius;           // re = newRadius * cos(phi);
  im *= newRadius;           // im = newRadius * sin(phi);
}

void Complex::setAngle(float newAngle)
{
  float r = getRadius();
  sinCos(newAngle, &im, &re);
  re *= r;                   // re = r * cos(newAngle);
  im *= r;                   // im = r * sin(newAngle);
}

void Complex::setRadiusAndAngle(float newRadius, float newAngle)
{
  sinCos(newAngle, &im, &re);
  re *= newRadius;           // re = newRadius * cos(newAngle);
  im *= newRadius;           // im = newRadius * sin(newAngle);
}

Complex Complex::getConjugate()
{
  return Complex(re, -im);
}

Complex Complex::getReciprocal()
{
  float scaler = 1.0 / (re*re + im*im);
  return Complex(scaler*re, -scaler*im);
}

bool Complex::isReal()
{
  return (im == 0.0);
}

bool Complex::isImaginary()
{
  return (re == 0.0);
}

bool Complex::isInfinite()
{
  if( re == INF || re == NEG_INF || im == INF || im == NEG_INF )
    return true;
  else
    return false;
}
