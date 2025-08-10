# Commands Reference

Keys
- SetKey <#chan|Nick> [CBC:|ECB:]<Key>
- DelKey <#chan|Nick>
- ShowKey <#chan|Nick>
- ListKeys [full]
- SetKeyFrom <target> <source>
- SetMode <target> ecb|cbc
- GetMode <target>

Exchange
- KeyX <Nick> [ecb|cbc]
- KeyXChan <#chan> <Nick> [ecb|cbc]
- KeyXChanAll <#chan> [ecb|cbc]
- AutoKeyX on|off

Processing & types
- ProcessIncoming on|off
- ProcessOutgoing on|off
- EncryptNotice on|off
- EncryptAction on|off

Topics
- EncryptTopic <#chan> on|off|status
- EncryptGlobalTopic on|off|status

Marking (local)
- MarkIncoming on|off
- MarkIncomingTarget <#chan|Nick> on|off
- MarkPos prefix|suffix
- MarkStr dec|enc|plain <text>
- MarkBroken on|off

Plaintext
- PlainPrefix <prefix|off>

Diagnostics
- SelfTest <ecb|cbc> <key> <text>
