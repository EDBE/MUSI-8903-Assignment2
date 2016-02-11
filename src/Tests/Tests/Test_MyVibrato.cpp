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
#include <iostream>
#include <fstream>

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
            m_iDataLength(2*8000),
            m_fMaxDelayLength(3.F),
            m_fMaxModLength(3.F),
            m_iBlockLength(171),
            m_iNumChannels(1),
            m_fSampleRate(8000),
            m_fDelay(3.f),
            m_fModFreq(10.F),
            m_fModWidth(0.5f)
        {
            VibratoIf::create(m_pVibrato);
            
            m_ppfInputData  = new float* [m_iNumChannels];
            m_ppfOutputData = new float* [m_iNumChannels];
            m_ppfInputTmp   = new float*[m_iNumChannels];
            m_ppfOutputTmp  = new float*[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfInputData[i]   = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfInputData[i], m_iDataLength);
                m_ppfOutputData[i]  = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfOutputData[i], m_iDataLength);
            }
        }
        
        ~VibratoData() {
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
            }
            delete [] m_ppfOutputTmp;
            delete [] m_ppfInputTmp;
            delete [] m_ppfOutputData;
            delete [] m_ppfInputData;
            
            VibratoIf::destroy(m_pVibrato);
        }
        
        void TestProcess()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);
                
                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                    m_ppfOutputTmp[c]   = &m_ppfOutputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pVibrato->process(m_ppfInputTmp, m_ppfOutputTmp, iNumFrames);
                
                iNumFramesRemaining -= iNumFrames;
            }

        }
        void TestProcessInplace()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);
                
                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pVibrato->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);
                
                iNumFramesRemaining -= iNumFrames;
            }
        }
        
        VibratoIf   *m_pVibrato;
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
                    m_fModFreq,
                    m_fModWidth;

    };
    
    TEST_FIXTURE(VibratoData, ZeroInput) {
        m_pVibrato->init(m_fMaxDelayLength, m_fSampleRate, m_iNumChannels);
        m_pVibrato->setParam(VibratoIf::kParamModFreq, m_fModFreq);
        m_pVibrato->setParam(VibratoIf::kParamModWidth, m_fModWidth);
        
        TestProcess();
        
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);
        
        m_pVibrato->reset();
    }
    
    TEST_FIXTURE(VibratoData, ZeroModWidth) {
        m_pVibrato->init(m_fMaxDelayLength, m_fSampleRate, m_iNumChannels);
        m_pVibrato->setParam(VibratoIf::kParamModFreq, m_fModFreq);
        m_pVibrato->setParam(VibratoIf::kParamModWidth, 0);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
          CVectorFloat::setValue(m_ppfInputData[i], 1.f, m_iDataLength);        
        }
        
        TestProcess();
        
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);

        
        m_pVibrato->reset();
    }

//    TEST_FIXTURE(VibratoData, DC2DC) {
//        m_pVibrato->reset();
//        
//        m_pVibrato->init(m_fMaxDelayLength, m_fSampleRate, m_iNumChannels);
//        m_pVibrato->setParam(VibratoIf::kParamModFreq, 10);
//        m_pVibrato->setParam(VibratoIf::kParamModWidth, m_fModWidth);
//        
//
//        for (int i = 0; i < m_iNumChannels; i++)
//        {
//            CVectorFloat::setValue(m_ppfInputData[i], 1.f, m_iDataLength);
//
//        }
//
//
//        TestProcess();
//        
//        for(int c=0; c<m_iNumChannels; c++) {
//            std::cout<<"\nNew Channel\n";
//            for (int sample=0; sample<m_iDataLength; sample++) {
//                std::cout<<m_ppfInputData[c][sample]<<"   "<<m_ppfOutputData[c][sample]<<std::endl;
//            }
//        }
//        
//        
//        for (int c = 0; c < m_iNumChannels; c++)
//            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);
//        
//
//    }
    
    TEST_FIXTURE(VibratoData, impulse) {
        m_pVibrato->reset();
        
        m_pVibrato->init(m_fMaxDelayLength, m_fSampleRate, m_iNumChannels);
        m_pVibrato->setParam(VibratoIf::kParamModFreq, m_fModFreq);
        m_pVibrato->setParam(VibratoIf::kParamModWidth, m_fModWidth);
        
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
              m_ppfInputData[i][12] = 1;
            
        }

        
        TestProcess();
        
        float **_ppfReferenceData = new float*[m_iNumChannels];
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            _ppfReferenceData[i] = new float[m_iDataLength];
            CVectorFloat::setZero(_ppfReferenceData[i], m_iDataLength);
            _ppfReferenceData[i][ 13 + (int)(m_fModWidth*m_fSampleRate) ] = 0.7071f;
            
        }
        
        
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(_ppfReferenceData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);
    
        
    }
    
    
    TEST_FIXTURE(VibratoData, sineWaveTest) {
        m_pVibrato->reset();
        
        m_pVibrato->init(m_fMaxDelayLength, m_fSampleRate, m_iNumChannels );
        m_pVibrato->setParam(VibratoIf::kParamModFreq, m_fModFreq);
        m_pVibrato->setParam(VibratoIf::kParamModWidth, m_fModWidth);

        
        CSynthesis::generateSine (m_ppfInputData[0], 20, m_fSampleRate, m_iDataLength, .8F, static_cast<float>(0*M_PI_2));
        
        TestProcess();
        
        std::ofstream outFile("/Users/Rithesh/Desktop/testSamples/myOut.txt"),
                      inFile("/Users/Rithesh/Desktop/testSamples/sineIn.txt");

        for(int c=0; c<m_iNumChannels; c++) {
        
           for (int sample=0; sample<m_iDataLength; sample++) {
               outFile<<m_ppfOutputData[c][sample]<<std::endl;
               inFile<<m_ppfInputData[c][sample]<<std::endl;
           }
        }
        
        outFile.close();
        

    }


}

#endif