# FSR Voltage Divider Analysis — FORGE v1

Boundary conditions for the FSR signal chain. Every value here is a
hard input to the firmware velocity mapping and the PCB component
selection. Derived by hand and verified numerically.

## Circuit

Each of the 16 pads is an identical voltage divider:

3V3 ──[ FSR 402 ]──┬── to MUX channel
│
[10kΩ]  R2 (fixed)
│
[100nF] C_filter (to GND, RC low-pass)
│
GND

- **V_in** = 3.3 V (regulated rail)
- **R1** = FSR 402, ~100 kΩ unloaded → ~200 Ω full press
- **R2** = 10 kΩ fixed (divider resistor)
- **C_filter** = 100 nF (low-pass, anti-alias / noise reject)

## Divider equation

V_out = V_in × R2 / (R1 + R2)

ADC reading (12-bit, V_ref = 3.3 V):
ADC_count = (V_out / 3.3) × 4095

## Resistance → voltage → ADC table

| FSR R1 (Ω) | Condition        | V_out (V) | ADC count |
|-----------:|------------------|----------:|----------:|
|    100 000 | unloaded         |     0.300 |       372 |
|     50 000 |                  |     0.550 |       683 |
|     20 000 |                  |     1.100 |     1 365 |
|     10 000 | light touch      |     1.650 |     2 048 |
|      5 000 |                  |     2.200 |     2 730 |
|      2 000 |                  |     2.750 |     3 412 |
|      1 000 |                  |     3.000 |     3 723 |
|        500 |                  |     3.143 |     3 900 |
|        200 | full press       |     3.235 |     4 015 |

**Usable range:** ~372 → ~4015 counts, comfortably inside the 12-bit
0–4095 span. No clipping at either extreme.

## Firmware threshold

`VELOCITY_THRESHOLD` is set to **500 counts** (≈ 0.40 V). This sits
just above the ~372-count unloaded floor, so electrical noise and the
resting FSR value do not produce false triggers, while any deliberate
touch (which jumps well past 2000 counts) registers immediately.

`VELOCITY_MAX_ADC` is set to **3900** rather than the theoretical 4015,
so that a firm — not bone-crushing — strike already reaches velocity
127. This keeps the top of the dynamic range playable.

## RC low-pass filter

The 100 nF cap with the 10 kΩ divider resistor forms a low-pass filter:

f_c = 1 / (2π × R2 × C) = 1 / (2π × 10 000 × 100e-9) ≈ 159 Hz

A ~159 Hz cutoff smooths high-frequency electrical noise and mechanical
contact chatter while passing the strike envelope (which evolves over
milliseconds, well below 159 Hz). It also limits how fast the node can
slew, which sets the ~10 µs mux settle time used in `pad.c`.

## Design-decision notes (feeds docs/design_decisions.md)

- **Why a voltage divider, not direct resistance measurement:** the
  RP2040 ADC reads voltage, not resistance. The divider converts FSR
  resistance into a voltage the ADC can sample directly, with no
  active components.
- **Why R2 = 10 kΩ:** it places the "light touch" knee (~10 kΩ FSR)
  near mid-scale (1.65 V / 2048 counts), giving good resolution across
  the playable pressure range rather than bunching it at one end.
- **Why 100 nF:** sets a ~159 Hz cutoff — low enough to reject noise,
  high enough to preserve strike dynamics.