# CD74HC4067 Mux & MCP1700 LDO Analysis — FORGE v1

Datasheet study for the two supporting ICs in the analog front end:
the 16:1 multiplexer that time-shares the ADC across 16 FSR pads, and
the LDO that regulates the 3.3V analog/digital rail.

## CD74HC4067 — 16-channel analog multiplexer/demultiplexer

**Package check:** BOM lists "SOIC-24" for part CD74HC4067M96 — confirmed
correct. The device is a 24-pin part (available in PDIP-24, SOIC-24,
SSOP-24), *not* 16-pin as might be assumed from "16-channel." The extra
pins are the 16 channel I/Os (Y0–Y15) plus 4 select lines (S0–S3), the
common I/O (Z), enable (E), VCC, and GND.

**S3:S0 truth table (channel select):**

| S3 | S2 | S1 | S0 | Channel selected |
|---:|---:|---:|---:|:-----------------:|
|  0 |  0 |  0 |  0 | Y0  |
|  0 |  0 |  0 |  1 | Y1  |
|  0 |  0 |  1 |  0 | Y2  |
|  0 |  0 |  1 |  1 | Y3  |
|  0 |  1 |  0 |  0 | Y4  |
|  0 |  1 |  0 |  1 | Y5  |
|  0 |  1 |  1 |  0 | Y6  |
|  0 |  1 |  1 |  1 | Y7  |
|  1 |  0 |  0 |  0 | Y8  |
|  1 |  0 |  0 |  1 | Y9  |
|  1 |  0 |  1 |  0 | Y10 |
|  1 |  0 |  1 |  1 | Y11 |
|  1 |  1 |  0 |  0 | Y12 |
|  1 |  1 |  0 |  1 | Y13 |
|  1 |  1 |  1 |  0 | Y14 |
|  1 |  1 |  1 |  1 | Y15 |

Standard binary counting, S0 = LSB, S3 = MSB — matches `mux_select()` in
`mux.c`, which writes exactly this pattern via `gpio_put()` calls.

**Enable (E):** active LOW. E = HIGH disables all switches (device goes
to high-impedance "off" state on every channel). This matches
`mux_enable()`/`mux_disable()` in `mux.c`, which drive `MUX_EN_PIN` low
to enable and high to disable.

**On-resistance:** the datasheet's "ON" Resistance table gives values
across the supply range. At V_CC = 4.5V, typical R_ON ≈ 70Ω; at
V_CC = 6V, typical R_ON ≈ 60Ω. FORGE runs the mux at 3.3V, below the
characterized 4.5V point, so actual R_ON at 3.3V will run somewhat
higher than the 70Ω typical figure (on-resistance increases as V_CC
drops on this family) — worst case, budget for on the order of
~100–150Ω at 3.3V, and treat it as a data-sheet-adjacent estimate
rather than a guaranteed number.

**Why R_ON matters for FORGE:** the mux sits in series between the FSR
divider's output node and the ADC input. R_ON adds to the effective
source impedance the ADC sees. The RP2040 ADC wants a source impedance
under roughly a few kΩ for accurate settling within its sample time, so
even ~100–150Ω of added mux resistance is a small fraction of the total
circuit impedance (dominated by the 10kΩ divider resistor) and is not a
practical concern here.

**Switching speed:** typical switching/propagation time ≈ 80ns per
independent measurement, with break-before-make transition of ~6ns at
4.5V. Both are far faster than the 10µs settle delay already used in
`pad.c`'s `read_pad_adc()` — the mux itself is not the bottleneck; the
10µs is dominated by the RC filter's own settling time (see
`voltage_divider_analysis.md`), not the mux's internal switching speed.

**3.3V compatibility:** the datasheet's recommended operating range for
this HC-family part spans roughly 2V–6V, so 3.3V operation is within
spec and standard practice for this chip.

## MCP1700T-3302E/TT — 3.3V LDO regulator

**Dropout voltage:** the defining spec for this part. Datasheet states
178mV typical dropout at 250mA load (SOT-23 package) — usable input
voltage need only exceed 3.3V + 0.178V ≈ 3.48V for the LDO to hold
regulation at full rated current. At FORGE's actual load (far under
250mA — just the Pico plus mux and LEDs), real dropout will be even
smaller than 178mV, giving comfortable margin off USB's nominal 5V.

**Quiescent current:** 1.6µA typical — negligible relative to the
Pico's own current draw, so it does not meaningfully affect the
project's power budget.

**Input capacitor:** datasheet recommends 1µF on both input and output
for stable operation — matches the 1µF cap called out in the master
plan's power section.

**Package/pinout:** SOT-23-3, standard 3-pin LDO (VIN, GND, VOUT).

## Summary — what this confirms for FORGE

- Mux truth table and enable polarity match the firmware exactly — no
  changes needed to `mux.c`.
- Mux on-resistance at 3.3V is not precisely characterized in the
  datasheet (specs start at 4.5V), but is small relative to the divider
  resistance regardless, so it's a non-issue rather than a risk.
- Mux switching speed is not the limiting factor in the 10µs settle
  window — the RC filter is.
- LDO dropout (178mV @ 250mA, less at FORGE's real load) leaves ample
  margin between USB's 5V supply and the 3.3V rail.