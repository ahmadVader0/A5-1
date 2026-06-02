# Experimental Methodology & Results

## Overview

This document describes the three experiments used to compare the original A5/1 cipher against the enhanced version. All experiments are reproducible with fixed seeds.

---

## General Parameters

| Parameter | Value |
|-----------|-------|
| Keystream length per trial | 1,000,000 bits (1M) |
| Number of random keys (avalanche) | 1,000 |
| Key size | 64 bits |
| IV (frame number) size | 22 bits |
| Output format | CSV files in `/data/results/` |

---

## Experiment 1: Avalanche Effect

### What it measures

How much the keystream output changes when a single bit in the key is flipped. Ideally, changing 1 bit in the key should change ~50% of the output bits (good diffusion).

### Steps

1. Pick a random 64-bit key `K` and a random 22-bit IV
2. Generate `N` bits of keystream using key `K` → call it `stream1`
3. Flip one bit in `K` to get `K'`
4. Generate `N` bits using `K'` → call it `stream2`
5. Count how many bits differ between `stream1` and `stream2`
6. Avalanche % = (differing bits / N) × 100
7. Repeat for 1,000 random keys
8. Report the **mean**, **standard deviation**, **min**, and **max**

### Expected Results

| Version | Expected Mean | Expected Std Dev |
|---------|--------------|-----------------|
| Original A5/1 | ~47–49% | Higher |
| Enhanced A5/1 | ~49.5–50.5% | Lower |

### Output Files

- `data/results/avalanche_original.csv`
- `data/results/avalanche_enhanced.csv`

---

## Experiment 2: Shannon Entropy

### What it measures

How "random" the keystream looks. Shannon entropy quantifies the unpredictability of the bit patterns. For a perfectly random binary stream, normalized entropy = 1.0.

### Steps

1. Generate a long keystream (1M bits)
2. Split it into blocks of size B (test with B = 4, 8, 16)
3. Count how often each possible block pattern appears
4. Compute Shannon entropy:
   ```
   H = -Σ p(x) × log₂(p(x))
   ```
   where p(x) = frequency of each pattern / total blocks
5. Normalize: `H_norm = H / B`
6. Repeat for both cipher versions

### Expected Results

| Version | Expected Normalized Entropy |
|---------|---------------------------|
| Original A5/1 | Slightly below 1.0 at larger block sizes |
| Enhanced A5/1 | Consistently closer to 1.0 |

### Output Files

- `data/results/entropy_original.csv`
- `data/results/entropy_enhanced.csv`

---

## Experiment 3: Frequency / Bias Analysis

### What it measures

Whether the keystream has an equal number of 0s and 1s. A biased output (more 0s than 1s or vice versa) is a weakness an attacker can exploit.

### Steps

1. Generate a long keystream (1M+ bits)
2. Count total 0s and total 1s
3. Compute bias: `bias = |count_1 - count_0| / total_bits`
4. Also divide the stream into windows of 1000 bits each
5. Compute the 1-bit frequency within each window
6. Check that per-window frequencies stay close to 0.5

### Expected Results

| Version | Expected Bias |
|---------|--------------|
| Original A5/1 | Small but detectable deviation from 0.5 |
| Enhanced A5/1 | Even closer to 0.0 (more balanced) |

### Output Files

- `data/results/frequency_original.csv`
- `data/results/frequency_enhanced.csv`

---

## Generated Visualizations

The experimental results have been visualized and saved as high-resolution plots under `report/figures/`.

### 1. Overall Metrics Comparison
This chart includes the box plots of the Avalanche Effect, Shannon Entropy comparison by block sizes, and Frequency distribution comparisons.

![All Metrics Comparison](../report/figures/all_metrics_comparison.png)

### 2. Detailed Distribution Analysis
This chart displays the Cumulative Distribution Functions (CDFs) for both Avalanche and Frequency, along with a Q-Q normality check.

![Detailed Analysis](../report/figures/detailed_analysis.png)


---

## Reproducibility

```bash
# Compile the C++ program
g++ -o a51_project src/main.cpp src/a51_original.cpp src/a51_enhanced.cpp src/analysis.cpp -Isrc

# Run all experiments
./a51_project
```

All random seeds are fixed in `src/main.cpp` for identical results across runs.

