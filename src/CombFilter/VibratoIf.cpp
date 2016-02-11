//
//  VibratoIf.cpp
//  MUSI8903
//
//  Created by Music Technology on 2/5/16.
//
//
#include "MUSI8903Config.h"
#include "VibratoIf.h"
#include "Vibrato.h"
#include "ErrorDef.h"
#include "Util.h"
#include "RingBuffer.h"

static const char*  kCMyProjectBuildDate             = __DATE__;


VibratoIf::VibratoIf () :
m_bIsInitialized(false),
m_fSampleRate(0),
m_pVibrato(0)
{
    this->reset ();
}

VibratoIf::~VibratoIf ()
{
    this->reset ();
}

const int  VibratoIf::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;
    
    switch (eVersionIdx)
    {
        case kMajor:
            iVersion    = MUSI8903_VERSION_MAJOR;
            break;
        case kMinor:
            iVersion    = MUSI8903_VERSION_MINOR;
            break;
        case kPatch:
            iVersion    = MUSI8903_VERSION_PATCH;
            break;
        case kNumVersionInts:
            iVersion    = -1;
            break;
    }
    
    return iVersion;
}
const char*  VibratoIf::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t VibratoIf::create( VibratoIf*& pCMyProject )
{
    pCMyProject = new VibratoIf ();
    
    if (!pCMyProject)
        return kUnknownError;
    
    
    return kNoError;
}

Error_t VibratoIf::destroy (VibratoIf*& pCMyProject)
{
    if (!pCMyProject)
        return kUnknownError;
    
    pCMyProject->reset ();
    
    delete pCMyProject;
    pCMyProject = 0;
    
    return kNoError;
    
}

Error_t VibratoIf::init( float fMaxDelayLengthInSec, float fSampleRateInHz, int iNumChannels )
{
    reset();
    
    if (fMaxDelayLengthInSec <= 0 ||
        fSampleRateInHz <= 0 ||
        iNumChannels <= 0)
        return kFunctionInvalidArgsError;
    
    m_fSampleRate       = fSampleRateInHz;
    m_bIsInitialized    = true;
    
    m_pVibrato = new Vibrato( CUtil::float2int<int>(fMaxDelayLengthInSec*fSampleRateInHz),
                              CUtil::float2int<int>(1000/fSampleRateInHz),
                              iNumChannels);
    
    
    return kNoError;
}

Error_t VibratoIf::reset ()
{
    delete m_pVibrato;
    m_pVibrato      = 0;
    
    m_fSampleRate       = 0;
    m_bIsInitialized    = false;
    
    return kNoError;
}

Error_t VibratoIf::process( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames )
{
    if (!m_bIsInitialized)
        return kNotInitializedError;
    
    return m_pVibrato->process(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
}

Error_t VibratoIf::setParam( VibratoParams eParam, float fParamValue )
{
    if (!m_bIsInitialized)
        return kNotInitializedError;
    
    switch (eParam)
    {
//        case kParamDelay:
//            return m_pVibrato->setParam(eParam, fParamValue * m_fSampleRate);
        case kParamModFreq:
            return m_pVibrato->setParam(eParam, fParamValue / m_fSampleRate);
        default:
        case kParamModWidth:
            return m_pVibrato->setParam(eParam, fParamValue * m_fSampleRate );
    }
    
}

float VibratoIf::getParam( VibratoParams eParam ) const
{
    if (!m_bIsInitialized)
        return kNotInitializedError;
    
    switch (eParam)
    {
//        case kParamDelay:
//            return m_pVibrato->getParam(eParam) / m_fSampleRate;
        default:
        case kParamModFreq:
            return m_pVibrato->getParam (eParam) * m_fSampleRate;
        case kParamModWidth:
            return m_pVibrato->getParam(eParam) / m_fSampleRate;
    }
}
