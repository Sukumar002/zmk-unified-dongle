# Universal Dongle project boundary

Status: isolated engineering project; no universal firmware is ready to flash.

## Protected baselines

- Hana remains on its normal shield/keymap and standard build path. Do not edit `boards/shields/hana/**` as part of this project.
- Chocofi remains on its normal shield/keymap and standard build path. Do not edit `boards/shields/chocofi/**` as part of this project.
- ZMK Studio custom build remains a separate public-release project. Version 1.0.3 is the protected release baseline; do not change its repository or release workflow here.
- No changes from this branch may be merged into `main` without a separately reviewed request.

## Project-only workspace

- Keyboard repository branch: `project/universal-dongle-isolated`.
- ZMK implementation dependency: `Sukumar002/zmk` branch `experiment/universal-dongle`, selected only through this project's `config/west.yml`.
- Project-only CI files: `build-universal-dongle-project.yaml` and `.github/workflows/universal-dongle-project.yml`.
- Existing Hana and Chocofi shield targets may be compiled as reference baselines, but their source files must not be edited by this project.
- Identity protocol flags may be supplied by CI command-line Kconfig for diagnostic builds; no standard board configuration is changed.

## Acceptance criteria before any test UF2 is offered

1. A dedicated universal dongle shield/keymap exists under a project-specific shield name and compiles against the intended nRF52840 dongle target.
2. Four split halves report stable board/side identities, independent of BLE connection order.
3. Central-side routing selects exactly one keyboard and uses that keyboard's layout, combos, hold-taps, and behaviors.
4. A switch drains/release all held HID usages before activating the other keyboard; old-board events cannot affect the new active keyboard.
5. If the active pair becomes unavailable, a complete healthy alternate pair can take over. A single surviving half is not treated as a complete keyboard. Recovery does not steal focus back automatically.
6. Bonds and selection survive restart without erasing the user's existing pairing data as a development shortcut.
7. CI artifacts include build provenance and are labeled diagnostic until routing and failover tests pass.

## Current limitation

The existing universal-router code is a policy model, and identity compile-checks do not prove end-to-end routing or runtime keymap selection. Current reference builds are not universal firmware and must not be flashed as such.
