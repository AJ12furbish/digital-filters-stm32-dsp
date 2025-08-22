# Digital Filters (STM32 + MATLAB)

## Overview
This project focuses on the design and real-time implementation of **digital filters** using MATLAB for filter design and an STM32 microcontroller for deployment. Both **Finite Impulse Response (FIR)** and **Infinite Impulse Response (IIR)** filters were explored.  

Key objectives:
- Design FIR and IIR bandpass filters with specified characteristics.
- Compare **theoretical vs. measured frequency responses**.
- Implement filters in C on STM32 using:
  - Direct form
  - Circular buffers
  - Frame-based filtering with ARM CMSIS-DSP library
  - Cascade of biquads for IIR implementation

---

## Tools & Technologies
- **MATLAB/Simulink** – filter design, response analysis, coefficient export
- **STM32 MCU (ARM Cortex-M)** – real-time DSP implementation
- **ARM CMSIS-DSP library** – optimized FIR and IIR filter routines
- **Signal Generator + Oscilloscope** – measurement of frequency response

---

## FIR Filter (Week 1)
- **Design method:** Parks-McClellan (Equiripple)
- **Order:** 30 (31 coefficients)
- **Sampling Frequency:** 48 kHz
- **Passband:** 6–14 kHz
- **Stopbands:** <4 kHz, >16 kHz
- **Implementation:**
  - Direct form tapped delay line in C
  - Circular buffer optimization
  - Frame-based filtering with CMSIS-DSP

---

## IIR Filter (Week 2 & 3)
- **Design method:** Elliptic
- **Order:** 6
- **Sampling Frequency:** 48 kHz
- **Passband:** 5–15 kHz
- **Attenuation:** 60 dB stopband, 2 dB passband ripple
- **Implementation:**
  - Direct form I
  - Circular buffer optimization
  - Cascade of second-order sections (SOS/biquads)

---

## Experiments & Results
1. **Theoretical Response:** Computed using MATLAB `freqz`.
2. **Measured Response:** Collected using signal generator + oscilloscope.
3. **Comparison:** Plotted measured vs theoretical responses for FIR and IIR.
4. **Performance:** Compared execution cycles between direct form and circular buffer implementations.

---

## Block Diagram

### FIR Filter Implementation
![FIR Block](https://www.dsprelated.com/josimages_new/pasp/img479.png)

### IIR Direct Form
![IIR DF](https://ccrma.stanford.edu/~jos/fp/img1146.png)

### IIR Cascade (SOS)
![IIR SOS](https://docs-be.ni.com/bundle/labview-digital-filter-design-toolkit-api-ref/page/lvdfdtconcepts/cascaded_2nd_order_form_ii.gif?_LANG=enus)

---

## Key Takeaways
- FIR filters are stable and easier to design, but require more coefficients (higher order).
- IIR filters achieve sharp transitions with lower order but require careful stability management.
- Circular buffers improve efficiency by reducing memory operations.
- Frame-based processing with CMSIS-DSP is more efficient for real-time applications.

---

## References
- *Digital Signal Processing using Arm Cortex-M based Microcontrollers* – C. Ünsalan, M. Erkin Yücel, H. Deniz Gürhan  
- *Software Receiver Design* – C. R. Johnson, W. A. Sethares, A. Klein  
- ARM CMSIS-DSP Documentation: [FIR Filter Functions](https://www.keil.com/pack/doc/CMSIS/DSP/html/group__FIR.html)
