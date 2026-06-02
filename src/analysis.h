#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <string>

/*
 * Analysis Module — 3 Metrics
 *
 * 1. Avalanche Effect   — how much output changes when 1 key bit flips
 * 2. Shannon Entropy    — measures randomness quality of the keystream
 * 3. Frequency Analysis — checks if 0s and 1s are evenly distributed
 *
 * Each function runs the test on both original and enhanced ciphers
 * and prints results to console + saves CSV to data/results/.
 */

// Avalanche Effect
// Flips one bit in the key, measures % of keystream bits that change.
// Runs over numTrials random keys, each generating streamLength bits.
// Prints mean, std dev, min, max for both versions.
void runAvalancheTest(int numTrials, int streamLength);

// Shannon Entropy
// Divides keystream into blocks, computes per-block entropy.
// Target is 1.0 bits/bit (perfect randomness).
// Tests block sizes: 4, 8, 16.
void runEntropyTest(int streamLength);

// Frequency / Bias Analysis
// Counts 0s and 1s in the keystream, reports deviation from 50/50.
// Also computes per-window frequencies for consistency check.
void runFrequencyTest(int streamLength, int windowSize);

// Helper: save an array of doubles to a CSV file (one value per line)
void saveToCSV(const std::string &filename, double values[], int count,
               const std::string &header);

#endif // ANALYSIS_H
