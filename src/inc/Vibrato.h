//
//  Vibrato.h
//  MUSI8903
//
//  Created by Music Technology on 2/4/16.
//
//

#ifndef __MUSI8903__Vibrato__
#define __MUSI8903__Vibrato__

#include "ErrorDef.h"
#include "Util.h"
#include "RingBuffer.h"
#include "VibratoIf.h"
#include <math.h>

#define PI 3.14159265358f

class Vibrato
{
public:

    Vibrato (int fMaxDelayInSample, float fMaxModFreq, int iNumChannels);
    ~Vibrato ();
    
    Error_t resetInstance ();
    
    Error_t setParam (VibratoIf::VibratoParams eParam, float fParamValue);
    float   getParam (VibratoIf::VibratoParams eParam) const;
    
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
private:
    CRingBuffer<float>  **_ppCRingBuffer;
    
    float   _afParam[VibratoIf::kNumVibratoParams];
    float   _aafParamRange[VibratoIf::kNumVibratoParams][2];

    int     _iNumChannels;

    Vibrato(const Vibrato& rhs);
    
    bool    isInParamRange (VibratoIf::VibratoParams, float fValue);
};


#endif /* defined(__MUSI8903__Vibrato__) */
