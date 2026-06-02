/*
 * main.cpp — A5/1 Cryptanalysis Project
 *
 * Runs all three experiments comparing original vs enhanced A5/1:
 *   1. Avalanche Effect
 *   2. Shannon Entropy
 *   3. Frequency / Bias
 *
 * Results are printed to console and saved as CSV in data/results/.
 */

#include "a51_original.h"
#include "a51_enhanced.h"
#include "analysis.h"

#include <iostream>

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "  A5/1 Cryptanalysis and Enhancement" << std::endl;
    std::cout << "  Comparing Original vs Enhanced Cipher" << std::endl;
    std::cout << "============================================" << std::endl;

    // Test 1: Avalanche Effect
    // 100 trials, 10000 bits each (fast enough for testing)
    runAvalancheTest(100, 10000);

    // Test 2: Shannon Entropy
    // 1,000,000 bits — tests block sizes 4, 8, 16
    runEntropyTest(1000000);

    // Test 3: Frequency / Bias
    // 1,000,000 bits, 1000-bit windows
    runFrequencyTest(1000000, 1000);

    std::cout << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "  All experiments complete." << std::endl;
    std::cout << "  CSV results saved to data/results/" << std::endl;
    std::cout << "============================================" << std::endl;

    return 0;
}
