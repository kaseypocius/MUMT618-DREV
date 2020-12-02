<!---layout: page
title: "Demos"
permalink: /demos/--->

<h2> Filter Pings </h2>

Audio has been normalized to enhance detail, and expose self resonances. I've kept the same settings across the different algorithms even when tweaking would be beneficial, to provide the fairest comparisons.

<a href="https://github.com/kaseypocius/MUMT618-DREV/blob/master/docs/audio/m32-dry_1.wav?raw=true">Dry </a> <br>
<img src="spectrograms/dry-1.png" alt="Spectrogram"> <br>

<a href="https://github.com/kaseypocius/MUMT618-DREV/blob/master/docs/audio/m32-dry_2.wav?raw=true">Dry </a> <br>
<img src="spectrograms/dry-2.png" alt="Spectrogram"> <br>

<a href="https://github.com/kaseypocius/MUMT618-DREV/blob/master/docs/audio/m32-dry_3.wav?raw=true">Dry </a> <br>
<img src="spectrograms/dry-3.png" alt="Spectrogram"> <br>


Passed through the reverbs with the following Settings: <br>
```
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
```
<br>

<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo1-Mother32Pings_JCREV.wav?raw=true">JCRev </a> <br>
<img src="spectrograms/Demo1-Mother32Pings_JCREV.png" alt="Spectrogram"> <br>

<h2> Liam Driscoll On The Guitar </h2>

<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo2-GuitDI_Dry.wav?raw=true"> Dry signal from DI</a><br>
<img src="spectrograms/Demo2-GuitDI_Dry.png" alt="Spectrogram"><br>

Passed through the reverbs with the following Settings:<br>
```
StkFloat InputGain = 0.5;

StkFloat DistortionMix = 0.2;
StkFloat VerbMix = 0.1;

StkFloat InputDiffusionConstant1 = 0.75;
StkFloat InputDiffusionConstant2 = 0.625;
StkFloat DecayDiffusionConstant1 = 0.7;
StkFloat DecayDiffusionConstant2 = 0.5;
StkFloat Decay = 0.9999;
StkFloat BandWidth = 1;
StkFloat Damping = 0.005 ;
StkFloat ExcursionMS = 0.5376163435;
```

<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo2-GuitDI_JCREV.wav?raw=true">JCRev </a> <br>
<img src="spectrograms/Demo2-GuitDI_JCREV.png" alt="Spectrogram"> <br>


Guit passed through the quad reverbs with the following Settings: <br>
```
StkFloat InputGain = 0.25;

StkFloat DistortionMix = 0.;
StkFloat VerbMix = 0.1;

StkFloat InputDiffusionConstant1 = 0.75;
StkFloat InputDiffusionConstant2 = 0.625;
StkFloat DecayDiffusionConstant1 = 0.7;
StkFloat DecayDiffusionConstant2 = 0.5;
StkFloat Decay = 0.49999;
StkFloat BandWidth = 0.5;
StkFloat Damping = 0.005 ;
StkFloat ExcursionMS = 0.5376163435;
```
<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo2-GuitDI_JCREV-QUAD.wav?raw=true">JCRev-Quad </a> <br>
<img src="spectrograms/Demo2-GuitDI_JCREV.png" alt="Spectrogram"> <br>

<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo2-GuitDI_NREV-QUAD.wav?raw=true">NRev-Quad </a> <br>
<img src="spectrograms/Demo2-GuitDI_NREV.png" alt="Spectrogram"> <br>

<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo2-GuitDI_PRCREV-QUAD.wav?raw=true">PRCRev-Quad </a> <br>
<img src="spectrograms/Demo2-GuitDI_PRCREV.png" alt="Spectrogram"> <br>

<h2> Mother 32 Short Sequence </h2>

<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo3-Mother32Seq_Dry.wav?raw=true"> Dry Mother32 Sequence</a><br>
<img src="spectrograms/Demo3-Mother32Seq_Dry.png" alt="Spectrogram"><br>

Mother32 sequenced passed through the quad reverbs with the following Settings: <br>
```
StkFloat InputGain = 0.25;

StkFloat DistortionMix = 0.;
StkFloat VerbMix = 0.1;

StkFloat InputDiffusionConstant1 = 0.75;
StkFloat InputDiffusionConstant2 = 0.625;
StkFloat DecayDiffusionConstant1 = 0.7;
StkFloat DecayDiffusionConstant2 = 0.5;
StkFloat Decay = 0.99999;
StkFloat BandWidth = 0.95;
StkFloat Damping = 0.9005 ;
StkFloat ExcursionMS = 0.5376163435;

```

<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo3-Mother32Seq_JCREV-QUAD.wav?raw=true">JCRev-Quad </a> <br>
<img src="spectrograms/Demo3-Mother32Seq_JCREV-QUAD.png" alt="Spectrogram"> <br>

<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo3-Mother32Seq_NREV-QUAD.wav?raw=true">NRev-Quad </a> <br>
<img src="spectrograms/Demo3-Mother32Seq_NREV-QUAD.png" alt="Spectrogram"> <br>

<a href="https://github.com/kaseypocius/MUMT-307-ShimmeringPeaks/blob/master/docs/audio/Demo3-Mother32Seq_PRCREVQUAD.wav?raw=true">PRCRev-Quad </a> <br>
<img src="spectrograms/Demo3-Mother32Seq_PRCREVQUAD.png" alt="Spectrogram"> <br>

```
StkFloat InputGain = 0.25;

StkFloat DistortionMix = 0.9;
StkFloat VerbMix = 0.1;

StkFloat InputDiffusionConstant1 = 0.75;
StkFloat InputDiffusionConstant2 = 0.625;
StkFloat DecayDiffusionConstant1 = 0.7;
StkFloat DecayDiffusionConstant2 = 0.5;
StkFloat Decay = 1;
StkFloat BandWidth = 0.15;
StkFloat Damping = 0.9005 ;
StkFloat ExcursionMS = 0.5376163435;
```

<a href="https://kaseypocius.github.io/MUMT-307-ShimmeringPeaks/about"> Back to the About</a>
