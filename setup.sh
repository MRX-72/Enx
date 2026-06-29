#!/usr/bin/env bash
# ──────────────────────────────────────────────────────────────
#  Enx setup script — builds and installs the Enx binary
#  Supported: Debian/Ubuntu/Arch/Fedora/RHEL, macOS (Homebrew)
# ──────────────────────────────────────────────────────────────
set -euo pipefail

# ── helpers ───────────────────────────────────────────────────
RED='\033[91m'; GREEN='\033[92m'; YELLOW='\033[93m'
CYAN='\033[96m'; GRAY='\033[90m'; BOLD='\033[1m'; RESET='\033[0m'

info()    { printf "${CYAN}${BOLD}[*]${RESET} %s\n" "$*"; }
ok()      { printf "${GREEN}${BOLD}[+]${RESET} %s\n" "$*"; }
warn()    { printf "${YELLOW}${BOLD}[~]${RESET} %s\n" "$*"; }
die()     { printf "${RED}${BOLD}[!]${RESET} %s\n" "$*" >&2; exit 1; }

SOURCE="EncDec.cpp"
BINARY="Enx"
CXX_FLAGS="-O2 -std=c++17 -Wall -Wextra"

# ── detect source ─────────────────────────────────────────────
[[ -f "$SOURCE" ]] || die "Source file '$SOURCE' not found. Run this script from the Enx directory."

# ── detect compiler ───────────────────────────────────────────
if command -v g++ &>/dev/null; then
    CXX="g++"
elif command -v clang++ &>/dev/null; then
    CXX="clang++"
else
    info "No C++ compiler found. Attempting to install g++ ..."
    if command -v apt-get &>/dev/null; then
        sudo apt-get install -y g++
    elif command -v pacman &>/dev/null; then
        sudo pacman -S --noconfirm gcc
    elif command -v dnf &>/dev/null; then
        sudo dnf install -y gcc-c++
    elif command -v brew &>/dev/null; then
        brew install gcc
    else
        die "Cannot auto-install a compiler. Install g++ or clang++ manually and re-run."
    fi
    CXX="g++"
fi

ok "Compiler : $CXX $(${CXX} --version | head -1)"

# ── compile ───────────────────────────────────────────────────
info "Compiling $SOURCE ..."
$CXX $CXX_FLAGS "$SOURCE" -o "$BINARY" \
    || die "Compilation failed. Check the output above."

ok "Binary   : ./$BINARY  ($(du -sh "$BINARY" | cut -f1))"

# ── install ───────────────────────────────────────────────────
# Prefer user-local paths (no sudo) first, fall back to /usr/local/bin
INSTALL_DIRS=("$HOME/.local/bin" "/usr/local/bin" "/usr/bin")
INSTALLED_TO=""

for dir in "${INSTALL_DIRS[@]}"; do
    if [[ -d "$dir" && -w "$dir" ]]; then
        cp "$BINARY" "$dir/$BINARY"
        INSTALLED_TO="$dir"
        break
    fi
done

if [[ -z "$INSTALLED_TO" ]]; then
    # Try with sudo for system paths
    for dir in "/usr/local/bin" "/usr/bin"; do
        if sudo cp "$BINARY" "$dir/$BINARY" 2>/dev/null; then
            INSTALLED_TO="$dir"
            break
        fi
    done
fi

[[ -n "$INSTALLED_TO" ]] || die "Could not install to any PATH directory. Copy '$BINARY' manually."

# ── verify PATH ───────────────────────────────────────────────
if ! echo "$PATH" | tr ':' '\n' | grep -qx "$INSTALLED_TO"; then
    warn "  $INSTALLED_TO is not in your \$PATH."
    warn "  Add this to ~/.bashrc or ~/.zshrc:"
    printf "      ${YELLOW}export PATH=\"\$PATH:$INSTALLED_TO\"${RESET}\n"
fi

ok "Installed : $INSTALLED_TO/$BINARY"

# ── smoke test ────────────────────────────────────────────────
info "Running smoke test ..."
RESULT=$("$INSTALLED_TO/$BINARY" --str "Test" --keyN 13 +E --quiet 2>/dev/null)
ROUND=$("$INSTALLED_TO/$BINARY" --str "$RESULT" --keyN 13 +D --quiet 2>/dev/null)

if [[ "$ROUND" == "Test" ]]; then
    ok "Smoke test passed  (encrypt -> decrypt -> 'Test')"
else
    warn "Smoke test mismatch: expected 'Test', got '$ROUND'"
fi

# ── done ──────────────────────────────────────────────────────
printf "\n${BOLD}${GREEN}Setup complete.${RESET}\n\n"
printf "  ${GRAY}Usage:${RESET}  Enx --str \"Hello\" --keyN 7 +E\n"
printf "  ${GRAY}Help: ${RESET}  Enx --help\n\n"
