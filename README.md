# Unified Dongle

A standalone ZMK firmware project for a shared BLE dongle that can route input from supported split-keyboard configurations.

## Project contents

- `boards/shields/universal_dongle/` — dongle shield, keymap, and 128×64 OLED configuration.
- `config/west.yml` — pinned ZMK and dongle-display dependencies.
- `.github/workflows/` — configuration audit, routing smoke checks, and unit tests.
- `build-universal-dongle-routing-smoke.yaml` — routing smoke-test build definition.
- `docs/universal-dongle/` — project boundaries and shared keymap/OLED requirements.

## Build and verification

Use the repository's GitHub Actions workflows as the source of truth for configuration validation and firmware build results. A successful CI build is not a substitute for the final physical-hardware switching test.

## Release scope

Unified Dongle releases are maintained separately from the multi-keyboard firmware repository.
