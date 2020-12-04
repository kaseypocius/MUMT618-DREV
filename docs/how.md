<!---layout: page
title: "How?"
permalink: /how/--->

<h2> How? </h2>

A stereo audio input is summed down to a mono signal, which is then fed into the 4 all pass filters. These act to reproduce early reflections, which are then fed into the reverb tank. Within the tank the audio is fed into 4 more allpass filters, here with added delay lines and low pass filters placed in a feedback loop. The strength of the feedback and cutoff of the low pass filters will determine the length of the resulting tail. 2 of the 4 allpass filters are modulated by LFOs, to add some pitch variation and to avoid the build up of clear tones in the tank. Some distortion may be added to the final feedback path, allowing for the decay to exceed 1 is desired without excessive clipping, and can help reproduce some of the characteristics found in early hardware digital reverbs, where the would be some signal saturation at the amplifier chips.


<h2> Why Use My Patches?</h2>
The DRev algorithm is computationally efficient and easily controlled while still capable of a wide variety of sonic characteristics. I can also be easily expanded and modified into a new reverberator if the user wishes to explore new models of reverberation, by expanding the size of the tank, adding a second tank or further output diffusion etc. More LFOs could be added, with more variation in shape and frequency as desired.


<h2> A closer look at the code </h2>
Lets let a step by step look at how the DSP portion code works.
<br>

```

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

//gets the effects together
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

static StkFloat InputGain;

static StkFloat DistortionMix;

static StkFloat InputDiffusionConstant1 ;
static StkFloat InputDiffusionConstant2 ;
static StkFloat DecayDiffusionConstant1 ;
static StkFloat DecayDiffusionConstant2 ;
static StkFloat Decay;
static StkFloat BandWidth;
static StkFloat Damping;
StkFloat ExcursionMS;
StkFloat Excursion;

static StkFloat BandWidth_A1;
static StkFloat Damping_B0;
static StkFloat Damping_A1;

```

<br>

Here the proper header files are loaded, and the STK class instances not using a data pointer are created here for use later on. We then create all the temp variables that are used in the processing loop, as well as all the gain & filter variables. The initial variables for the delay lines are also set up here, but will be scaled for the sample rate later.

<br>

```
// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int DRev( void *outputBuffer, void *inputBuffer, unsigned int nFrames,
  double streamTime, RtAudioStreamStatus status, void *dataPointer)
```
  <br>

  Here we build our tick statement. This patch uses "callback" audio, which will call the FX script every time a new block of audio is needed. This also builds the data pointer we'll use for the modulator oscillators, to avoid a segment fault on start up.

  <br>

  ```
  // Cast the buffers to the correct data type.


  register StkFloat *In = (StkFloat *) inputBuffer;
  register StkFloat *Out = (StkFloat *) outputBuffer;

  SineWave *LFO1 = (SineWave *) dataPointer;
  SineWave *LFO2 = (SineWave *) dataPointer;

  ```

  <br>

  Here the I/O is cast to the correct data type, as well as the LFOs needed to avoid the build up of audible tones. The ammount of variation for the delay loop here is also adapted from ms to samples.

  <br>

  ```
  // We know we only have 1 sample per frame here.
  for ( int i=0; i<nFrames; i++ ) {

    LFO1->tick();
    LFO2->tick();

```
<br>

Now we start the processing loop. We start by generating the samples for the LFOs.

<br>

```
InTempL = (*In++);
InTempR = (*In++);

InTemp =  InTempL + InTempR * InputGain ;

```
<br>

Here the basic input values are created and mixed.

<br>

```
DecayTemp_[0] = abs(lengths[4] + (LFO1->lastOut() * Excursion));

if (DecayTemp_[0] < 0.5) {
    DecayTemp_[0] = 0.5;
}

Decay_Diffusion1_[0].setDelay( DecayTemp_[0] );

DecayTemp_[1] = abs(lengths[6] + (LFO2->lastOut() * Excursion));
if (DecayTemp_[1] < 0.5) {
    DecayTemp_[1] = 0.5;
}
```
<br>

The new delay line lengths are then calculated, using the new LFO values. Some error checking is included, for proper interpolation and to avoid segment faults.

<br>

```
PreDelay.tick(InTemp);

Bandwidth.tick(PreDelay.lastOut());

```
<br>

An initial predelay is calculated here and the initial "BandWidth" filter, which can help emulate lower sample rates and vintage hardware while still calculating the rest of the reverb chain in a more modern sample rate to avoid artifacting

<br>

```
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

```
<br>

Here we calculate the cascade of input all pass filters, resulting in the initial input diffusion.

<br>

```

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



```
<br>

Here the first pass through in the tank is calculated

<br>

```
FBTemp = TankDelay_[3].lastOut() * Decay;

cubic.tick(FBTemp);

FBTemp = (FBTemp * (1-DistortionMix)) + (cubic.lastOut() * DistortionMix);

```
<br>
Here the feedback value is calculated, and then fed into the distortion. The mix is then caclulated based on the user's specifications.

<br>

```
OutTempL =  0.6  * (Decay_Diffusion2_[0].tapOut(taps[0]) + Decay_Diffusion2_[0].tapOut(taps[1]) - Decay_Diffusion2_[1].tapOut(taps[2]) + Decay_Diffusion2_[1].tapOut(taps[3]) - Decay_Diffusion1_[0].tapOut(taps[4]) - Decay_Diffusion1_[1].tapOut(taps[5]) - TankDelay_[1].tapOut(taps[6]));

OutTempR =  0.6  * (Decay_Diffusion1_[0].tapOut(taps[7]) + Decay_Diffusion1_[0].tapOut(taps[8]) - Decay_Diffusion1_[1].tapOut(taps[9]) + Decay_Diffusion1_[1].tapOut(taps[10]) - Decay_Diffusion2_[0].tapOut(taps[11]) - Decay_Diffusion2_[1].tapOut(taps[12]) - Decay_Diffusion2_[1].tapOut(taps[13]));

*Out++ = OutTempL;
*Out++ = OutTempR;

```
<br>
The outputs are then calculated, using the various points from within the loop as specified in the paper to increase complexity in the reverb tail

<br>

```
int main( int argc, char *argv[] )
{
  RtAudio adac;
  SineWave LFO1;
  SineWave LFO2;

  unsigned int nFrames = 256;  // 256 sample frames
  unsigned int SampleRate = 48000;
  unsigned int nChannels = 2;
  
  ```
  <br> 
  Sets the sample rate
  <br>
  
  ```
  //user input vars, followed by error checking
  if ( argc != 11 ) {
      std::cout << "usage: " << argv[0] << " filepath" << std::endl;
      std::cout << "    dump var for filepath" << std::endl;
      std::cout << "usage: " << argv[1] << ": Input Gain" << std::endl;
      std::cout << "   A float point scaler to control input gain" << std::endl;
      std::cout << "usage: " << argv[2] << ": Distortion Mix" << std::endl;
      std::cout << "   A float point scaler to control the mix of distortion on the tank feedback path" << std::endl;
      std::cout << "usage: " << argv[3] << ": InputDiffusionConstant1" << std::endl;
      std::cout << "   A float point between 0 & 1 to control the first set of allpass filters" << std::endl;
      std::cout << "usage: " << argv[4] << ": InputDiffusionConstant2" << std::endl;
      std::cout << "   A float point between 0 & 1 to control the second set of allpass filters" << std::endl;
      std::cout << "usage: " << argv[5] << ": DecayDiffusionConstant1" << std::endl;
      std::cout << "   The first of two float points between 0 & 1 used to control the tank filters " << std::endl;
      std::cout << "usage: " << argv[6] << ": DecayDiffusionConstant2" << std::endl;
      std::cout << "   The second of two float points between 0 & 1 used to control the tank filters " << std::endl;
      std::cout << "usage: " << argv[7] << ": Decay" << std::endl;
      std::cout << "   A float point used to control the tail length of the sound " << std::endl;
      std::cout << "usage: " << argv[8] << ": BandWidth" << std::endl;
      std::cout << "   A float point between 0 & 1 used to control the lowpass filtering at the input" << std::endl;
      std::cout << "usage: " << argv[9] << ": Damping" << std::endl;
      std::cout << "   A float point between 0 & 1 used to control the lowpass filtering within the tank" << std::endl;
      std::cout << "usage: " << argv[10] << "ms : ExcursionMS" << std::endl;
      std::cout << "  The modulation depth of the varrying allpass filters in the tank, measured in milliseconds" << std::endl;
      return(0);
  }
  
  std::cout << std::endl;
  std::cout << "Settings Dump" << std::endl;
  std::cout << std::endl;
  std::cout << "I Live here:" << argv[0] << std::endl;
  std::cout << argv[1] << ": Input Gain" << std::endl;
  std::cout << argv[2] << ": Distortion Mix" << std::endl;
  std::cout << argv[3] << ": InputDiffusionConstant1" << std::endl;
  std::cout << argv[4] << ": InputDiffusionConstant2" << std::endl;
  std::cout << argv[5] << ": DecayDiffusionConstant1" << std::endl;
  std::cout << argv[6] << ": DecayDiffusionConstant2" << std::endl;
  std::cout << argv[7] << ": Decay" << std::endl;
  std::cout << argv[8] << ": BandWidth" << std::endl;
  std::cout << argv[9] << ": Damping" << std::endl;
  std::cout << argv[10] << "ms : ExcursionMS" << std::endl;
  
 InputGain= atof(argv[1]);
  
  if (InputGain <= 0){
    std::cout << " Input Gain should be more than zero.... setting to 0.5" << std::endl;
    InputGain = 0.5;
  }

 DistortionMix= atof(argv[2]);
  
  if (DistortionMix < 0){
    std::cout << " Distoriotn Mix should be at least zero.... setting to 0" << std::endl;
    DistortionMix = 0;
  }
  
  if (DistortionMix > 1){
    std::cout << " Distoriotn Mix should be less than one.... setting to 1" << std::endl;
    DistortionMix = 0;
  }

  InputDiffusionConstant1 = atof(argv[3]);
  
  if (InputDiffusionConstant1 <= 0){
    std::cout << " InputDiffusionContants should be more than zero.... setting to 0.75" << std::endl;
    InputDiffusionConstant1 = 0.75;
  }
  
  if (InputDiffusionConstant1 > 1){
    std::cout << " InputDiffusionContants should be less than one.... setting to 0.75" << std::endl;
    InputDiffusionConstant1 = 0.75;
  }
  
  InputDiffusionConstant2 = atof(argv[4]);
  
  if (InputDiffusionConstant2 <= 0){
    std::cout << " InputDiffusionContants should be more than zero.... setting to 0.65" << std::endl;
    InputDiffusionConstant2 = 0.65;
  }
  
  if (InputDiffusionConstant2 > 1){
    std::cout << " InputDiffusionContants should be less than one.... setting to 0.65" << std::endl;
    InputDiffusionConstant2 = 0.65;
  }
  
   DecayDiffusionConstant1 = atof(argv[5]);
  
  if (DecayDiffusionConstant1 <= 0){
    std::cout << " DecayDiffusionContants should be more than zero.... setting to 0.7" << std::endl;
    DecayDiffusionConstant1 = 0.7;
  }
  
  if (DecayDiffusionConstant1 > 1){
    std::cout << " DecayDiffusionContants should be less than one.... setting to 0.7" << std::endl;
    DecayDiffusionConstant1 = 0.7;
  }
  
  DecayDiffusionConstant2 = atof(argv[6]);
  
  if (DecayDiffusionConstant2 <= 0){
    std::cout << " DecayDiffusionContants should be more than zero.... setting to 0.5" << std::endl;
    DecayDiffusionConstant2 = 0.5;
  }
  
  if (DecayDiffusionConstant2 > 1){
    std::cout << " DecayDiffusionContants should be less than one.... setting to 0.5" << std::endl;
    DecayDiffusionConstant2 = 0.5;
  }
  
  Decay = atof(argv[7]);
  
  if (Decay < 0){
    std::cout << " Decay should be not be negative.... setting to 0" << std::endl;
    Decay = 0;
  }
  
  BandWidth = atof(argv[8]);
  
  if (BandWidth <= 0){
    std::cout << " BandWidth should be more than zero.... setting to 0.001" << std::endl;
    BandWidth = 0.001;
  }
  
  if (BandWidth > 1){
    std::cout << " BandWidth should be less than one.... setting to 0.9995" << std::endl;
    BandWidth = 0.9995;
  }
  
  Damping  = atof(argv[9]);
  
  if (Damping < 0){
    std::cout << " Damping shouldn't be negative.... setting to 0.005" << std::endl;
    Damping = 0.005;
  }
  
  if (Damping > 1){
    std::cout << " Damping should be less than one.... setting to 0.9995" << std::endl;
    Damping = 0.9995;
  }
  
   ExcursionMS = atof(argv[10]);
  
  if (ExcursionMS > 1000){
    std::cout << " Excusion can't be more than a second.... setting to 1 second" << std::endl;
    ExcursionMS = 1000;
  }
  
  static StkFloat BandWidth_A1 = -(1.0 - BandWidth);
  static StkFloat Damping_B0 = 1.0 -Damping;
  static StkFloat Damping_A1 = -Damping

```
user vars are set up

```

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
            Decay_Diffusion1_[i].setMaximumDelay( lengths[i+4] + SampleRate);
            Decay_Diffusion1_[i].setDelay( lengths[i+4] );
          }
          for ( i=0; i<2; i++ ) {
            Decay_Diffusion2_[i].setMaximumDelay( lengths[i+6] + SampleRate);
            Decay_Diffusion2_[i].setDelay( lengths[i+6] );
          }

          for ( i=0; i<4; i++ ) {
            TankDelay_[i].setMaximumDelay( lengths[i+8] );
            TankDelay_[i].setDelay( lengths[i+8] );
          }

```
<br>

Once the sample rate is specified by the user, we use  it to create a scalar value used to rescale the original values from the paper to those appropriate for the new sampling rate.

  <h2> Challenges </h2>
The initial challenge for me was structuring the allpass filters, which I did not initially do correctly. From there, this algorithm should self sustain almost infinetly at a Decay setting of 1, but this still does not seem to be the case. This model is still quite susceptible to internal clipping at any of the filters within the tank, so care must be taken to avoid this, unless this effect is deemed desirable by the user.  

  <h2> For the Future</h2>
  As mentioned above, additional LFOs could be added with a wider variety of modulation shapes. Further experimentation with other effects to add to the feedback loop could also be useful, such as chorus or flangeing for further tail complexity. Implementations such as Valley Audio's Plateau add a size variable, so the user can tune the filters as they see fit. I did not pursue this as I rarely used size settings that aren't the ones found in original paper, but this method would allow for some more novel use cases, allowing for this reverb to double as a pitched percussion instrument.


  <a href="https://kaseypocius.github.io/MUMT618-DREV/about"> Back to the About</a>
