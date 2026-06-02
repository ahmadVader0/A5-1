#include "a51_enhanced.h"
#include <cstdint>

// Constructor
A51Enhanced::A51Enhanced() {
    reset();
}

// Reset
void A51Enhanced::reset() {
    for (int i = 0; i < 19; i++) R1[i] = 0;
    for (int i = 0; i < 22; i++) R2[i] = 0;
    for (int i = 0; i < 23; i++) R3[i] = 0;
    for (int i = 0; i < 25; i++) R4[i] = 0;
}

// Helper function to clock a single register using tap positions
void A51Enhanced::clockRegister(int reg[], int length, int tap1, int tap2, int tap3, int tap4, int mixBit) {
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

// Determines the clocking decision for the cipher using selected register bits
int A51Enhanced::clockingDecision() {
    int bit0 = R1[8] ^ R2[10] ^ R3[10] ^ R4[0];
    int bit1 = R1[5] ^ R2[7] ^ R3[15] ^ R4[12];
    return (bit1 << 1) | bit0;
}

// Executes clocking for selected registers based on the decision value
void A51Enhanced::applyClocking(int decision) {
    if (decision == 0) {
        clockRegister(R1, 19, 18, 17, 16, 13);
        clockRegister(R2, 22, 21, 20, -1, -1);
        clockRegister(R4, 25, 24, 17, 7, 3);
    } else if (decision == 1) {
        clockRegister(R2, 22, 21, 20, -1, -1);
        clockRegister(R3, 23, 22, 21, 20, 7);
        clockRegister(R4, 25, 24, 17, 7, 3);
    } else if (decision == 2) {
        clockRegister(R1, 19, 18, 17, 16, 13);
        clockRegister(R3, 23, 22, 21, 20, 7);
        clockRegister(R4, 25, 24, 17, 7, 3);
    } else if (decision == 3) {
        clockRegister(R1, 19, 18, 17, 16, 13);
        clockRegister(R2, 22, 21, 20, -1, -1);
        clockRegister(R3, 23, 22, 21, 20, 7);
    }
}

// Computes the combined output bit using MSBs and a nonlinear term
int A51Enhanced::getOutput() {
    int a = R1[18];
    int b = R2[21];
    int c = R3[22];
    int d = R4[24];
    return a ^ b ^ (c & d);
}

// Get individual register MSB outputs (for analysis)
void A51Enhanced::getRegisterOutputs(int &r1Out, int &r2Out, int &r3Out, int &r4Out) {
    r1Out = R1[18];
    r2Out = R2[21];
    r3Out = R3[22];
    r4Out = R4[24];
}

// Initializes the four registers using the 64-bit key and 22-bit frame number
void A51Enhanced::initialize(uint64_t key, uint32_t frameNumber) {
    reset();

    // Load the 64-bit key bit by bit
    for (int i = 0; i < 64; i++) {
        int keyBit = (key >> i) & 1;
        clockRegister(R1, 19, 18, 17, 16, 13, keyBit);
        clockRegister(R2, 22, 21, 20, -1, -1, keyBit);
        clockRegister(R3, 23, 22, 21, 20, 7, keyBit);
        clockRegister(R4, 25, 24, 17, 7, 3, keyBit);
    }

    // Load the 22-bit frame number bit by bit
    for (int i = 0; i < 22; i++) {
        int frameBit = (frameNumber >> i) & 1;
        clockRegister(R1, 19, 18, 17, 16, 13, frameBit);
        clockRegister(R2, 22, 21, 20, -1, -1, frameBit);
        clockRegister(R3, 23, 22, 21, 20, 7, frameBit);
        clockRegister(R4, 25, 24, 17, 7, 3, frameBit);
    }

    // Run 100 warm-up cycles (discarding output) using the decision-based clocking
    for (int i = 0; i < 100; i++) {
        int decision = clockingDecision();
        applyClocking(decision);
    }
}

// Generates a single keystream bit
int A51Enhanced::generateBit() {
    int decision = clockingDecision();
    applyClocking(decision);
    return getOutput();
}

// Generates multiple keystream bits
void A51Enhanced::generateKeystream(int output[], int numBits) {
    for (int i = 0; i < numBits; i++) {
        output[i] = generateBit();
    }
}
