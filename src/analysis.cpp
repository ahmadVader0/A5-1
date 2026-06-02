/*
 * analysis.cpp — Analysis Module (3 Metrics)
 *
 * Implements:
 *   1. Avalanche Effect   — single-bit key change → measure output change
 *   2. Shannon Entropy    — block-level randomness measurement
 *   3. Frequency Analysis — 0/1 balance check
 *
 * Each test runs on both original and enhanced ciphers,
 * prints comparison results, and saves CSV data.
 */

#include "analysis.h"
#include "a51_original.h"
#include "a51_enhanced.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;


// ================================================================
// Helper: save doubles to CSV
// ================================================================
void saveToCSV(const string &filename, double values[], int count,
               const string &header) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: could not open " << filename << endl;
        return;
    }
    file << header << endl;
    for (int i = 0; i < count; i++) {
        file << values[i] << endl;
    }
    file.close();
    cout << "  Saved to " << filename << endl;
}


// ================================================================
// METRIC 1: Avalanche Effect
// ================================================================
//
// For each trial:
//   1. Pick a random key and IV
//   2. Generate N keystream bits with that key
//   3. Flip one bit in the key
//   4. Generate N bits again
//   5. Count how many bits differ → avalanche %
//
// Reports mean, std dev, min, max over all trials.

void runAvalancheTest(int numTrials, int streamLength) {
    cout << endl;
    cout << "========================================" << endl;
    cout << "  AVALANCHE EFFECT TEST" << endl;
    cout << "  Trials: " << numTrials << endl;
    cout << "  Stream length: " << streamLength << " bits" << endl;
    cout << "========================================" << endl;

    // Allocate arrays for keystream bits
    int *stream1 = new int[streamLength];
    int *stream2 = new int[streamLength];

    // Store avalanche ratios for each trial
    double *origResults = new double[numTrials];
    double *enhResults  = new double[numTrials];

    srand(42); // fixed seed for reproducibility

    for (int trial = 0; trial < numTrials; trial++) {
        // Generate random key and IV
        uint64_t key = ((uint64_t)rand() << 32) | rand();
        uint32_t iv  = rand() & 0x3FFFFF; // 22 bits

        // Pick a random bit position to flip (0-63)
        int flipBit = rand() % 64;
        uint64_t flippedKey = key ^ ((uint64_t)1 << flipBit);

        // --- Original A5/1 ---
        A51Original origCipher;

        origCipher.initialize(key, iv);
        origCipher.generateKeystream(stream1, streamLength);

        origCipher.initialize(flippedKey, iv);
        origCipher.generateKeystream(stream2, streamLength);

        int diffCount = 0;
        for (int i = 0; i < streamLength; i++) {
            if (stream1[i] != stream2[i]) diffCount++;
        }
        origResults[trial] = (double)diffCount / streamLength * 100.0;

        // --- Enhanced A5/1 ---
        A51Enhanced enhCipher;

        enhCipher.initialize(key, iv);
        enhCipher.generateKeystream(stream1, streamLength);

        enhCipher.initialize(flippedKey, iv);
        enhCipher.generateKeystream(stream2, streamLength);

        diffCount = 0;
        for (int i = 0; i < streamLength; i++) {
            if (stream1[i] != stream2[i]) diffCount++;
        }
        enhResults[trial] = (double)diffCount / streamLength * 100.0;
    }

    // Compute statistics for original
    double origMean = 0, origMin = origResults[0], origMax = origResults[0];
    for (int i = 0; i < numTrials; i++) {
        origMean += origResults[i];
        if (origResults[i] < origMin) origMin = origResults[i];
        if (origResults[i] > origMax) origMax = origResults[i];
    }
    origMean /= numTrials;

    double origStddev = 0;
    for (int i = 0; i < numTrials; i++) {
        origStddev += (origResults[i] - origMean) * (origResults[i] - origMean);
    }
    origStddev = sqrt(origStddev / numTrials);

    // Compute statistics for enhanced
    double enhMean = 0, enhMin = enhResults[0], enhMax = enhResults[0];
    for (int i = 0; i < numTrials; i++) {
        enhMean += enhResults[i];
        if (enhResults[i] < enhMin) enhMin = enhResults[i];
        if (enhResults[i] > enhMax) enhMax = enhResults[i];
    }
    enhMean /= numTrials;

    double enhStddev = 0;
    for (int i = 0; i < numTrials; i++) {
        enhStddev += (enhResults[i] - enhMean) * (enhResults[i] - enhMean);
    }
    enhStddev = sqrt(enhStddev / numTrials);

    // Print results
    cout << endl;
    cout << "Results (ideal = 50.0%):" << endl;
    cout << "  Original A5/1:" << endl;
    cout << "    Mean:    " << origMean << "%" << endl;
    cout << "    Std Dev: " << origStddev << "%" << endl;
    cout << "    Min:     " << origMin << "%" << endl;
    cout << "    Max:     " << origMax << "%" << endl;
    cout << endl;
    cout << "  Enhanced A5/1:" << endl;
    cout << "    Mean:    " << enhMean << "%" << endl;
    cout << "    Std Dev: " << enhStddev << "%" << endl;
    cout << "    Min:     " << enhMin << "%" << endl;
    cout << "    Max:     " << enhMax << "%" << endl;

    // Save to CSV
    saveToCSV("data/results/avalanche_original.csv", origResults, numTrials,
              "avalanche_percent");
    saveToCSV("data/results/avalanche_enhanced.csv", enhResults, numTrials,
              "avalanche_percent");

    delete[] stream1;
    delete[] stream2;
    delete[] origResults;
    delete[] enhResults;
}


// ================================================================
// METRIC 2: Shannon Entropy
// ================================================================
//
// Divides keystream into blocks of size B.
// Counts how often each block pattern appears.
// Computes: H = -Σ p(x) * log2(p(x))
// Normalizes: H_norm = H / B  (should be close to 1.0)

void runEntropyTest(int streamLength) {
    cout << endl;
    cout << "========================================" << endl;
    cout << "  SHANNON ENTROPY TEST" << endl;
    cout << "  Stream length: " << streamLength << " bits" << endl;
    cout << "========================================" << endl;

    int blockSizes[] = {4, 8, 16};
    int numBlockSizes = 3;

    // Use a fixed key and IV
    uint64_t key = 0xABCDEF0123456789ULL;
    uint32_t iv  = 0x1234;

    int *stream = new int[streamLength];

    // Store results: [blockSize, origEntropy, enhEntropy]
    double origEntropies[3];
    double enhEntropies[3];

    for (int b = 0; b < numBlockSizes; b++) {
        int blockSize = blockSizes[b];
        int numBlocks = streamLength / blockSize;
        int numPatterns = 1 << blockSize; // 2^blockSize possible patterns

        // --- Original ---
        A51Original origCipher;
        origCipher.initialize(key, iv);
        origCipher.generateKeystream(stream, streamLength);

        // Count occurrences of each pattern
        int *origCounts = new int[numPatterns];
        memset(origCounts, 0, numPatterns * sizeof(int));

        for (int i = 0; i < numBlocks; i++) {
            int pattern = 0;
            for (int j = 0; j < blockSize; j++) {
                pattern = (pattern << 1) | stream[i * blockSize + j];
            }
            origCounts[pattern]++;
        }

        // Compute Shannon entropy
        double origH = 0.0;
        for (int i = 0; i < numPatterns; i++) {
            if (origCounts[i] > 0) {
                double p = (double)origCounts[i] / numBlocks;
                origH -= p * log2(p);
            }
        }
        origEntropies[b] = origH / blockSize; // normalized

        delete[] origCounts;

        // --- Enhanced ---
        A51Enhanced enhCipher;
        enhCipher.initialize(key, iv);
        enhCipher.generateKeystream(stream, streamLength);

        int *enhCounts = new int[numPatterns];
        memset(enhCounts, 0, numPatterns * sizeof(int));

        for (int i = 0; i < numBlocks; i++) {
            int pattern = 0;
            for (int j = 0; j < blockSize; j++) {
                pattern = (pattern << 1) | stream[i * blockSize + j];
            }
            enhCounts[pattern]++;
        }

        double enhH = 0.0;
        for (int i = 0; i < numPatterns; i++) {
            if (enhCounts[i] > 0) {
                double p = (double)enhCounts[i] / numBlocks;
                enhH -= p * log2(p);
            }
        }
        enhEntropies[b] = enhH / blockSize;

        delete[] enhCounts;
    }

    // Print results
    cout << endl;
    cout << "Results (ideal = 1.000000):" << endl;
    cout << "  Block Size | Original | Enhanced" << endl;
    cout << "  -----------|----------|--------" << endl;
    for (int b = 0; b < numBlockSizes; b++) {
        cout << "  " << blockSizes[b]
             << "          | " << origEntropies[b]
             << " | " << enhEntropies[b] << endl;
    }

    // Save to CSV
    ofstream origFile("data/results/entropy_original.csv");
    ofstream enhFile("data/results/entropy_enhanced.csv");
    if (origFile.is_open() && enhFile.is_open()) {
        origFile << "block_size,normalized_entropy" << endl;
        enhFile  << "block_size,normalized_entropy" << endl;
        for (int b = 0; b < numBlockSizes; b++) {
            origFile << blockSizes[b] << "," << origEntropies[b] << endl;
            enhFile  << blockSizes[b] << "," << enhEntropies[b] << endl;
        }
        origFile.close();
        enhFile.close();
        cout << "  Saved to data/results/entropy_original.csv" << endl;
        cout << "  Saved to data/results/entropy_enhanced.csv" << endl;
    }

    delete[] stream;
}


// ================================================================
// METRIC 3: Frequency / Bias Analysis
// ================================================================
//
// Counts 0s and 1s in the keystream.
// Reports overall bias = |count1 - count0| / total
// Also computes per-window frequencies for consistency.

void runFrequencyTest(int streamLength, int windowSize) {
    cout << endl;
    cout << "========================================" << endl;
    cout << "  FREQUENCY / BIAS TEST" << endl;
    cout << "  Stream length: " << streamLength << " bits" << endl;
    cout << "  Window size: " << windowSize << " bits" << endl;
    cout << "========================================" << endl;

    uint64_t key = 0xABCDEF0123456789ULL;
    uint32_t iv  = 0x1234;

    int *stream = new int[streamLength];
    int numWindows = streamLength / windowSize;

    double *origWindowFreqs = new double[numWindows];
    double *enhWindowFreqs  = new double[numWindows];

    // --- Original ---
    A51Original origCipher;
    origCipher.initialize(key, iv);
    origCipher.generateKeystream(stream, streamLength);

    int origOnes = 0;
    for (int i = 0; i < streamLength; i++) {
        origOnes += stream[i];
    }
    int origZeros = streamLength - origOnes;
    double origBias = fabs((double)origOnes - origZeros) / streamLength;

    // Per-window frequencies
    for (int w = 0; w < numWindows; w++) {
        int ones = 0;
        for (int i = 0; i < windowSize; i++) {
            ones += stream[w * windowSize + i];
        }
        origWindowFreqs[w] = (double)ones / windowSize;
    }

    // --- Enhanced ---
    A51Enhanced enhCipher;
    enhCipher.initialize(key, iv);
    enhCipher.generateKeystream(stream, streamLength);

    int enhOnes = 0;
    for (int i = 0; i < streamLength; i++) {
        enhOnes += stream[i];
    }
    int enhZeros = streamLength - enhOnes;
    double enhBias = fabs((double)enhOnes - enhZeros) / streamLength;

    for (int w = 0; w < numWindows; w++) {
        int ones = 0;
        for (int i = 0; i < windowSize; i++) {
            ones += stream[w * windowSize + i];
        }
        enhWindowFreqs[w] = (double)ones / windowSize;
    }

    // Print results
    cout << endl;
    cout << "Results (ideal bias = 0.000000):" << endl;
    cout << endl;
    cout << "  Original A5/1:" << endl;
    cout << "    Total 1s: " << origOnes << endl;
    cout << "    Total 0s: " << origZeros << endl;
    cout << "    Bias:     " << origBias << endl;
    cout << endl;
    cout << "  Enhanced A5/1:" << endl;
    cout << "    Total 1s: " << enhOnes << endl;
    cout << "    Total 0s: " << enhZeros << endl;
    cout << "    Bias:     " << enhBias << endl;

    // Save window frequencies to CSV
    saveToCSV("data/results/frequency_original.csv", origWindowFreqs, numWindows,
              "window_frequency");
    saveToCSV("data/results/frequency_enhanced.csv", enhWindowFreqs, numWindows,
              "window_frequency");

    delete[] stream;
    delete[] origWindowFreqs;
    delete[] enhWindowFreqs;
}
