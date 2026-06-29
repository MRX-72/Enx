# Enx — Encryption Xcelerate

Caesar-shift cipher CLI written in pure C++.  
Shifts every byte (raw mode) or every letter A–Z/a–z (alpha mode) by a user-supplied integer key.

---

## Build

```bash
git clone https://github.com/MRX-72/Enx
cd Enx
bash setup.sh
```

> Requires `g++` (GCC 9+) or `clang++` (LLVM 10+).  
> On Windows, run `setup.sh` under Git Bash / WSL, or compile manually:

```powershell
g++ EncDec.cpp -O2 -std=c++17 -o Enx.exe
```

---

## Syntax

```
Enx --str "<text>" --keyN <int> +E|+D [--alpha] [--hex] [--quiet]
```

| Flag | Type | Description |
|------|------|-------------|
| `--str "<text>"` | string | Input text. Omit to read from stdin. |
| `--keyN <int>` | integer | Shift key (positive or negative). |
| `+E` | flag | Encrypt (default). |
| `+D` | flag | Decrypt. |
| `--alpha` | flag | Alpha-wrap mode: only A–Z / a–z shifted, wraps within alphabet. |
| `--hex` | flag | Print hex bytes of output alongside text. |
| `--quiet` | flag | Output result only — no banner, no labels. Useful for scripting. |
| `--version` | flag | Print version string. |
| `--help` | flag | Print usage. |

---

## Examples

### Basic encrypt / decrypt (raw shift)

```bash
# Encrypt
Enx --str "Hello World" --keyN 7 +E

# Decrypt
Enx --str "Olssv'^vrld" --keyN 7 +D
```

### Alpha-wrap mode (ROT-N, printable-safe)

```bash
# Encrypt — only letters shift, spaces/punctuation unchanged
Enx --str "Hello, World!" --keyN 13 +E --alpha
# Output: Uryyb, Jbeyq!

# Decrypt
Enx --str "Uryyb, Jbeyq!" --keyN 13 +D --alpha
# Output: Hello, World!
```

### Hex output

```bash
Enx --str "ABC" --keyN 1 +E --hex
# Output text : BCD
# Hex         : 42 43 44
```

### Negative key

```bash
# Shift backwards — equivalent to decrypting with positive key
Enx --str "Hello" --keyN -3 +E
```

### Stdin / pipe

```bash
echo "Secret Message" | Enx --keyN 5 +E

cat ciphertext.txt | Enx --keyN 5 +D --quiet > plaintext.txt
```

### Quiet mode (scripting)

```bash
CIPHER=$(Enx --str "payload" --keyN 9 +E --quiet)
echo "Encrypted: $CIPHER"
```

### Chaining encrypt then decrypt

```bash
Enx --str "Test" --keyN 42 +E --quiet | Enx --keyN 42 +D --quiet
# Output: Test
```

---

## Modes at a glance

```
Raw shift  (default)   — every byte += keyN
                         non-printable bytes possible at large keys

Alpha wrap (--alpha)   — only [A-Za-z] shifted
                         wraps within a-z / A-Z boundary
                         safe key: any integer (mod 26 applied internally)
```

---

## Key behaviour

```
keyN =  0    →  output == input  (warning shown)
keyN =  13   →  ROT13 when used with --alpha
keyN = -N +D →  same as keyN = +N +E
```

---

## Exit codes

| Code | Meaning |
|------|---------|
| `0` | Success |
| `1` | Bad arguments or missing input |

---

## License

MIT — see [LICENSE](LICENSE).
