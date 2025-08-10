# Troubleshooting

## "+OK ..." not decrypted
- Ensure a key is set for the target (`ListKeys`)
- Try `SetMode <target> cbc` (most clients use CBC)
- For FiSH/legacy, try `SetMode <target> ecb`

## Garbled text in CBC SelfTest
- CBC uses IV+plaintext; ensure Base64 decode works (`SelfTest cbc <key> <text>`)
- Keys must be identical on both sides; check no stray spaces

## ECB truncated output
- FiSH uses 12-char blocks; messages not multiple of 12 are trimmed
- Enable `MarkBroken on` to visibly mark broken blocks with `&`

## Build fails on CI
- Ensure `znc-dev`, `libssl-dev`, and `libicu-dev` are installed
- Ubuntu runners provide OpenSSL 3.x
