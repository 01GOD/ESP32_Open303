#include "rosic_BiquadFilter.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

BiquadFilter::BiquadFilter()
{
  frequency  = 1000.0f;
  gain       = 0.0f;
  bandwidth  = 2.0f*asinhf(1.0f/sqrtf(2.0f))/logf(2.0f);
  sampleRate = SAMPLE_RATE;
  mode       = BYPASS;
  calcCoeffs();
  reset();    
}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void BiquadFilter::setSampleRate(float newSampleRate)
{
  if( newSampleRate > 0.0f )
    sampleRate = newSampleRate;
  calcCoeffs();
}

void BiquadFilter::setMode(int newMode)
{
  mode = newMode; // 0:bypass, 1:Low Pass, 2:High Pass
  calcCoeffs();
}

void BiquadFilter::setFrequency(float newFrequency)
{
  frequency = newFrequency;
  calcCoeffs();
}

void BiquadFilter::setGain(float newGain)
{
  gain = newGain;
  calcCoeffs();
}

void BiquadFilter::setBandwidth(float newBandwidth)
{
  bandwidth = newBandwidth;
  calcCoeffs();
}

//-------------------------------------------------------------------------------------------------
//others:

void BiquadFilter::calcCoeffs()
{
  float w = TWOPI*frequency* DIV_SAMPLE_RATE;
  float s, c;
  switch(mode)
  {
  case LOWPASS6: 
    {
      // formula from dspguide:
      float x = exp(-w);
      a1 = x;
      a2 = 0.0f;
      b0 = 1.0f-x;
      b1 = 0.0f;
      b2 = 0.0f;
    }
    break;
  case LOWPASS12: 
    {
      // formula from Robert Bristow Johnson's biquad cookbook:
      sinCos(w, &s, &c);
      float q     = dB2amp(gain);
      float alpha = s/(2.0f*q);
      float scale = 1.0f/(1.0f+alpha);
      a1 = 2.0f*c       * scale;
      a2 = (alpha-1.0f) * scale;
      b1 = (1.0f-c)     * scale;
      b0 = 0.5f*b1;
      b2 = b0;
    }
    break;
  case HIGHPASS6: 
    {
      // formula from dspguide:
      float x = exp(-w);
      a1 = x;
      a2 = 0.0f;
      b0 = 0.5f*(1.0f+x);
      b1 = -b0;
      b2 = 0.0f;
    }
    break;
  case HIGHPASS12: 
    {
      // formula from Robert Bristow Johnson's biquad cookbook:
      sinCos(w, &s, &c);
      float q     = dB2amp(gain);
      float alpha = s/(2.0f*q);
      float scale = 1.0f/(1.0f+alpha);
      a1 = 2.0f*c       * scale;
      a2 = (alpha-1.0f) * scale;
      b1 = -(1.0f+c)    * scale;
      b0 = -0.5f*b1;
      b2 = b0;
    }
    break;
  case BANDPASS: 
    {
      // formula from Robert Bristow Johnson's biquad cookbook:      
      sinCos(w, &s, &c);
      float alpha = s * sinhf( 0.5f*logf(2.0f) * bandwidth * w / s );
      float scale = 1.0f/(1.0f+alpha);
      a1 = 2.0f*c       * scale;
      a2 = (alpha-1.0f) * scale;
      b1 = 0.0f;
      b0 = 0.5f*s       * scale;
      b2 = -b0;
    }
    break;
  case BANDREJECT: 
    {
      // formula from Robert Bristow Johnson's biquad cookbook:
      sinCos(w, &s, &c);
      float alpha = s * sinhf( 0.5f*logf(2.0f) * bandwidth * w / s );
      float scale = 1.0/(1.0+alpha);
      a1 = 2.0f*c       * scale;
      a2 = (alpha-1.0f) * scale;
      b0 = 1.0         * scale;
      b1 = -2.0f*c      * scale;
      b2 = 1.0         * scale;
    }
    break;
  case PEAK: 
    {
      // formula from Robert Bristow Johnson's biquad cookbook:
      sinCos(w, &s, &c);
      float alpha = s * sinh( 0.5*log(2.0f) * bandwidth * w / s );
      float A     = dB2amp(gain);
      float scale = 1.0f/(1.0f+alpha/A);
      a1 = 2.0f*c             * scale;
      a2 = ((alpha/A) - 1.0f) * scale;
      b0 = (1.0f+alpha*A)     * scale;
      b1 = -2.0f*c            * scale;
      b2 = (1.0f-alpha*A)     * scale;
    }
    break;
  case LOW_SHELF: 
    {
      // formula from Robert Bristow Johnson's biquad cookbook:
      sinCos(w, &s, &c);
      float A     = dB2amp(0.5*gain);
      float q     = 1.0 / (2.0*sinh( 0.5*log(2.0f) * bandwidth ));
      float beta  = sqrt(A) / q;
      float scale = 1.0 / ( (A+1.0f) + (A-1.0f)*c + beta*s);
      a1 = 2.0 *     ( (A-1.0f) + (A+1.0f)*c          ) * scale;
      a2 = -         ( (A+1.0f) + (A-1.0f)*c - beta*s ) * scale;
      b0 =       A * ( (A+1.0f) - (A-1.0f)*c + beta*s ) * scale;
      b1 = 2.0 * A * ( (A-1.0f) - (A+1.0f)*c          ) * scale;
      b2 =       A * ( (A+1.0f) - (A-1.0f)*c - beta*s ) * scale;
    }
    break;




    // \todo: implement shelving and allpass modes

  default: // bypass
    {
      b0 = 1.0f;
      b1 = 0.0f;
      b2 = 0.0f;
      a1 = 0.0f;
      a2 = 0.0f;
    }break;
  }
}

void BiquadFilter::reset()
{
  x1 = 0.0f;
  x2 = 0.0f;
  y1 = 0.0f;
  y2 = 0.0f;
}
