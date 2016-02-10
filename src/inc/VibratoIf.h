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
    
    /*! Create a pointer to VibratoIf
     \
     */
    static Error_t create (VibratoIf*& pVibratoIf);
    
    /*! Delete the pointer
     \
     */
    static Error_t destroy (VibratoIf*& pVibratoIf);
    
    /*! Initialize the Vibrato object
     \ Check the parameter is valid or not
     \ Create Vibrato object with the valid parameter
     \ The Vibrato constructor only needs delay length (in sec) and sample rate (in Hz)
     \
     */
    Error_t init (float fMaxDelayLengthInSec, float fSampleRateInHz, int iNumChannels);
    
    /*! 
     \
     \
     */
    Error_t reset ();
    
    /*! Set or change parameter of Vibrato
     \ User is able to change the Delay time (in sec)
     \                            Modulation frequency (in Hz)
     \                            Modulation Width (in sec)
     */
    Error_t setParam (VibratoParams eParam, float fParamValue);
    
    /*! Get the current paramter from Vibrato object
     */
    float   getParam (VibratoParams eParam) const;
    
    /*! Process function
     \  
     */
    Error_t process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
private:
    bool     m_bIsInitialized;
    float    m_fSampleRate;
    Vibrato* m_pVibrato;
    
    VibratoIf();
    ~VibratoIf();
};

#endif /* defined(__MUSI8903__VibratoIf__) */
