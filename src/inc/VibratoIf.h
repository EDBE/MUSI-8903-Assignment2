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

class VibratoIf {
public:
    enum Version_t {
        kMajor,
        kMinor,
        kPatch,
        
        kNumVersionInts
    };
    
    enum VibratoParams{
        kParamFixedDelay,
        kParamModFreq,
        kParamModWidth,
        kNumVibratoParams
        //maybe more
    };
    
    static const int getVersion (const Version_t eVersionIdx);
    static const char* getBuildDate();
    
    void create ();
    void destroy ();
    
    void init ();
    void reset ();
    
    Error_t setParam ();
    float   getParam ();
    
    Error_t process();
    
private:
    bool   _bIsInitialized;
    float  _fSampleRate;
};

#endif /* defined(__MUSI8903__VibratoIf__) */
