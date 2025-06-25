//
//  Fsig.h
//  SpecGrain - Shared Code
//
//  Created by Benjamin Ward (Old Computer) on 6/25/25.
//

#ifndef Fsig_h
#define Fsig_h

#include <vector>
#include <JuceHeader.h>

struct fsig{
    std::vector<float> amplitudes;
    std::vector<float> frequencies;
    
    void resize(size_t size)
    {
        amplitudes.resize(size);
        frequencies.resize(size);
    }
    
    void clear()
    {
        juce::FloatVectorOperations::clear(amplitudes.data(), amplitudes.size());
        juce::FloatVectorOperations::clear(frequencies.data(), frequencies.size());
    }
};

#endif /* Fsig_h */
