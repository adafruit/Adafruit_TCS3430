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

---

## Detailed Register Reference

## Register Overview

| Address | Name | Access | Reset | Description |
|---------|------|--------|-------|-------------|
| 0x80 | ENABLE | R/W | 0x00 | Enables states and interrupts |
| 0x81 | ATIME | R/W | 0x00 | ADC integration time |
| 0x83 | WTIME | R/W | 0x00 | ALS wait time |
| 0x84 | AILTL | R/W | 0x00 | ALS interrupt low threshold low byte |
| 0x85 | AILTH | R/W | 0x00 | ALS interrupt low threshold high byte |
| 0x86 | AIHTL | R/W | 0x00 | ALS interrupt high threshold low byte |
| 0x87 | AIHTH | R/W | 0x00 | ALS interrupt high threshold high byte |
| 0x8C | PERS | R/W | 0x00 | ALS interrupt persistence filters |
| 0x8D | CFG0 | R/W | 0x80 | Configuration register zero |
| 0x90 | CFG1 | R/W | 0x00 | Configuration register one |
| 0x91 | REVID | R | 0x41 | Revision ID |
| 0x92 | ID | R | 0xDC | Device ID |
| 0x93 | STATUS | R | 0x00 | Device status register |
| 0x94 | CH0DATAL | R | 0x00 | Z — CH0 ADC low byte |
| 0x95 | CH0DATAH | R | 0x00 | Z — CH0 ADC high byte |
| 0x96 | CH1DATAL | R | 0x00 | Y — CH1 ADC low byte |
| 0x97 | CH1DATAH | R | 0x00 | Y — CH1 ADC high byte |
| 0x98 | CH2DATAL | R | 0x00 | IR1 — CH2 ADC low byte |
| 0x99 | CH2DATAH | R | 0x00 | IR1 — CH2 ADC high byte |
| 0x9A | CH3DATAL | R | 0x00 | X/IR2 — CH3 ADC low byte |
| 0x9B | CH3DATAH | R | 0x00 | X/IR2 — CH3 ADC high byte |
| 0x9F | CFG2 | R/W | 0x04 | Configuration register two |
| 0xAB | CFG3 | R/W | 0x0C | Configuration register three |
| 0xD6 | AZ_CONFIG | R/W | 0x7F | Auto zero configuration |
| 0xDD | INTENAB | R/W | 0x00 | Interrupt enables |

---

## ENABLE (0x80) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:4 | RESERVED | 0 | RW | Reserved |
| 3 | WEN | 0 | RW | Wait Enable. 1 = activate wait timer, 0 = disable |
| 2 | RESERVED | 0 | RW | Reserved |
| 1 | AEN | 0 | RW | ALS Enable. Set AEN=1 and PON=1 together to ensure autozero runs first |
| 0 | PON | 0 | RW | Power ON. Activates internal oscillator for timers and ADC |

> **Note:** Write mode/parameter fields before asserting AEN. AEN requires PON.

---

## ATIME (0x81) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | ATIME | 0x00 | RW | Integration time in 2.78ms increments |

| Value | Cycles | Time | Max ALS |
|-------|--------|------|---------|
| 0x00 | 1 | 2.78ms | 1023 |
| 0x01 | 2 | 5.56ms | 2047 |
| 0x11 | 18 | 50ms | 18431 |
| 0x40 | 65 | 181ms | 65535 |
| 0xFF | 256 | 711ms | 65535 |

---

## WTIME (0x83) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | WTIME | 0x00 | RW | Wait time in 2.78ms increments (×12 if WLONG set) |

| Value | Cycles | Wait Time (normal/WLONG) |
|-------|--------|--------------------------|
| 0x00 | 1 | 2.78ms / 33.4ms |
| 0x23 | 36 | 100ms / 1.20s |
| 0xFF | 256 | 711ms / 8.53s |

---

## AILTL (0x84) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | AILTL | 0x00 | RW | Low byte of ALS interrupt low threshold (CH0) |

---

## AILTH (0x85) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | AILTH | 0x00 | RW | High byte of ALS interrupt low threshold (CH0) |

> **Note:** AILTH:AILTL form a 16-bit threshold. A latch holds the low byte until the high byte is written.

---

## AIHTL (0x86) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | AIHTL | 0 | RW | Low byte of ALS interrupt high threshold |

---

## AIHTH (0x87) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | AIHTH | 0 | RW | High byte of ALS interrupt high threshold |

> **Note:** AIHTH:AIHTL form a 16-bit threshold. If CH0 exceeds this and APERS count reached, AINT asserts.

---

## PERS (0x8C) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:4 | RESERVED | 0 | RW | Reserved |
| 3:0 | APERS | 0 | RW | ALS interrupt persistence filter |

| APERS | Interrupt When… |
|-------|------------------|
| 0 | Every ALS cycle |
| 1 | Any value outside threshold |
| 2 | 2 consecutive out of range |
| 3 | 3 consecutive |
| 4 | 5 consecutive |
| 5 | 10 consecutive |
| 6 | 15 consecutive |
| 7 | 20 consecutive |
| 12 | 45 consecutive |
| 13 | 50 consecutive |
| 14 | 55 consecutive |
| 15 | 60 consecutive |

---

## CFG0 (0x8D) — Reset: 0x80

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:3 | RESERVED | 10000 | RW | Reserved. Must be set to 10000 |
| 2 | WLONG | 0 | RW | Wait Long. 1 = wait cycle ×12 from WTIME |
| 1:0 | RESERVED | 00 | RW | Reserved. Must be set to 00 |

---

## CFG1 (0x90) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:4 | RESERVED | 0 | RW | Reserved |
| 3 | AMUX | 0 | RW | ALS Mux. 0 = X channel, 1 = IR2 on CH3 |
| 2 | RESERVED | 0 | RW | Reserved |
| 1:0 | AGAIN | 0 | RW | ALS Gain: 00=1×, 01=4×, 10=16×, 11=64× |

---

## REVID (0x91) — Reset: 0x41

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:3 | RESERVED | 01000 | RO | Reserved |
| 2:0 | REV_ID | 001 | RO | Revision Number Identification |

---

## ID (0x92) — Reset: 0xDC

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:2 | ID | 110111 | RO | Part Number ID (110111 = TCS3430) |
| 1:0 | RESERVED | 00 | RO | Reserved |

---

## STATUS (0x93) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7 | ASAT | 0 | R/SC | ALS Saturation. Write 1 to clear |
| 6:5 | RESERVED | 0 | R/SC | Reserved |
| 4 | AINT | 0 | R/SC | ALS Interrupt. Write 1 to clear |
| 3:0 | RESERVED | 0 | R/SC | Reserved |

---

## CH0DATAL (0x94) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | CH0DATAL | 0 | RO | Low byte of CH0 ADC — Z data |

## CH0DATAH (0x95) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | CH0DATAH | 0 | RO | High byte of CH0 ADC — Z data |

## CH1DATAL (0x96) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | CH1DATAL | 0 | RO | Low byte of CH1 ADC — Y data |

## CH1DATAH (0x97) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | CH1DATAH | 0 | RO | High byte of CH1 ADC — Y data |

## CH2DATAL (0x98) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | CH2DATAL | 0 | RO | Low byte of CH2 ADC — IR1 data |

## CH2DATAH (0x99) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | CH2DATAH | 0 | RO | High byte of CH2 ADC — IR1 data |

## CH3DATAL (0x9A) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | CH3DATAL | 0 | RO | Low byte of CH3 ADC — X (AMUX=0) or IR2 (AMUX=1) |

## CH3DATAH (0x9B) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:0 | CH3DATAH | 0 | RO | High byte of CH3 ADC — X (AMUX=0) or IR2 (AMUX=1) |

---

## CFG2 (0x9F) — Reset: 0x04

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7:5 | RESERVED | 000 | RW | Reserved. Set to 000 |
| 4 | HGAIN | 0 | RW | High 128× gain. Set with AGAIN=11 for 128× mode |
| 3:0 | RESERVED | 0100 | RW | Reserved. Set to 0100 |

---

## CFG3 (0xAB) — Reset: 0x0C

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7 | INT_READ_CLEAR | 0 | RW | If set, STATUS flags reset on I2C read |
| 6:5 | RESERVED | 0 | RW | Reserved. Set to 0 |
| 4 | SAI | 0 | RW | Sleep After Interrupt. Powers down after ALS cycle if interrupt generated |
| 3:0 | RESERVED | 1100 | RW | Reserved. Any value OK; no functional effect |

> **Note:** SAI uses the interrupt signal to turn OFF the oscillator. Wake by clearing STATUS (0x93).

---

## AZ_CONFIG (0xD6) — Reset: 0x7F

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7 | AZ_MODE | 0 | RW | 0 = start at zero, 1 = start at previous offset (faster on average) |
| 6:0 | AZ_NTH_ITERATION | 0x7F | RW | Autozero every Nth ALS cycle. 0=never, 0x7F=first cycle only |

---

## INTENAB (0xDD) — Reset: 0x00

| Bits | Name | Reset | Access | Description |
|------|------|-------|--------|-------------|
| 7 | ASIEN | 0 | RW | 1 = enable ASAT interrupt |
| 6:5 | RESERVED | 0 | RW | Reserved |
| 4 | AIEN | 0 | RW | 1 = enable ALS interrupt |
| 3:0 | RESERVED | 0 | RW | Reserved |
