#ifndef A51_ORIGINAL_H
#define A51_ORIGINAL_H

#include <cstdint>

// Class representing the original standard A5/1 stream cipher
class A51Original {
public:
    A51Original();

    // Initializes the cipher with a 64-bit key and a 22-bit frame number
    void initialize(uint64_t key, uint32_t frameNumber);

    // Generates a single keystream bit
    int generateBit();

    // Generates a sequence of keystream bits of the given length into a caller-provided array
    void generateKeystream(int output[], int numBits);

    // Resets all registers to zero
    void reset();

    // Gets the MSB output of each register (for analysis)
    void getRegisterOutputs(int &r1Out, int &r2Out, int &r3Out);

private:
    // Registers of A5/1 represented as simple arrays of bits (0 or 1)
    int R1[19];
    int R2[22];
    int R3[23];

    // Helper function to calculate the majority of three bits
    int majority(int b1, int b2, int b3);

    // Helper function to clock a single register using tap positions
    void clockRegister(int reg[], int length, int tap1, int tap2, int tap3 = -1, int tap4 = -1, int mixBit = 0);

    // Helper function to get the current output bit by XORing the MSBs
    int getOutput();
};

#endif // A51_ORIGINAL_H
