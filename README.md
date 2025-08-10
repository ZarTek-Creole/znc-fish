# ZNC FiSH module (ECB/CBC, DH1080) — ZNC 1.11.x + OpenSSL 3

[![build](https://github.com/ZarTek-Creole/znc-fish/workflows/build/badge.svg)](https://github.com/ZarTek-Creole/znc-fish/actions/workflows/build.yml)
[![C++ Quality Checks and Formatting](https://github.com/ZarTek-Creole/znc-fish/workflows/C++%20Quality%20Checks%20and%20Formatting/badge.svg)](https://github.com/ZarTek-Creole/znc-fish/actions/workflows/Quality_Checks_and_Formatting.yml)
[![Test ZNC Module](https://github.com/ZarTek-Creole/znc-fish/workflows/Test%20ZNC%20Module/badge.svg)](https://github.com/ZarTek-Creole/znc-fish/actions/workflows/test-znc-module.yml)

FiSH-compatible encryption module for ZNC:
- Blowfish ECB (FiSH) and CBC (Mircryption-compatible)
- DH1080 key exchange (FiSH10 trailing 'A' compatibility)
- Per-target keys, auto mode learning (ECB/CBC)
- Incoming/outgoing processing toggles, notices/actions, topics
- Rich commands with readable tables and help

See also: [Commands](docs/COMMANDS.md), [Troubleshooting](docs/TROUBLESHOOTING.md), [CHANGELOG](CHANGELOG.md)

## Requirements
- ZNC 1.11.x
- OpenSSL 3.x (legacy algorithms available; deprecation warnings silenced with GCC pragmas)
- GCC ≥ 12

## Build and install
```bash
znc-buildmod fish.cpp
install -D -m 0755 fish.so ~/.znc/modules/fish.so
# Or system-wide
# install -D -m 0755 fish.so /usr/local/lib/znc/fish.so
```
Restart ZNC after replacing the module to unload any previous version.

## Quick start
- Set a key (CBC default): `/msg *fish SetKey #chan CBC:mySecret`
- ECB for legacy clients: `/msg *fish SetKey nick ECB:legacyKey`
- List keys: `/msg *fish ListKeys` (append `full` to show full keys)
- Show a key: `/msg *fish ShowKey <#chan|nick>`
- Disable encryption for a target: `/msg *fish DisableTarget <#chan|nick> on|off`
- Topics: `/msg *fish EncryptTopic <#chan> on|off|status`, global default: `/msg *fish EncryptGlobalTopic on|off|status`
- Plain text escape: start message with `` or configure `/msg *fish PlainPrefix <prefix>`

## DH1080 key exchange
- With a nick: `/msg *fish KeyX <nick> [ecb|cbc]`
- With a nick but store under channel: `/msg *fish KeyXChan <#chan> <nick> [ecb|cbc]`
- Broadcast to all nicks in a channel (per-nick keys): `/msg *fish KeyXChanAll <#chan> [ecb|cbc]`
- Auto on first PM when no key: `/msg *fish AutoKeyX on|off`

The module accepts `DH1080_INIT` and `DH1080_INIT_CBC`, and sends/accepts FiSH10 public keys with trailing `A`.

## Commands
- Keys:
  - `SetKey <#chan|Nick> [CBC:|ECB:]<Key>`
  - `DelKey <#chan|Nick>`
  - `ShowKey <#chan|Nick>`
  - `ListKeys [full]`
  - `SetKeyFrom <target> <source>`
  - `SetMode <target> ecb|cbc`, `GetMode <target>`
- Exchange:
  - `KeyX <Nick> [ecb|cbc]`, `KeyXChan <#chan> <Nick> [ecb|cbc]`, `KeyXChanAll <#chan> [ecb|cbc]`
  - `AutoKeyX on|off`
- Processing & message types:
  - `ProcessIncoming on|off`, `ProcessOutgoing on|off`
  - `EncryptNotice on|off`, `EncryptAction on|off`
- Topics:
  - `EncryptTopic <#chan> on|off|status`, `EncryptGlobalTopic on|off|status`
- Marking (local only):
  - `MarkIncoming on|off`, `MarkIncomingTarget <target> on|off`
  - `MarkPos prefix|suffix`
  - `MarkStr dec|enc|plain <text>`
  - `MarkBroken on|off` (append `&` for truncated FiSH blocks)
- Plaintext helper:
  - `PlainPrefix <prefix|off>`; also supports backtick escape ``
- Diagnostics:
  - `SelfTest <ecb|cbc> <key> <text>` — encrypt→decrypt roundtrip

## Message detection and compatibility
- CBC: `+OK *<b64>` (IV + plaintext then encrypted, standard MIME base64)
- ECB: `+OK <fish64>` or `mcps <fish64>` (FiSH base64 `./0-9a-zA-Z` in 12-char blocks)
- Auto fallback: on incoming messages, the module tries the expected mode first, then the other mode.
- Auto mode learn: on successful decrypt, stored mode can be updated unless disabled.

## Notes
- Keys are stored in ZNC NV as `CBC:<key>` or `ECB:<key>` (default CBC when no prefix).
- The ECB implementation matches FiSH behavior (raw key via `BF_set_key(strlen(key), key)` and FiSH base64 packing).
- OpenSSL 3 deprecation warnings are silenced in-code using GCC pragmas.

## Testing
- Local roundtrip: `/msg *fish SelfTest ecb MyKey hello` and `/msg *fish SelfTest cbc MyKey hello`
- Interop: test with FiSH/irssi or mIRC FiSH10:
  - PM: each side `KeyX <nick>`, send a short message like `!df`
  - Channel: set the same CBC key for all clients, verify `+OK *...` decrypts consistently
