# Contributing

Thanks for your interest in contributing!

## Development setup
- Linux with ZNC 1.11.x and OpenSSL 3.x
- Packages: `znc-dev libssl-dev libicu-dev pkg-config cmake g++ make`
- Build: `znc-buildmod fish.cpp`

## Branching & PRs
- Use feature branches (`feature/...`) or fix branches (`fix/...`)
- Keep PRs focused and small when possible
- Update README/CHANGELOG as needed
- Ensure ECB/CBC SelfTest succeed and interop is verified

## Code style
- Prefer clear, readable C++
- Avoid deep nesting; handle errors early
- Keep changes minimal outside the scope of the PR

## Commit messages
- Prefix with feat/fix/docs/ci/refactor/chore
- Use imperative mood (e.g., "fix: handle ECB padding")

## Security
- Do not include secrets (keys, tokens)
- Report vulnerabilities via SECURITY.md
