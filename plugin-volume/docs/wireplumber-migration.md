# WirePlumber modernization notes

This plugin now assumes PipeWire/WirePlumber as the primary backend and only falls back to PulseAudio when PipeWire is missing. The goal is to keep live control of sinks/sources while persisting enable/disable policy through WirePlumber rules.

## Runtime layer
- Keep using the PipeWire backend for live volume/mute and routing; replace remaining PulseAudio calls with WirePlumber/libpipewire as you touch code.
- Expose sinks/sources and streams as Qt models; prefer the `media.class` values that PipeWire/WirePlumber publish instead of PulseAudio naming.

## Persistent policy layer
- Device/node enable/disable is stored as SPA-JSON in `~/.config/wireplumber/wireplumber.conf.d/60-1g4-panel-volume.conf`.
- Rules set `device.disabled` or `node.disabled` and are applied after `systemctl --user restart wireplumber` (or a configurable restart command for non-systemd setups).
- Use `WirePlumberPolicy` (plugin-volume/wireplumberpolicy.{h,cpp}) to load, mutate, and write the SPA-JSON plus restart WirePlumber.

Example usage inside a configuration dialog:

```cpp
WirePlumberPolicy policy;
policy.load();
policy.setDeviceDisabled("alsa_card.pci-0000_00_1b.0", true);
policy.setNodeDisabled("alsa_output.pci-0000_00_1b.0.analog-stereo", true);
policy.write();
policy.restart(); // optional; show UI hint before calling
```

## UI/UX direction
- Add a “Configuration” tab that lists devices/nodes with checkboxes (“Disable until restart”) backed by `WirePlumberPolicy`.
- Show a banner that a restart is required when policy changes are pending, and trigger a rescan when the backend reconnects.
- Keep live controls (volume/mute/move-stream/default-device) immediate and avoid persisting them to policy.

## Follow-ups
- Replace `pactl` calls in `PipeWireEngine` with WirePlumber metadata updates.
- Swap 1g4-mixer’s libpulse dependency with a WirePlumber-driven view model using the same widgets.
- Add tests for SPA-JSON generation to guard the policy file format.
