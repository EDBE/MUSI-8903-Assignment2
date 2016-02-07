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

//Vibrato::Vibrato( float fFixedDelayInSample, float modFreq, float modWidth, int iNumChannels) :
//_ppCRingBuffer(0),
//
//_fModFreqInSample( modFreq ),
//_fModWidthInSample( modWidth ),
//_iNumChannels(iNumChannels),
//_fDelayFixedInSample(fFixedDelayInSample)
//{
//    
//    assert(fFixedDelayInSample > 0);
//    assert(iNumChannels > 0);
//    
//    for (int i = 0; i < VibratoIf::kNumVibratoParams; i++) {
//        _afParam[i]            = 0.F;
//    }
//    
//    _aafParamRange[VibratoIf::kParamFixedDelay][0] = 0;
//    _aafParamRange[VibratoIf::kParamFixedDelay][1] = fFixedDelayInSample + modWidth;
//    _aafParamRange[VibratoIf::kParamModFreq][0] = 0;
//    _aafParamRange[VibratoIf::kParamModFreq][1] = 70;
//    _aafParamRange[VibratoIf::kParamModWidth][0] = 0;
//    _aafParamRange[VibratoIf::kParamModWidth][1] = fFixedDelayInSample;
//    
//    _ppCRingBuffer = new CRingBuffer<float>*[_iNumChannels];
//    for (int c = 0; c < _iNumChannels; c++) {
//        _ppCRingBuffer[c]  = new CRingBuffer<float>(ceil(fFixedDelayInSample+modWidth));
//    }
//    _pCRingBuffer = new CRingBuffer<float>( ceil(fFixedDelayInSample));
//    std::cout << "Constructor is called" << std::endl;
//}

Vibrato::Vibrato( int fFixedDelayInSample, int modFreq, int modWidth, int iNumChannels) :
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
        _ppCRingBuffer[c]  = new CRingBuffer<float>(ceil(fFixedDelayInSample+modWidth));
    }
    _pCRingBuffer = new CRingBuffer<float>( fFixedDelayInSample+modWidth );
    std::cout << "Constructor is called: " << fFixedDelayInSample+modWidth <<" bufferlen"<<std::endl;
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
    
//    for(int c = 0; c < _iNumChannels; c++) {
//        for (int n = 0; n < iNumberOfFrames; n++) {
//            mod = sin(_fModFreqInSample * 2 * PI * n);
//            std::cout<<mod<<std::endl;
//            tap = _fDelayFixedInSample + _fModWidthInSample * mod;
//            i   = tap;
//            frac= tap - i;
//            
//            
//            _ppCRingBuffer[c]->putPostInc(ppfInputBuffer[c][n]);
//            dummy = _ppCRingBuffer[c]->getPostInc();
//            ppfOutputBuffer[c][n] = _ppCRingBuffer[c]->get(i)*frac + _ppCRingBuffer[c]->get(i-1)*(1-frac);
////            std::cout << _ppCRingBuffer[c]->get(i-1) << ", " << _ppCRingBuffer[c]->get(i) << std::endl;
//        }
//    }
    
    for ( int n =0; n<iNumberOfFrames; n++ ) {
        
        mod = sin(_fModFreqInSample * 2 * PI * n / 16000);
        
        tap = _fDelayFixedInSample + _fModWidthInSample * mod;
        i   = tap;
        frac= tap - i;
        
        
//        std::cout<< mod << ", " << tap<<" Frac: "<<frac<<std::endl;
        
        _pCRingBuffer->putPostInc( ppfInputBuffer[0][n] );
        dummy = _pCRingBuffer->getPostInc();
//        std::cout<< "\t"<<dummy<<std::endl;
        
        ppfOutputBuffer[0][n] = _pCRingBuffer->get( i )*frac + _pCRingBuffer->get( i-1 )*(1-frac);
        if (ppfOutputBuffer[0][n] > 1) {
            std::cout << "peak is here" << n << std::endl;
        }
        //std::cout<<outputVec[n]<<std::endl;
//        std::cout<<_pCRingBuffer->get( i )<<", "<<_pCRingBuffer->get( i-1 )<<std::endl;
        std::cout << ppfOutputBuffer[0][n] << std::endl;
        //curDelay     = nextDelay;
        
    }

    return kNoError;
}
