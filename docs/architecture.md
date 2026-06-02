# A5/1 Stream Cipher — Architecture Deep-Dive

## 1. Historical Context

A5/1 is a stream cipher developed in 1987, used to encrypt voice and data communication in the GSM (Global System for Mobile Communications) standard. It was deployed in millions of mobile phones worldwide, making it one of the most widely used ciphers in history.

The cipher was originally kept secret, but its design was reverse-engineered and published in 1999, exposing fundamental weaknesses that have since been extensively studied in academic cryptanalysis.

---

## 2. Core Components

### 2.1 Linear Feedback Shift Registers (LFSRs)

A5/1 uses three LFSRs of different lengths:

| Register | Length (bits) | Feedback Taps | Clocking Bit |
|----------|--------------|---------------|--------------|
| R1 | 19 | 13, 16, 17, 18 | Bit 8 |
| R2 | 22 | 20, 21 | Bit 10 |
| R3 | 23 | 7, 20, 21, 22 | Bit 10 |

Each LFSR is a shift register where the input bit (feedback) is computed as the XOR of specific bit positions (tap positions). The choice of tap positions corresponds to **primitive polynomials** over GF(2), ensuring the register cycles through all non-zero states (maximal length sequence).

#### Primitive Polynomials

| Register | Polynomial |
|----------|-----------|
| R1 | x¹⁹ + x¹⁸ + x¹⁷ + x¹⁶ + x¹³ + 1 |
| R2 | x²² + x²¹ + x²⁰ + 1 |
| R3 | x²³ + x²² + x²¹ + x²⁰ + x⁷ + 1 |

### 2.2 Majority-Rule Clocking Mechanism

At each clock cycle, the **clocking bits** of the three registers are examined:

- R1 clocking bit: bit 8
- R2 clocking bit: bit 10
- R3 clocking bit: bit 10

The **majority value** (the value that appears at least twice among the three clocking bits) is computed. Only the registers whose clocking bit **matches** the majority value are stepped.

```
majority = majority_of(R1[8], R2[10], R3[10])

if R1[8] == majority → step R1
if R2[10] == majority → step R2
if R3[10] == majority → step R3
```

This means at each cycle, either **2 or 3 registers** are clocked (never just 1, never 0).

### 2.3 Output Generation

The keystream bit at each cycle is the XOR of the **most significant bits (MSBs)** of all three registers:

```
output_bit = R1[18] XOR R2[21] XOR R3[22]
```

This output bit is then XORed with the plaintext bit to produce ciphertext.

---

## 3. Initialization Procedure

### Step 1: Zero State
All three registers are set to zero.

### Step 2: Key Loading (64 cycles)
For each bit of the 64-bit session key Kc:
- All three registers are clocked (ignoring the majority rule)
- The key bit is XORed into the LSB (bit 0) of each register

### Step 3: Frame Number Loading (22 cycles)
For each bit of the 22-bit frame number (IV):
- All three registers are clocked (ignoring the majority rule)
- The frame bit is XORed into the LSB of each register

### Step 4: Warm-up (100 cycles)
- Run the cipher with majority-rule clocking for 100 cycles
- **Discard** all output bits (they are not used)

### Step 5: Keystream Generation
After warm-up, the cipher produces keystream bits using the normal clocking and output mechanism:
- 114 bits for downlink encryption
- 114 bits for uplink encryption

---

## 4. State Space Analysis

| Property | Value |
|----------|-------|
| Total internal state | 19 + 22 + 23 = **64 bits** |
| Key length | **64 bits** |
| IV (frame number) | **22 bits** |
| Keystream per frame | **228 bits** (114 down + 114 up) |
| LFSR period (combined) | ≈ 2⁶⁴ (product of individual periods) |

---

## 5. Diagram

```
┌──────────────────────────────────────────────────────┐
│                        A5/1                          │
│                                                      │
│  ┌──────────────────┐    Clocking                    │
│  │   R1 (19 bits)   │──── bit 8 ──────┐              │
│  │  taps: 13,16,17,18│                 │              │
│  └──────────────────┘                 │              │
│           │                           ▼              │
│           │                     ┌──────────┐         │
│           │                     │          │         │
│  ┌──────────────────┐           │          │         │
│  │   R2 (22 bits)   │──── bit 10┤ MAJORITY ├─ CLOCK  │
│  │  taps: 20,21     │           │ FUNCTION │         │
│  └──────────────────┘           │          │         │
│           │                     │          │         │
│           │                     └──────────┘         │
│  ┌──────────────────┐                 ▲              │
│  │   R3 (23 bits)   │──── bit 10──────┘              │
│  │  taps: 7,20,21,22│                                │
│  └──────────────────┘                                │
│           │                                          │
│           │ MSB                                      │
│           ▼                                          │
│     ┌───────────┐                                    │
│     │  XOR (⊕)  │───────────► Keystream bit          │
│     └───────────┘                                    │
└──────────────────────────────────────────────────────┘
```

---

## 6. Security Observations

The architecture has several inherent weaknesses stemming from its simplicity:

1. **Linear feedback**: Every LFSR is purely linear over GF(2), meaning the internal state evolves linearly and is algebraically tractable.

2. **Predictable clocking**: The majority function depends on only 3 bits, making it possible to predict clocking behavior with probability > 0.5.

3. **Small state space**: 64 bits of internal state is feasible to attack with modern computing resources.

4. **Linear output**: XOR of MSBs preserves linearity, enabling correlation and algebraic attacks.

These observations form the basis for the proposed enhancements documented in [enhancements.md](enhancements.md).
