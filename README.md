
 # ADSSS Modem

 C++ modulation/demodulation algorithms to implement a data transmission channel between "smart" devices using an acoustic channel with an arbitrary, user-defined audio fragment acting as carrier sound.

 ![Infographics for Acoustic Modem](https://github.com/dephonica/AcousticModem/blob/master/images/acoumodem.png)

 ## Encoder usage

 ### With carrier PCM data in memory:

```cpp
std::vector<PCMTYPE> referencePcm;
// fill reference vector with PCM carrier data here
ADSSS::PcmHelpers::ReferenceSource referenceSource(referencePcmSamplerate, referencePcm);
ADSSS::Encoder encoder(referenceSource);
auto resultPcm = encoder.Go(payloadData);
```

 ## Decoder usage

 ### With encoded PCM data in memory:

```cpp
std::vector<PCMTYPE> encodedPcm;
// fill encoded vector with PCM data here
ADSSS::PcmHelpers::ReferenceSource encodedSource(encodedPcmSamplerate, encodedPcm);
ADSSS::Decoder decoder(encodedSource);
auto decodedData = decoder.Go();
```
