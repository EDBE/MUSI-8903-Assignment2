//
//  Vibrato.cpp
//  MUSI8903
//
//  Created by Music Technology on 2/4/16.
//
//
#include "MUSI8903Config.h"
#include "Vibrato.h"
#include <iostream>



Vibrato::Vibrato( int fMaxDelayInSample, int fMaxModFreq, int iNumChannels) :
_ppCRingBuffer(0),
_iNumChannels(iNumChannels)
{
    
    assert(fMaxDelayInSample > 0);
    assert(iNumChannels > 0);
    
    for (int i = 0; i < VibratoIf::kNumVibratoParams; i++) {
        _afParam[i]            = 0.F;
    }
    
//    _aafParamRange[VibratoIf::kParamDelay][0] = 0;
//    _aafParamRange[VibratoIf::kParamDelay][1] = fMaxDelayInSample;
    _aafParamRange[VibratoIf::kParamModFreq][0] = 0;
    _aafParamRange[VibratoIf::kParamModFreq][1] = fMaxModFreq;
    _aafParamRange[VibratoIf::kParamModWidth][0] = 0;
    _aafParamRange[VibratoIf::kParamModWidth][1] = fMaxDelayInSample/2;
    //Maximum modulation width cannot be greter than the fixed delay
    
    _ppCRingBuffer = new CRingBuffer<float>* [_iNumChannels];
    for (int c = 0; c < _iNumChannels; c++) {
        _ppCRingBuffer[c]  = new CRingBuffer<float>(fMaxDelayInSample);
    }
//    _pCRingBuffer = new CRingBuffer<float>( 2*fDelayInSample );
//    std::cout << "Constructor is called: " << 2*fMaxDelayInSample<<" bufferlen"<<std::endl;
}

Vibrato::~Vibrato()
{
    if (_ppCRingBuffer)
    {
        for (int c = 0; c < _iNumChannels; c++)
            delete _ppCRingBuffer[c];
    }
    delete [] _ppCRingBuffer;
}

Error_t Vibrato::resetInstance()
{
    //Reset Buffer
    for (int c = 0; c < _iNumChannels; c++)
    {
        _ppCRingBuffer[c]->reset ();
        _ppCRingBuffer[c]->setWriteIdx(CUtil::float2int<int>(_afParam[VibratoIf::kParamModWidth]));
    }
    
    return kNoError;
}

Error_t Vibrato::setParam( VibratoIf::VibratoParams eParam, float fParamValue )
{
    if (isInParamRange(eParam, fParamValue))
        return kFunctionInvalidArgsError;
    
    if (eParam == VibratoIf::kParamModWidth)
    {
        int iNumAdditionalTaps  = CUtil::float2int<int>(fParamValue - _afParam[VibratoIf::kParamModWidth]);
        if (iNumAdditionalTaps < 0)
        {
            for (int c = 0; c < _iNumChannels; c++)
            {
                _ppCRingBuffer[c]->setWriteIdx(CUtil::float2int<int>(fParamValue) + _ppCRingBuffer[c]->getReadIdx());
            }
        }
        else
        {
            
            for (int c = 0; c < _iNumChannels; c++)
            {
                for (int i = 0; i < iNumAdditionalTaps; i++)
                {
                    _ppCRingBuffer[c]->putPostInc(0.F);
                }
            }
        }
    }
    
    _afParam[eParam]   = fParamValue;
    
    return kNoError;
}

float Vibrato::getParam( VibratoIf::VibratoParams eParam ) const
{
    return _afParam[eParam];
}

bool Vibrato::isInParamRange( VibratoIf::VibratoParams eParam, float fValue )
{
    if (fValue < _aafParamRange[eParam][0] || fValue > _aafParamRange[eParam][1])
    {
        return true;
    }
    else
    {
        return false;
    }
}

Error_t Vibrato::process( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames )
{
    float mod, tap, frac;//, dummy;
    int i;
    
    for (int c = 0; c < _iNumChannels; c++) {
        for ( int n =0; n < iNumberOfFrames; n++ ) {
        
            mod = sin(_afParam[VibratoIf::kParamModFreq] * 2 * PI * (n) );

        
            tap = 1 + _afParam[VibratoIf::kParamModWidth]+ _afParam[VibratoIf::kParamModWidth]* mod;
            i   = (int) (tap+0.001);
            frac= tap - i;
        
            _ppCRingBuffer[c]->putPostInc( ppfInputBuffer[c][n] );
            
            
            float dl1 = _ppCRingBuffer[c]->getFromWrite( -i ),
                  dl2 = _ppCRingBuffer[c]->getFromWrite( -i-1 );
            
            ppfOutputBuffer[c][n] = dl2*frac + dl1*(1-frac);

        }
    }

    return kNoError;
}
