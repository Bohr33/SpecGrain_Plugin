//
//  Fsig.h
//  SpecGrain - Shared Code
//
//  Created by Benjamin Ward (Old Computer) on 6/25/25.
//

#ifndef Fsig_h
#define Fsig_h

#include <vector>
#include <algorithm>

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
        std::fill(amplitudes.begin(), amplitudes.end(), 0.0f);
        std::fill(frequencies.begin(), frequencies.end(), 0.0f);
    }
};

#endif /* Fsig_h */
