# TCS3430 — XYZ Tristimulus Color Sensor Design Document

## Chip Overview
- **Manufacturer:** ams-OSRAM
- **I2C Address:** 0x39 (fixed)
- **Chip ID:** 0xDC (register 0x92)
- **Function:** CIE 1931 XYZ tristimulus color sensor with IR channel
- **Channels:** Z (CH0), Y (CH1), IR1 (CH2), X or IR2 (CH3, muxed)
- **Key Feature:** Human eye response matching via XYZ color space

## Register Map

| Addr | Name | R/W | Description |
|------|------|-----|-------------|
| 0x80 | ENABLE | R/W | Power on (bit 0), ALS enable (bit 1), Wait enable (bit 3) |
| 0x81 | ATIME | R/W | ALS integration time: (ATIME+1) × 2.78ms |
| 0x83 | WTIME | R/W | Wait time: (WTIME+1) × 2.78ms (×12 if WLONG) |
| 0x84-0x85 | AILTL/AILTH | R/W | ALS interrupt low threshold (16-bit LE) |
| 0x86-0x87 | AIHTL/AIHTH | R/W | ALS interrupt high threshold (16-bit LE) |
| 0x8C | PERS | R/W | ALS interrupt persistence filter [3:0] |
| 0x8D | CFG0 | R/W | WLONG bit 2 (12× wait multiplier) |
| 0x90 | CFG1 | R/W | AGAIN [1:0] (gain), AMUX bit 3 (IR2 vs X on CH3) |
| 0x91 | REVID | R | Revision ID |
| 0x92 | ID | R | Device ID (0xDC) |
| 0x93 | STATUS | R | ASAT (bit 7), AINT (bit 4) — write 1 to clear |
| 0x94-0x95 | CH0DATA | R | Z channel (16-bit LE) |
| 0x96-0x97 | CH1DATA | R | Y channel (16-bit LE) |
| 0x98-0x99 | CH2DATA | R | IR1 channel (16-bit LE) |
| 0x9A-0x9B | CH3DATA | R | X or IR2 channel (16-bit LE, depends on AMUX) |
| 0x9F | CFG2 | R/W | HGAIN bit 4 (128× mode) |
| 0xAB | CFG3 | R/W | INT_READ_CLEAR bit 7, SAI bit 4 |
| 0xD6 | AZ_CONFIG | R/W | AZ_MODE bit 7, AZ_NTH_ITERATION [6:0] |
| 0xDD | INTENAB | R/W | ASIEN bit 7 (sat int), AIEN bit 4 (ALS int) |

## Hardware Notes
- **INT pin:** Breakout has an inverter on INT output for level shifting. Sensor is active-low open-drain, inverter is also open-drain. Needs INPUT_PULLUP. Active-HIGH at MCU.

## Gain Settings
| Enum | AGAIN[1:0] | HGAIN | Effective Gain |
|------|-----------|-------|----------------|
| 0 | 00 | 0 | 1× |
| 1 | 01 | 0 | 4× |
| 2 | 10 | 0 | 16× |
| 3 | 11 | 0 | 64× |
| 4 | 11 | 1 | 128× |

## Persistence Filter Values
PERS[3:0]: 0=every, 1-3=1-3 cycles, 4=5, 5=10, 6=15, 7=20, 8=25, 9=30, A=35, B=40, C=45, D=50, E=55, F=60

## Current Library Status

### Implemented (Complete)
- Power on/off, ALS enable, wait enable
- Integration time (cycles and ms)
- Wait time (cycles, ms, WLONG multiplier)
- ALS gain (1×–128× with HGAIN handling)
- ALS interrupt thresholds (low/high)
- Interrupt persistence filter
- AMUX (IR2 vs X channel selection)
- Channel data read (Z, Y, IR1/X/IR2) — 6-byte burst
- Status: saturation and interrupt flags (read + clear)
- CFG3: interrupt clear on read, sleep after interrupt
- Auto-zero: mode and interval
- Interrupt enables: ALS and saturation

### Missing / Needs Work
1. **No `getIR1()` / `getIR2()` individual channel reads** — only `getData(&x, &y, &z)` which misses IR
2. **getData() doesn't return all 4 channels** — reads 6 bytes (CH0-CH2) but CH3 is separate
3. **No data-valid check** — should check STATUS AVALID bit before reading data
4. **No simpletest example** — only a test sketch that exercises all features
5. **Missing CI workflow** — no `.github/workflows/githubci.yml`
6. **Missing doxygen** — no `Doxyfile`
7. **No LICENSE file content** — LICENSE file exists but need to verify BSD text
8. **getData() naming is confusing** — params named x,y,z but reads CH0(Z), CH1(Y), CH2(IR1)
9. **No hw_tests/** — need hardware test suite

### API Improvements Needed
- `getChannel(ch)` or individual `getX()`, `getY()`, `getZ()`, `getIR1()`, `getIR2()`
- `readAllChannels()` that returns all 4 (switching AMUX to get both X and IR2)
- Data-valid polling before channel reads
- Better example(s): simpletest, XYZ color reading, interrupt demo
