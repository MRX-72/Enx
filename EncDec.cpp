// ============================================================
//  Enx - Encryption Xcelerate  |  Caesar-shift cipher CLI
//  Core concept: character-shift based encryption/decryption
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

// ──────────────────────────────────────────────
//  ANSI color / style helpers (256-color aware)
// ──────────────────────────────────────────────
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"
#define ITALIC      "\033[3m"

// Standard colors
#define CYAN        "\033[96m"   // bright cyan
#define GREEN       "\033[92m"   // bright green
#define YELLOW      "\033[93m"   // bright yellow
#define BLUE        "\033[94m"   // bright blue
#define MAGENTA     "\033[95m"   // bright magenta
#define RED         "\033[91m"   // bright red
#define WHITE       "\033[97m"
#define GRAY        "\033[90m"

// Accent (256-color foreground)
#define ORANGE      "\033[38;5;214m"
#define TEAL        "\033[38;5;43m"
#define VIOLET      "\033[38;5;141m"
#define LIME        "\033[38;5;154m"

// Background highlights
#define BG_DARK     "\033[48;5;234m"

static const char* VERSION = "2.0.0";

// ──────────────────────────────────────────────
//  Platform: enable ANSI on Windows
// ──────────────────────────────────────────────
void enableAnsi() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

// ──────────────────────────────────────────────
//  Utilities
// ──────────────────────────────────────────────
static std::string repeat(const std::string& s, int n) {
    std::string r;
    r.reserve(s.size() * n);
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

static std::string toHex(const std::string& s) {
    std::ostringstream oss;
    for (unsigned char c : s)
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)c << ' ';
    std::string h = oss.str();
    if (!h.empty() && h.back() == ' ') h.pop_back();
    return h;
}

static std::string escapeUnprintable(const std::string& s) {
    std::string out;
    out.reserve(s.size() * 2);
    for (unsigned char c : s) {
        if (c >= 32 && c < 127) out += c;
        else { out += '['; out += std::to_string((int)c); out += ']'; }
    }
    return out;
}

// ──────────────────────────────────────────────
//  Banner
// ──────────────────────────────────────────────
void printBanner() {
    std::cout
        << "\n" << CYAN << BOLD
        << "  ███████╗███╗   ██╗██╗  ██╗\n"
        << "  ██╔════╝████╗  ██║╚██╗██╔╝\n"
        << "  █████╗  ██╔██╗ ██║ ╚███╔╝ \n"
        << "  ██╔══╝  ██║╚██╗██║ ██╔██╗ \n"
        << "  ███████╗██║ ╚████║██╔╝ ██╗\n"
        << "  ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝\n"
        << RESET
        << GRAY << ITALIC
        << "        Encryption  Xcelerate  " << TEAL << "v" << VERSION << RESET << "\n"
        << GRAY << repeat("─", 44) << RESET << "\n";
}

// ──────────────────────────────────────────────
//  Usage
// ──────────────────────────────────────────────
void printUsage(const char* prog) {
    printBanner();
    std::cout
        << "\n" << BOLD << WHITE << "  USAGE\n" << RESET
        << GRAY << "  " << repeat("─", 40) << "\n" << RESET

        << "\n" << YELLOW << BOLD << "  Encrypt\n" << RESET
        << "    " << LIME << prog << RESET
        << " --str " << CYAN << "\"text\"" << RESET
        << " --keyN " << ORANGE << "<int>" << RESET
        << " +E" << " " << GRAY << "[--alpha] [--hex] [--quiet]\n" << RESET

        << "\n" << VIOLET << BOLD << "  Decrypt\n" << RESET
        << "    " << LIME << prog << RESET
        << " --str " << CYAN << "\"text\"" << RESET
        << " --keyN " << ORANGE << "<int>" << RESET
        << " +D" << " " << GRAY << "[--alpha] [--hex] [--quiet]\n" << RESET

        << "\n" << BOLD << WHITE << "  STDIN MODE\n" << RESET
        << "    " << GRAY << "echo \"hello\" | " << LIME << prog << RESET
        << " --keyN " << ORANGE << "3" << RESET << " +E\n"

        << "\n" << BOLD << WHITE << "  OPTIONS\n" << RESET
        << GRAY << "  " << repeat("─", 40) << "\n" << RESET
        << "  " << GREEN  << "--str   \"<text>\"" << RESET << GRAY << "   Input text (or omit to read stdin)\n" << RESET
        << "  " << GREEN  << "--keyN  <int>  " << RESET << GRAY << "   Shift key (non-zero integer)\n" << RESET
        << "  " << GREEN  << "+E             " << RESET << GRAY << "   Encrypt mode  (default)\n" << RESET
        << "  " << GREEN  << "+D             " << RESET << GRAY << "   Decrypt mode\n" << RESET
        << "  " << GREEN  << "--alpha        " << RESET << GRAY << "   Alphabet-only mode: wraps A-Z/a-z, leaves others\n" << RESET
        << "  " << GREEN  << "--hex          " << RESET << GRAY << "   Show hex representation of output\n" << RESET
        << "  " << GREEN  << "--quiet        " << RESET << GRAY << "   Print only the result (no banner/labels)\n" << RESET
        << "  " << GREEN  << "--version      " << RESET << GRAY << "   Show version and exit\n" << RESET
        << "  " << GREEN  << "--help         " << RESET << GRAY << "   Show this help\n" << RESET

        << "\n" << BOLD << WHITE << "  EXAMPLES\n" << RESET
        << GRAY << "  " << repeat("─", 40) << "\n" << RESET
        << "  " << LIME << prog << RESET << " --str " << CYAN << "\"Hello World\"" << RESET << " --keyN " << ORANGE << "13" << RESET << " +E\n"
        << "  " << LIME << prog << RESET << " --str " << CYAN << "\"Uryyb Jbeyq\"" << RESET << " --keyN " << ORANGE << "13" << RESET << " +D --alpha\n"
        << "  echo " << CYAN << "\"secret\"" << RESET << " | " << LIME << prog << RESET << " --keyN " << ORANGE << "7" << RESET << " +E --hex\n"
        << "\n";
}

// ──────────────────────────────────────────────
//  Cipher engines
// ──────────────────────────────────────────────

// Raw shift: shift every byte by keyN (original behaviour)
std::string rawShift(const std::string& input, int keyN) {
    std::string out = input;
    for (char& c : out) c = static_cast<char>(c + keyN);
    return out;
}

// Alpha-wrap Caesar: only A-Z / a-z are shifted, wraps within alphabet
std::string alphaShift(const std::string& input, int keyN) {
    std::string out = input;
    for (char& c : out) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>('A' + ((c - 'A' + keyN % 26 + 26) % 26));
        } else if (c >= 'a' && c <= 'z') {
            c = static_cast<char>('a' + ((c - 'a' + keyN % 26 + 26) % 26));
        }
        // digits, spaces, punctuation left as-is
    }
    return out;
}

// ──────────────────────────────────────────────
//  Main
// ──────────────────────────────────────────────
int main(int argc, char* argv[]) {
    enableAnsi();

    // ── no args → help ─────────────────────
    if (argc < 2) { printUsage(argv[0]); return 0; }

    // ── parse arguments ────────────────────
    std::string str;
    int  keyN      = 0;
    bool encrypt   = true;
    bool alphaMode = false;
    bool hexOutput = false;
    bool quiet     = false;
    bool keySet    = false;
    bool strSet    = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "--version" || arg == "-v") {
            std::cout << CYAN << BOLD << "Enx" << RESET
                      << " Encryption Xcelerate  version " << TEAL << VERSION << RESET << "\n";
            return 0;
        }
        else if (arg == "--str") {
            if (i + 1 >= argc) {
                std::cerr << RED << BOLD << "  [!] --str requires an argument.\n" << RESET;
                return 1;
            }
            str = argv[++i];
            strSet = true;
        }
        else if (arg == "--keyN") {
            if (i + 1 >= argc) {
                std::cerr << RED << BOLD << "  [!] --keyN requires an integer argument.\n" << RESET;
                return 1;
            }
            try {
                keyN = std::stoi(argv[++i]);
                keySet = true;
            } catch (const std::exception&) {
                std::cerr << RED << BOLD << "  [!] --keyN value must be a valid integer.\n" << RESET;
                return 1;
            }
        }
        else if (arg == "+E") { encrypt = true;  }
        else if (arg == "+D") { encrypt = false; }
        else if (arg == "--alpha")   { alphaMode = true; }
        else if (arg == "--hex")     { hexOutput = true; }
        else if (arg == "--quiet")   { quiet = true; }
        else {
            std::cerr << YELLOW << BOLD << "  [?] Unknown argument: " << RESET << arg
                      << GRAY << "  (use --help for usage)\n" << RESET;
        }
    }

    // ── stdin fallback if --str not given ──
    if (!strSet) {
        bool isatty_stdin = true;
#ifdef _WIN32
        isatty_stdin = (GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_CHAR);
#endif
        if (!isatty_stdin) {
            std::ostringstream oss;
            oss << std::cin.rdbuf();
            str = oss.str();
            // strip trailing newline added by shells
            if (!str.empty() && str.back() == '\n') str.pop_back();
            if (!str.empty() && str.back() == '\r') str.pop_back();
            strSet = !str.empty();
        }
    }

    if (!strSet || str.empty()) {
        std::cerr << RED << BOLD << "  [!] No input string provided."
                  << RESET << GRAY << "  Use --str \"text\" or pipe via stdin.\n" << RESET;
        return 1;
    }

    if (!keySet) {
        std::cerr << RED << BOLD << "  [!] No key provided."
                  << RESET << GRAY << "  Use --keyN <int>.\n" << RESET;
        return 1;
    }

    // ── warnings ───────────────────────────
    if (!quiet) {
        if (keyN == 0)
            std::cerr << YELLOW << "  [~] Warning: key is 0 — output will be identical to input.\n" << RESET;
        if (!encrypt && alphaMode && keyN % 26 == 0)
            std::cerr << YELLOW << "  [~] Warning: key is a multiple of 26 in alpha mode — output identical.\n" << RESET;
    }

    // ── apply cipher ───────────────────────
    int effectiveKey = encrypt ? keyN : -keyN;
    std::string result = alphaMode ? alphaShift(str, effectiveKey)
                                   : rawShift(str, effectiveKey);

    // ── quiet mode: raw output only ────────
    if (quiet) {
        std::cout << result << "\n";
        return 0;
    }

    // ── rich output ────────────────────────
    printBanner();

    const char* modeColor  = encrypt ? YELLOW : VIOLET;
    const char* modeLabel  = encrypt ? "ENCRYPT" : "DECRYPT";
    const char* modeSymbol = encrypt ? "🔒" : "🔓";
    const std::string sep  = GRAY + repeat("─", 44) + std::string(RESET);

    std::cout << "\n"
              << modeColor << BOLD << "  " << modeSymbol << "  " << modeLabel
              << (alphaMode ? "  [alpha-wrap]" : "  [raw-shift]")
              << RESET << "\n"
              << sep << "\n";

    // Input
    std::cout << CYAN  << "  Input   " << RESET
              << GRAY  << "│ " << RESET
              << WHITE << escapeUnprintable(str) << RESET
              << GRAY  << "  (" << str.size() << " byte" << (str.size() != 1 ? "s" : "") << ")\n" << RESET;

    // Key
    std::cout << ORANGE << "  Key     " << RESET
              << GRAY   << "│ " << RESET
              << BOLD   << keyN << RESET
              << GRAY   << DIM << (encrypt ? "  (+shift)" : "  (-shift)") << RESET << "\n";

    // Output
    std::cout << GREEN << "  Output  " << RESET
              << GRAY  << "│ " << RESET
              << BOLD << WHITE << escapeUnprintable(result) << RESET
              << GRAY  << "  (" << result.size() << " byte" << (result.size() != 1 ? "s" : "") << ")\n" << RESET;

    // Hex line (conditional)
    if (hexOutput) {
        std::cout << TEAL << "  Hex     " << RESET
                  << GRAY << "│ " << RESET
                  << DIM  << toHex(result) << RESET << "\n";
    }

    // Mode indicator
    std::cout << sep << "\n";
    if (alphaMode) {
        std::cout << GRAY << DIM
                  << "  Alpha mode: only A–Z / a–z shifted, wraps within alphabet.\n"
                  << RESET;
    } else {
        std::cout << GRAY << DIM
                  << "  Raw mode: every byte shifted by " << keyN
                  << ". Use --alpha for printable-safe output.\n"
                  << RESET;
    }
    std::cout << "\n";

    return 0;
}
