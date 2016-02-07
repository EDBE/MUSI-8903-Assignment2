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

Vibrato::Vibrato( float fFixedDelayInSample, float modFreq, float modWidth, int iNumChannels) :
_ppCRingBuffer(0),

_fModFreqInSample( modFreq ),
_fModWidthInSample( modWidth ),
_iNumChannels(iNumChannels),
_fDelayFixedInSample(fFixedDelayInSample)
{
    
    assert(fFixedDelayInSample > 0);
    assert(iNumChannels > 0);
    
    for (int i = 0; i < VibratoIf::kNumVibratoParams; i++) {
        _afParam[i]            = 0.F;
    }
    
    _aafParamRange[VibratoIf::kParamFixedDelay][0] = 0;
    _aafParamRange[VibratoIf::kParamFixedDelay][1] = fFixedDelayInSample + modWidth;
    _aafParamRange[VibratoIf::kParamModFreq][0] = 0;
    _aafParamRange[VibratoIf::kParamModFreq][1] = 70;
    _aafParamRange[VibratoIf::kParamModWidth][0] = 0;
    _aafParamRange[VibratoIf::kParamModWidth][1] = fFixedDelayInSample;
    
    _ppCRingBuffer = new CRingBuffer<float>*[_iNumChannels];
    for (int c = 0; c < _iNumChannels; c++) {
        _ppCRingBuffer[c]  = new CRingBuffer<float>(ceil(fFixedDelayInSample));
    }
    std::cout << "Constructor is called" << std::endl;
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
        _ppCRingBuffer[c]->setWriteIdx(CUtil::float2int<int>(_afParam[VibratoIf::kParamFixedDelay]));
    }
    
    return kNoError;
}

Error_t Vibrato::setParam( VibratoIf::VibratoParams eParam, float fParamValue )
{
    if (isInParamRange(eParam, fParamValue))
        return kFunctionInvalidArgsError;
    
    // special actions for special parameters
    if (eParam == VibratoIf::kParamFixedDelay)
    {
        int iNumAdditionalTaps  = CUtil::float2int<int>(fParamValue - _afParam[VibratoIf::kParamFixedDelay]);
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
        return false;
    }
    else
    {
        return true;
    }
}

Error_t Vibrato::process( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames )
{
    float mod, tap, frac, dummy;
    int i;
    
    for(int c = 0; c < _iNumChannels; c++) {
        for (int n = 0; n < iNumberOfFrames; n++) {
            mod = sinf(_fModFreqInSample * 2 * PI * n);
            tap = _fDelayFixedInSample + _fModWidthInSample * mod;
            i   = tap;
            frac= tap - i;
            
            _ppCRingBuffer[c]->putPostInc(ppfInputBuffer[c][n]);
            dummy = _ppCRingBuffer[c]->getPostInc();
            ppfOutputBuffer[c][n] = _ppCRingBuffer[c]->get(i)*frac + _ppCRingBuffer[c]->get(i-1)*(1-frac);
        }
    }
    

    return kNoError;
}
