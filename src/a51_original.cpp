#include "a51_original.h"
#include <cstdint>

// Constructor
A51Original::A51Original() {
    reset();
}

// Reset
void A51Original::reset() {
    for (int i = 0; i < 19; i++) R1[i] = 0;
    for (int i = 0; i < 22; i++) R2[i] = 0;
    for (int i = 0; i < 23; i++) R3[i] = 0;
}

// Helper function to compute majority of three bits
int A51Original::majority(int b1, int b2, int b3) {
    return (b1 + b2 + b3) >= 2 ? 1 : 0;
}

// Clocks a register: computes feedback from tap positions, shifts, and inserts feedback at index 0
void A51Original::clockRegister(int reg[], int length, int tap1, int tap2, int tap3, int tap4, int mixBit) {
    int feedback = reg[tap1] ^ reg[tap2];
    if (tap3 != -1) feedback ^= reg[tap3];
    if (tap4 != -1) feedback ^= reg[tap4];
    feedback ^= mixBit;

    // Shift all elements right by 1
    for (int i = length - 1; i > 0; --i) {
        reg[i] = reg[i - 1];
    }
    reg[0] = feedback;
}

// Combines the outputs of the three registers using XOR of MSBs
int A51Original::getOutput() {
    return R1[18] ^ R2[21] ^ R3[22];
}

// Get individual register MSB outputs (for analysis)
void A51Original::getRegisterOutputs(int &r1Out, int &r2Out, int &r3Out) {
    r1Out = R1[18];
    r2Out = R2[21];
    r3Out = R3[22];
}

// Initializes the three registers using the 64-bit key and 22-bit frame number
void A51Original::initialize(uint64_t key, uint32_t frameNumber) {
    reset();

    // Load the 64-bit key bit by bit
    for (int i = 0; i < 64; i++) {
        int keyBit = (key >> i) & 1;
        clockRegister(R1, 19, 18, 17, 16, 13, keyBit);
        clockRegister(R2, 22, 21, 20, -1, -1, keyBit);
        clockRegister(R3, 23, 22, 21, 20, 7, keyBit);
    }

    // Load the 22-bit frame number bit by bit
    for (int i = 0; i < 22; i++) {
        int frameBit = (frameNumber >> i) & 1;
        clockRegister(R1, 19, 18, 17, 16, 13, frameBit);
        clockRegister(R2, 22, 21, 20, -1, -1, frameBit);
        clockRegister(R3, 23, 22, 21, 20, 7, frameBit);
    }

    // Run 100 warm-up cycles (discarding output) using majority clocking
    for (int i = 0; i < 100; i++) {
        int maj = majority(R1[8], R2[10], R3[10]);
        if (R1[8] == maj) clockRegister(R1, 19, 18, 17, 16, 13);
        if (R2[10] == maj) clockRegister(R2, 22, 21, 20, -1, -1);
        if (R3[10] == maj) clockRegister(R3, 23, 22, 21, 20, 7);
    }
}

// Generates a single keystream bit by clocking the registers using majority logic
int A51Original::generateBit() {
    int maj = majority(R1[8], R2[10], R3[10]);
    if (R1[8] == maj) clockRegister(R1, 19, 18, 17, 16, 13);
    if (R2[10] == maj) clockRegister(R2, 22, 21, 20, -1, -1);
    if (R3[10] == maj) clockRegister(R3, 23, 22, 21, 20, 7);

    return getOutput();
}

// Generates multiple keystream bits
void A51Original::generateKeystream(int output[], int numBits) {
    for (int i = 0; i < numBits; i++) {
        output[i] = generateBit();
    }
}
