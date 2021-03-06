
#include <iostream>
#include <ctime>

#include "MUSI8903Config.h"

#include "AudioFileIf.h"
#include "Vibrato.h"
#include <Vector>

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();
//Vibrato small;
/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize          = 1024;

    clock_t                 time                = 0;

    float                   **ppfAudioData      = 0;
    float                   **ppfAudioOut      = 0;
    CAudioFileIf            *phAudioFile        = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    showClInfo ();

    VibratoIf *_pVibrato;
    VibratoIf::create(_pVibrato);
    
    
    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        return -1;
    }
    else
    {
        sInputFilePath  = argv[1] ;
        sInputFilePath += ".wav";
        sOutputFilePath = argv[1];
        sOutputFilePath +=  "Out.txt";
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open (sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData            = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float [kBlockSize];
    
    ppfAudioOut            = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioOut[i] = new float [kBlockSize];

    time                    = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    
    _pVibrato->init(3.F, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    
    _pVibrato->setParam(VibratoIf::kParamModFreq, 5.F);
    _pVibrato->setParam(VibratoIf::kParamModWidth, .01F);

    
    while ( !phAudioFile->isEof() ) {
    
        long long iNumFrames = kBlockSize;
//        phAudioFile->getLength(iNumFrames);
        phAudioFile->readData(ppfAudioData, iNumFrames );


        _pVibrato -> process(ppfAudioData, ppfAudioData, iNumFrames);

    
        for ( int sample = 0; sample < iNumFrames; sample++ ) {
            for ( int channel = 0; channel < stFileSpec.iNumChannels; channel++ ) {
                hOutputFile << ppfAudioData[channel][sample]<<"\t";
            }
            hOutputFile << "\n";
        }
    }
    
    

    cout << "reading/writing done in: \t"    << (clock()-time)*1.F/CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfAudioData[i];
    delete [] ppfAudioData;
    ppfAudioData = 0;

    return 0;
   
}


void     showClInfo()
{
    cout << "GTCMT MUSI8903" << endl;
    cout << "(c) 2016 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

