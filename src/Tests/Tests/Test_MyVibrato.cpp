//
//  Test_MyVibrato.cpp
//  MUSI8903
//
//  Created by Music Technology on 2/8/16.
//
//
#include "MUSI8903Config.h"
#ifdef WITH_TESTS

#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "Synthesis.h"
#include "Vector.h"
#include "VibratoIf.h"

SUITE(Vibrato) {
    struct VibratoData {
        VibratoData():
            m_pVibrato(0),
            m_ppfInputData(0),
            m_ppfOutputData(0),
            m_iDataLength(35131),
            m_fMaxDelayLength(3.F),
            m_fMaxModLength(3.F),
            m_iBlockLength(171),
            m_iNumChannels(3),
            m_fSampleRate(8000),
            m_fDelay(.1F),
            m_fModFreq(.1F)
        {
            VibratoIf::create(m_pVibrato);
            
            m_ppfInputData = new float* [m_iNumChannels];
            m_ppfOutputData = new float* [m_iNumChannels];
            
        }
        
        ~VibratoData() {
            
        }
        
        void TestProcess() {}
        
        VibratoIf *m_pVibrato;
        float       **m_ppfInputData,
                    **m_ppfOutputData,
                    **m_ppfInputTmp,
                    **m_ppfOutputTmp;
        int         m_iDataLength;
        float       m_fMaxDelayLength,
                    m_fMaxModLength;
        int         m_iBlockLength;
        int         m_iNumChannels;
        float       m_fSampleRate,
                    m_fDelay,
                    m_fModFreq;


    };
}

#endif