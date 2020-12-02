// stkshell.cpp
//
// A simple "shell" of an STK program that outputs noise using RtWvOut.
//
// To compile on an OS-X system, just type "make" from within the
// directory where this file is found.

// WARNING: no error checking here!

#include "RtAudio.h"
#include "DelayA.h"
#include "Delay.h"
#include "Noise.h"
#include "OnePole.h"
#include "FileWvOut.h"
#include <cmath>
#include "SineWave.h"
#include "Cubic.h"

using namespace stk;

OnePole Bandwidth;
OnePole Damping1;
OnePole Damping2;
OnePole Damping3;
DelayA PreDelay;
DelayA TankDelay_[4];
DelayA Input_Diffusion1_[2];
DelayA Input_Diffusion2_[2];
DelayA Decay_Diffusion1_[2];
DelayA Decay_Diffusion2_[2];
Cubic cubic;

static StkFloat InTemp = 0;
static StkFloat InTempL = 0;
static StkFloat InTempR = 0;
static StkFloat OutTempL = 0;
static StkFloat OutTempR = 0;

static StkFloat FBTemp = 0;
static StkFloat DecayTemp_[2] = {0, 0};
static StkFloat temp, temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7;

StkFloat lengths[12] = {142, 107, 379, 277, 672, 1800, 908, 2656, 4453, 3720, 4217, 3163};
int taps[14] = {266, 2974, 1913, 1996, 1990, 187, 1066, 353, 3627, 1228, 2673, 2111, 335, 121};

StkFloat InputGain = 0.9;

StkFloat DistortionMix = 1;
StkFloat VerbMix = 0.1;

StkFloat InputDiffusionConstant1 = 0.75;
StkFloat InputDiffusionConstant2 = 0.625;
StkFloat DecayDiffusionConstant1 = 0.7;
StkFloat DecayDiffusionConstant2 = 0.5;
StkFloat Decay = 1.1;
StkFloat BandWidth = 1;
StkFloat Damping = 0.005 ;
StkFloat ExcursionMS = 0.5376163435;

StkFloat BandWidth_A1 = -(1.0 - BandWidth);
StkFloat Damping_B0 = 1.0 -Damping;
StkFloat Damping_A1 = -Damping;


unsigned int SampleRate;



int DRev( void *outputBuffer, void *inputBuffer, unsigned int nFrames,
         double streamTime, RtAudioStreamStatus status, void *dataPointer )
{
    // Cast the buffers to the correct data type.
    
    
    register StkFloat *In = (StkFloat *) inputBuffer;
    register StkFloat *Out = (StkFloat *) outputBuffer;
    
    SineWave *LFO1 = (SineWave *) dataPointer;
    SineWave *LFO2 = (SineWave *) dataPointer;
    

    
    StkFloat Excursion = ExcursionMS * SampleRate/1000;
     
    
    

    // We know we only have 1 sample per frame here.
    for ( int i=0; i<nFrames; i++ ) {
        
        LFO1->tick();
        LFO2->tick();
        
        InTempL = (*In++);
        InTempR = (*In++);
        
        InTemp =  InTempL + InTempR * InputGain ;
        
        DecayTemp_[0] = lengths[4] + (LFO1->lastOut() * Excursion);
        
        Decay_Diffusion1_[0].setDelay( DecayTemp_[0] );
        
        DecayTemp_[1] = lengths[6] + (LFO2->lastOut() * Excursion);
        Decay_Diffusion2_[0].setDelay( DecayTemp_[1] );
        
        PreDelay.tick(InTemp);
        
        Bandwidth.tick(PreDelay.lastOut());
        
        temp = Input_Diffusion1_[0].lastOut();
        temp0 = -(temp * InputDiffusionConstant1);
        temp0 += Bandwidth.lastOut();
        Input_Diffusion1_[0].tick(temp0);
        temp0 = (InputDiffusionConstant1 * temp0) + temp;

        temp = Input_Diffusion1_[1].lastOut();
        temp1 = -(temp * InputDiffusionConstant1);
        temp1 += temp0;
        Input_Diffusion1_[1].tick(temp1);
        temp1 = (InputDiffusionConstant1 * temp1) + temp;
        
        temp = Input_Diffusion2_[0].lastOut();
        temp2 = -(temp * InputDiffusionConstant2);
        temp2 += temp1;
        Input_Diffusion2_[0].tick(temp2);
        temp2 = (InputDiffusionConstant2 * temp2) + temp;
        
        temp = Input_Diffusion2_[1].lastOut();
        temp3 = -(temp * InputDiffusionConstant2);
        temp3 += temp2;
        Input_Diffusion2_[1].tick(temp3);
        temp3 = (InputDiffusionConstant2 * temp3) + temp;
        
        temp = Decay_Diffusion1_[0].lastOut();
        temp4 = (temp * DecayDiffusionConstant1);
        temp4 += temp3 + FBTemp;
        Decay_Diffusion1_[0].tick(temp4);
        temp4 = -(DecayDiffusionConstant1 * temp4) + temp;
        
        TankDelay_[0].tick(temp4);
        
        Damping1.tick(TankDelay_[0].lastOut());
        
        temp = Decay_Diffusion1_[1].lastOut();
        temp5 = -(temp * DecayDiffusionConstant2);
        temp5 += Damping1.lastOut() * Decay;
        Decay_Diffusion1_[1].tick(temp5);
        temp5 = (DecayDiffusionConstant2 * temp5) + temp;
        
        TankDelay_[1].tick(temp5);
        
        temp = Decay_Diffusion2_[0].lastOut();
        temp6 = (temp * DecayDiffusionConstant1);
        temp6 += (TankDelay_[1].lastOut() * Decay) + temp3;
        Decay_Diffusion2_[0].tick(temp4);
        temp6 = -(DecayDiffusionConstant1 * temp6) + temp;
        
        TankDelay_[2].tick(temp6);
        
        Damping1.tick(TankDelay_[2].lastOut());
        
        temp = Decay_Diffusion2_[1].lastOut();
        temp7 = -(temp * DecayDiffusionConstant2);
        temp7 += Damping2.lastOut() * Decay;
        Decay_Diffusion2_[1].tick(temp5);
        temp7 = (DecayDiffusionConstant2 * temp7) + temp;
        
        TankDelay_[3].tick(temp7);
        
        FBTemp = TankDelay_[3].lastOut() * Decay;
        
        cubic.tick(FBTemp);
        
        FBTemp = (FBTemp * (1-DistortionMix)) + (cubic.lastOut() * DistortionMix);
        
        
        OutTempL =  0.6  * (Decay_Diffusion2_[0].tapOut(taps[0]) + Decay_Diffusion2_[0].tapOut(taps[1]) - Decay_Diffusion2_[1].tapOut(taps[2]) + Decay_Diffusion2_[1].tapOut(taps[3]) - Decay_Diffusion1_[0].tapOut(taps[4]) - Decay_Diffusion1_[1].tapOut(taps[5]) - TankDelay_[1].tapOut(taps[6]));
        
        OutTempR =  0.6  * (Decay_Diffusion1_[0].tapOut(taps[7]) + Decay_Diffusion1_[0].tapOut(taps[8]) - Decay_Diffusion1_[1].tapOut(taps[9]) + Decay_Diffusion1_[1].tapOut(taps[10]) - Decay_Diffusion2_[0].tapOut(taps[11]) - Decay_Diffusion2_[1].tapOut(taps[12]) - Decay_Diffusion2_[1].tapOut(taps[13]));
        
        *Out++ = OutTempL;
        *Out++ = OutTempR;
        
        
    }
    
    return 0;
}
  


int main( int argc, char *argv[] )
{
    RtAudio adac;
    SineWave LFO1;
    SineWave LFO2;
  
    unsigned int nFrames = 256;  // 256 sample frames
    unsigned int SampleRate = 48000;
    unsigned int nChannels = 2;
  
    StkFloat scaler = SampleRate/29761;
    
    
    StkFloat Excursion = ExcursionMS * SampleRate/1000;
    
    
     int delay, i;
    if ( scaler != 1.0 ) {
      for ( i=0; i<12; i++ ) {
        delay =  scaler * lengths[i] ;
        if ( (delay & 1) == 0) delay++;
        lengths[i] = delay;
      }}
        
        if ( scaler != 1.0 ) {
          for ( i=0; i<14; i++ ) {
            delay = (int) floor( scaler * taps[i] );
            if ( (delay & 1) == 0) delay++;
            taps[i] = delay;
          }}
        
        Bandwidth.setCoefficients(BandWidth, BandWidth_A1);
        Damping1.setCoefficients(Damping_B0, Damping_A1);
        Damping2.setCoefficients(Damping_B0, Damping_A1);
    
        PreDelay.setMaximumDelay(SampleRate * 2);
        PreDelay.setDelay(1);

        
        for ( i=0; i<2; i++ ) {
        Input_Diffusion1_[i].setMaximumDelay( lengths[i] );
        Input_Diffusion1_[i].setDelay( lengths[i] );
        }
    
        
        for ( i=0; i<2; i++ ) {
        Input_Diffusion2_[i].setMaximumDelay( lengths[i+2] );
        Input_Diffusion2_[i].setDelay( lengths[i+2] );
        }
    
        
        for ( i=0; i<2; i++ ) {
        Decay_Diffusion1_[i].setMaximumDelay( lengths[i+4] + 100);
        Decay_Diffusion1_[i].setDelay( lengths[i+4] );
        }
        for ( i=0; i<2; i++ ) {
        Decay_Diffusion2_[i].setMaximumDelay( lengths[i+6] + 100);
        Decay_Diffusion2_[i].setDelay( lengths[i+6] );
        }
    
        for ( i=0; i<4; i++ ) {
        TankDelay_[i].setMaximumDelay( lengths[i+8] );
        TankDelay_[i].setDelay( lengths[i+8] );
        }
    
    RtAudio::StreamParameters oParameters, iParameters;
    oParameters.deviceId = adac.getDefaultOutputDevice();
    iParameters.deviceId = adac.getDefaultInputDevice();
    oParameters.nChannels = nChannels;
    iParameters.nChannels = nChannels;
        
    try {
        adac.openStream( &oParameters, &iParameters, RTAUDIO_FLOAT64, SampleRate, &nFrames, &DRev, (void *)&LFO1 );
    }
    catch ( RtAudioError &error ) {
        error.printMessage();
        exit( EXIT_FAILURE );
    }
    
    LFO1.setFrequency(0.10);
    LFO2.setFrequency(0.070);

    try {
        adac.startStream();
    }
    
    
    catch ( RtAudioError &error ) {
        error.printMessage();
        goto cleanup;
    }
    
    // Block waiting here.
    char keyhit;
    std::cout << "\nPlaying ... press <enter> to quit.\n";
    std::cin.get( keyhit );
    // Shut down the output stream.
    try {
        adac.closeStream();
    }
    catch ( RtAudioError &error ) {
        error.printMessage();
    }
    std::cout << "See you soon!" << std::endl;
cleanup:

    return 0;
    
 
}
    

