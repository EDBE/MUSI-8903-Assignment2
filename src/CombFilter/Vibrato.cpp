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
    
    _aafParamRange[VibratoIf::kParamDelay][0] = 0;
    _aafParamRange[VibratoIf::kParamDelay][1] = fMaxDelayInSample;
    _aafParamRange[VibratoIf::kParamModFreq][0] = 0;
    _aafParamRange[VibratoIf::kParamModFreq][1] = fMaxModFreq;
    _aafParamRange[VibratoIf::kParamModWidth][0] = 0;
    _aafParamRange[VibratoIf::kParamModWidth][1] = fMaxDelayInSample;
    
    _ppCRingBuffer = new CRingBuffer<float>*[_iNumChannels];
    for (int c = 0; c < _iNumChannels; c++) {
        _ppCRingBuffer[c]  = new CRingBuffer<float>(ceil(2*fMaxDelayInSample));
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
        _ppCRingBuffer[c]->setWriteIdx(CUtil::float2int<int>(_afParam[VibratoIf::kParamDelay]));
    }
    
    return kNoError;
}

Error_t Vibrato::setParam( VibratoIf::VibratoParams eParam, float fParamValue )
{
    if (isInParamRange(eParam, fParamValue))
        return kFunctionInvalidArgsError;
    
    // special actions for special parameters
    if (eParam == VibratoIf::kParamDelay)
    {
        int iNumAdditionalTaps  = CUtil::float2int<int>(fParamValue - _afParam[VibratoIf::kParamDelay]);
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
    for (int c = 0; c < _iNumChannels; c++) {
        for ( int n =0; n<iNumberOfFrames; n++ ) {
        
            mod = sin(_afParam[VibratoIf::kParamModFreq] * 2 * PI * n );
        
            tap = _afParam[VibratoIf::kParamDelay]+ _afParam[VibratoIf::kParamModWidth]* mod;
            i   = (int) tap;
            frac= tap - i;

//        std::cout<< mod << ", " << tap<<" Frac: "<<frac<<std::endl;
//        std::cout<<ppfInputBuffer[0][n]<<std::endl;
        
            _ppCRingBuffer[c]->putPostInc( ppfInputBuffer[c][n] );
        
            if (ppfInputBuffer[0][n] > 1 || ppfInputBuffer[c][n] < -1) {
                std::cout << "peak input is here" << n << std::endl;
            }
            dummy = _ppCRingBuffer[c]->getPostInc();
//        std::cout<< "\t"<<dummy<<std::endl;
            float dl1 = _ppCRingBuffer[c]->get( i ),
                  dl2 = _ppCRingBuffer[c]->get( i-1 );
//        std::cout<<dl1<<" , "<<dl2<<std::endl;
            ppfOutputBuffer[0][n] = dl1*frac + dl2*(1-frac);
//        std::cout<<ppfOutputBuffer[0][n]<<std::endl;
//        if (ppfOutputBuffer[0][n] > 1 || ppfOutputBuffer[0][n] < -1) {
//            std::cout << "peak is here" << n << std::endl;
//        }
        //std::cout<<outputVec[n]<<std::endl;
//        std::cout<<_pCRingBuffer->get( i )<<", "<<_pCRingBuffer->get( i-1 )<<std::endl;
//        std::cout << ppfOutputBuffer[0][n] << std::endl;
        //curDelay     = nextDelay;
        
        }
    }

    return kNoError;
}
