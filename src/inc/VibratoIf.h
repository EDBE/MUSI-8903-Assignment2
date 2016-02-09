//
//  VibratoIf.h
//  MUSI8903
//
//  Created by Music Technology on 2/5/16.
//
//

#ifndef __MUSI8903__VibratoIf__
#define __MUSI8903__VibratoIf__

#include <stdio.h>
#include "ErrorDef.h"

class Vibrato;

class VibratoIf {
public:
    enum Version_t {
        kMajor,
        kMinor,
        kPatch,
        
        kNumVersionInts
    };
    
    enum VibratoParams{
        kParamDelay,
        kParamModFreq,
        kParamModWidth,
        kNumVibratoParams
        //maybe more
    };
    
    static const int getVersion (const Version_t eVersionIdx);
    static const char* getBuildDate();
    
    static Error_t create (VibratoIf*& pVibratoIf);
    static Error_t destroy (VibratoIf*& pVibratoIf);
    
    Error_t init (float fMaxDelayLengthInSec, float fSampleRateInHz, int iNumChannels);
    Error_t reset ();
    
    Error_t setParam (VibratoParams eParam, float fParamValue);
    float   getParam (VibratoParams eParam) const;
    
    Error_t process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
private:
    bool     m_bIsInitialized;
    float    m_fSampleRate;
    Vibrato* m_pVibrato;
    
    VibratoIf();
    ~VibratoIf();
};

#endif /* defined(__MUSI8903__VibratoIf__) */
