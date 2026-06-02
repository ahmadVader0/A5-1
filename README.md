# Cryptanalysis and Enhancement of A5/1 Stream Cipher

> **"Cryptanalysis and Enhancement of A5/1 Stream Cipher Using Dynamic Clocking, Extended LFSR Architecture, and Nonlinear Output Combining"**

Welcome to the **A5/1 Stream Cipher Cryptanalysis and Enhancement** project repository. This project explores the architectural vulnerabilities of the standard GSM A5/1 stream cipher, proposes a set of targeted cryptographic enhancements, and compares both implementations using empirical statistical tests.

---

## 📂 Repository Roadmap

Click the links below to navigate the documentation files and folders:

* **[src/](src/)** — C++ Source files:
  * [a51_original.h](src/a51_original.h) & [a51_original.cpp](src/a51_original.cpp) — Core implementation of the standard A5/1 GSM cipher.
  * [a51_enhanced.h](src/a51_enhanced.h) & [a51_enhanced.cpp](src/a51_enhanced.cpp) — Implementation of our proposed Enhanced A5/1 cipher.
  * [analysis.h](src/analysis.h) & [analysis.cpp](src/analysis.cpp) — Statistics gathering engine (Avalanche, Entropy, Bias).
  * [main.cpp](src/main.cpp) — Main test execution entry point.
* **[docs/](docs/)** — Detailed documentation chapters:
  * [architecture.md](docs/architecture.md) — Deep-dive into standard A5/1 LFSR configurations and majority-rule clocking.
  * [enhancements.md](docs/enhancements.md) — Detailed explanation of our proposed modifications (4th LFSR, state-dependent clocking, nonlinear combining).
  * [experiments.md](docs/experiments.md) — Steps and methodologies used in our Avalanche, Shannon Entropy, and Frequency Bias tests.
  * [references.md](docs/references.md) — Academic bibliography and reference materials.
* **[data/results/](data/results/)** — Exported CSV datasets containing experimental logs.
* **[report/figures/](report/figures/)** — Generated high-resolution visualization graphs.
* **[task.md](task.md)** — Project implementation progress tracker.
* **[plan.md](plan.md)** — Original academic planning document.

---

## 🛠️ Cipher Implementations Overview

The project compares two cipher variations:

1. **Original A5/1 Cipher:**
   * **State Space:** 64 bits across 3 LFSRs (R1: 19-bit, R2: 22-bit, R3: 23-bit).
   * **Clocking Rule:** Deterministic majority-rule based on 3 clocking tap bits.
   * **Output Function:** Linear XOR combination of the registers' MSBs.

2. **Enhanced A5/1 Cipher:**
   * **State Space:** **89 bits** (+25 bits) with the addition of a **4th LFSR (R4)**, raising the complexity of brute-force and table-based lookup attacks.
   * **Clocking Rule:** Dynamic, state-dependent multi-bit clocking decision computed from 8 register bits, preventing register isolation.
   * **Output Function:** Nonlinear Boolean combining function ($a \oplus b \oplus (c \land d)$) to resist algebraic attacks.

---

## 📊 Experimental Results

Tests were performed over $1,000,000$ bits of generated keystreams. The empirical comparison is summarized below:

| Metric Tested | Ideal Value | Original A5/1 | Enhanced A5/1 | Outcome & Improvement |
| :--- | :--- | :--- | :--- | :--- |
| **Avalanche Effect (Mean)** | 50.0000% | 49.9527% | **50.0328%** | **Improved** (+0.0145% closer to ideal) |
| **Avalanche Effect (Std Dev)** | 0.0000% | 0.5298% | **0.4886%** | **Improved** (7.8% tighter distribution) |
| **Shannon Entropy (4-bit)** | 1.000000 | 0.999990 | 0.999990 | Equal / Near-perfect |
| **Shannon Entropy (8-bit)** | 1.000000 | 0.999836 | 0.999811 | Similar |
| **Shannon Entropy (16-bit)** | 1.000000 | 0.946236 | 0.946130 | Similar |
| **Frequency Bias (1s / 0s)** | 50% / 50% | 50.0777% / 49.9223% | **49.9928% / 50.0072%** | **Major Gain** (Highly balanced) |
| **Frequency Bias (Absolute)** | 0.0000% | 0.077700% | **0.007200%** | **90.73% reduction in bias** |

---

## 📈 Visualizations

The generated metrics are plotted and saved. Below are the charts comparing the ciphers:

### 1. Overall Metrics Comparison
This dashboard displays the comparative box plots of the Avalanche Effect, Shannon Entropy comparison by block sizes, and Frequency distributions.

![All Metrics Comparison](report/figures/all_metrics_comparison.png)

### 2. Cumulative Distribution & Normality Checks
This chart compares the CDF lines of the ciphers and checks the normality of the avalanche distribution.

![Detailed Analysis](report/figures/detailed_analysis.png)

---

## 🚀 Building and Running

### Prerequisites
To compile the C++ source code, you need a C++ compiler supporting standard features (e.g., `g++`):
```bash
g++ --version
```

### Compiling the Project
Compile the executable from the repository root:
```bash
g++ -o a51_project src/main.cpp src/a51_original.cpp src/a51_enhanced.cpp src/analysis.cpp -Isrc
```

### Running the Experiments
Execute the binary to run the avalanche, entropy, and frequency experiments and overwrite the CSV datasets:
```bash
./a51_project
```
The output logs will print to the console and write results to the [data/results/](data/results/) directory.
