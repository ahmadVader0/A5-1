#ifndef A51_ENHANCED_H
#define A51_ENHANCED_H

#include <cstdint>

// Class representing the enhanced version of the A5/1 cipher
// It features 4 registers, a nonlinear clocking decision, and a nonlinear output filter
class A51Enhanced {
public:
    A51Enhanced();

    // Initializes the cipher with a 64-bit key and 22-bit frame number
    void initialize(uint64_t key, uint32_t frameNumber);

    // Generates a single keystream bit
    int generateBit();

    // Generates multiple keystream bits into a caller-provided array
    void generateKeystream(int output[], int numBits);

    // Resets all registers to zero
    void reset();

    // Gets the MSB output of each register (for analysis)
    void getRegisterOutputs(int &r1Out, int &r2Out, int &r3Out, int &r4Out);

private:
    // Registers of A5/1 Enhanced represented as simple arrays of bits (0 or 1)
    int R1[19];
    int R2[22];
    int R3[23];
    int R4[25];

    // Determines the clocking decision for the cipher using selected register bits
    int clockingDecision();

    // Executes clocking for selected registers based on the decision value
    void applyClocking(int decision);

    // Helper function to clock a single register using tap positions
    void clockRegister(int reg[], int length, int tap1, int tap2, int tap3 = -1, int tap4 = -1, int mixBit = 0);

    // Helper function to get the current output bit combining MSBs with a nonlinear term
    int getOutput();
};

#endif // A51_ENHANCED_H
