# FORGE-v1

A custom 4×4 velocity-sensitive USB MIDI pad controller built from scratch, featuring a 16-step onboard sequencer, custom 2-layer PCB, hand-soldered SMD assembly, and laser-cut enclosure.

## Overview

This project is a fully custom MIDI controller designed and built from the ground up. The hardware is centered around the RP2040 microcontroller, with force-sensitive resistors (FSRs) scanned through a 16:1 analog multiplexer for per-pad velocity detection. Firmware is written in embedded C using the Pico SDK, implementing logarithmic velocity mapping, and class-compliant USB MIDI via TinyUSB.

## Features

- 4×4 FSR pad grid with velocity sensitivity
- 16-step onboard sequencer
- Custom 2-layer PCB designed in Altium Designer
- Hand-soldered SMD assembly
- Laser-cut enclosure
- Class-compliant USB MIDI (no driver required)

## Status

**Currently In Progress** — Currently prototyping circuit design; estimated completion by early 2027

## Repository Structure

- `firmware/` — Embedded C source code (Pico SDK, TinyUSB)
- `hardware/` — PCB schematics and layout files (Altium Designer)
- `docs/` — Design notes and test procedures
- `media/` — Photos and demo media
