
# Kickoff Freeze Plugin (BakkesMod)

Adds a **kickoff-style countdown** to **Custom Training** shots, freezing your car for the countdown while still letting you **steer** and **powerslide**. Designed to pair nicely with **Musty Speedflip** training packs and the **Speedflip Trainer** plugin.

- Locks: **Throttle, Boost, Jump**
- Allows: **Steering (Yaw/Pitch/Roll)** and **Powerslide (Handbrake)**
- Auto-starts the countdown whenever a **training shot resets/loads** (no manual bind needed)
- Simple on-screen HUD: **3…2…1… GO!**

> Target: **Windows / Rocket League / BakkesMod / Visual Studio 2022 (Release | Win32)**

---

## Quick Start (Recommended: Visual Studio 2022)

1. **Fork the official template** (or clone it locally):
   - https://github.com/bakkesmodorg/bakkesmod-plugin-template

2. **Copy this repo’s `src/` files** into the template’s `src/` folder.
   - Overwrite the template’s default stubs.

3. Open the solution in **Visual Studio 2022**:
   - Set configuration to **Release** and platform to **Win32**.
   - Build the solution. The DLL will be copied to your BakkesMod `plugins` directory if your template is configured that way; otherwise copy it manually.

4. Launch Rocket League + BakkesMod, then load the plugin:
   - BakkesMod → **F2** → **Plugins** → **Loaded Plugins** → select your built DLL.
   - Go to **Custom Training** and reset a shot — you should see a **3–2–1–GO** overlay and input lock until GO.

---

## Cvars (Console)

- `kf_countdown_secs` (float, default **3.0**): length of the countdown.
- `kf_autostart_on_reset` (bool, default **1**): auto-start countdown on Custom Training **reset/load**.

Examples:
```
kf_countdown_secs 2.5
kf_autostart_on_reset 1
```

---

## Behavior Details

During countdown:
- Car linear and angular velocities are forced to **0** each frame.
- **Throttle, Boost, Jump are blocked.**
- **Powerslide/Handbrake is allowed.**
- Steering inputs are read (so you can hold your angle), but the car won’t rotate because angular velocity is locked.

When `timeLeft <= 0`, the plugin releases control on the next frame.

---

## Source Files

- `src/KickoffFreezePlugin.cpp` — the plugin implementation.
- `src/plugin.cpp` — a stub used by the template; the registration macro is in `KickoffFreezePlugin.cpp`.

> The project uses the template’s standard headers: `bakkesmod/plugin/bakkesmodplugin.h`, `wrappers/...`

---

## Troubleshooting

- **Not building x86 (Win32):** make sure the platform is **Win32**, not x64.
- **Plugin not loading:** verify the DLL ended up in `.../BakkesMod/plugins/`. In the BakkesMod console, run `plugin load "YourPluginName.dll"` to see errors.
- **No countdown on reset:** ensure you’re in **Custom Training**. Some packs may trigger `StartRound` vs `ResetRound`; this plugin hooks multiple training events to cover both.
- **Speedflip Trainer:** this plugin doesn’t conflict; it only modifies your inputs during the countdown window.

---

## License

MIT — see `LICENSE`.
