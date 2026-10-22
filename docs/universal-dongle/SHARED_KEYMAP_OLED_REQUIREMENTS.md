# Universal dongle shared keymap and OLED requirements

Status: implementation acceptance contract; not evidence that these behaviors are implemented.

## Keymap model

- The active board owns one board-specific base layer: `HBASE` for Hana and `CBASE` for Chocofi.
- Non-base layers are shared between both boards: `NUM`, `NAV`, `MOUSE`, `SET`, and runtime-editable Studio layers.
- Switching the active board must atomically select the corresponding base layer while preserving the shared layer definitions and runtime settings.
- Native key positions must be translated through the board-specific position map before events reach the common keymap. The two matrices must not be treated as interchangeable row-major arrays.

## Shared combos

- Use one common combo catalogue for both boards, alongside the shared non-base layers.
- Combo intent and bindings should be shared where the same action is meaningful on both boards.
- Because Hana and Chocofi have different physical matrix sizes and native position numbering, each combo must declare or resolve board-specific physical positions that map to the same intended logical keys. Never copy raw position numbers between boards without translating and verifying them.
- Combos that depend on a key or thumb position absent on one board must be adapted to an equivalent position or explicitly marked unsupported for that board; they must not silently trigger on unrelated keys.
- Verify common combos on HBASE and CBASE and on shared layers, including simultaneous key presses near a profile switch. Pending combo state from the old active board must be cancelled/drained during handover so it cannot trigger after the new board becomes active.

## Switching safety

- A switch request may originate from either keyboard, including when that keyboard is currently inactive.
- The dongle must validate the requesting board identity and require a complete, connected left/right pair before activating it.
- Drain all accepted HID key/button presses from the old active board before enabling input from the new board.
- Ignore input from the inactive board, except for a valid, explicit switch request.
- A failed or incomplete transition must not leave stuck keys or route old-board releases into the new board.

## OLED identity and battery display

- Keep the Bluetooth status indicator at the top right.
- Use the available space beside it for a keyboard glyph and an identity label: `KB1` means Hana; `KB2` means Chocofi.
- Display battery state only for connected halves belonging to the active keyboard. Do not show the inactive keyboard's battery levels.
- A missing active half must not be represented as a valid battery reading. Hide that half's reading until it reconnects or reports a valid level.
- Identity and battery widgets must update after a completed switch and after active-half connection or battery-state changes.

## Build and hardware acceptance

Do not label a build as a flash candidate until all of the following are demonstrated:

1. The central firmware compiles with the board-specific base-layer selection integrated into the live routing path.
2. The inactive keyboard can request activation without first becoming the active input source.
3. A switch releases held keyboard and mouse usages before the new board is admitted.
4. HBASE/CBASE changes correctly while shared layers and the common combo catalogue remain available on both boards.
5. Common combos map to the correct physical positions on both boards and do not leak pending state across switches.
6. The OLED identity label follows the active board.
7. OLED battery readings include only the active keyboard's connected halves.
8. Automated routing tests and the complete firmware build pass.

The existing policy-model tests and identity diagnostic builds alone do not satisfy these hardware acceptance criteria. Do not flash a diagnostic/reference artifact as universal firmware.
