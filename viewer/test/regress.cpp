// gaviewer_regress -- headless regression harness for GAViewer.
//
// Loads .g source files, executes a console command (e.g. "FIG_2_2();") exactly
// as if it was typed into the GAViewer console, and dumps the resulting state:
// the console output and every object (multivector coordinates plus their
// geometric interpretation). Each time the script suspends ('suspend;'), the
// state is dumped as a new frame and execution is resumed, until the script
// completes.
//
// The dump can be compared against a "golden" reference file. Numbers are
// compared with a tolerance, everything else must match exactly. This makes
// the harness usable as a safety net while replacing the parser (output must
// be identical) and the algebra implementation (numbers may differ slightly).
//
// Usage:
//   gaviewer_regress [-I <dir>]... [-l <geo>] [-s <geo>] [-o <actual>] [-g <golden> [-u]] [-t <tol>] <command>
//
//   -I <dir>     load all .g files in <dir> (sorted by name); may be repeated
//   -o <file>    write the dump to <file> (default: stdout, unless -g is given)
//   -g <file>    compare the dump with golden reference <file>; exit code 1 on mismatch
//   -u           (with -g) write/overwrite the golden reference instead of comparing
//   -t <tol>     numeric tolerance, relative to magnitude (default 1e-6)
//   -f <n>       maximum number of frames (default 100)
//   -l <file>    open the GA file <file> (e.g. a .geo file) before the command, as File > Open
//                of the GUI does (the command may then be omitted)
//   -s <file>    save the state to the .geo file <file> after the command, as File > Save
//                of the GUI does (without the parts that need the GUI: title, camera, ...)

// <windows.h> (pulled in by the OpenGL headers) must not define min/max macros:
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#include "console/console.h"
#include "console/consolescope.h"
#include "gafile.h"
#include "gaincludes.h"
#include "mvint.h"
#include "object.h"
#include "state.h"

namespace fs = std::filesystem;

namespace
{

// ---------------------------------------------------------------------------
// console output capture
// ---------------------------------------------------------------------------

std::string g_log;

int captureCprintf(const char *fmt, va_list ap)
{
    va_list ap2;
    va_copy(ap2, ap);
    int n = std::vsnprintf(nullptr, 0, fmt, ap2);
    va_end(ap2);
    if (n <= 0)
        return n;
    std::string buf(static_cast<size_t>(n) + 1, '\0');
    std::vsnprintf(buf.data(), buf.size(), fmt, ap);
    buf.resize(static_cast<size_t>(n));
    g_log += buf;
    return n;
}

void replaceAll(std::string &s, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;
    for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
        s.replace(pos, from.size(), to);
}

// Paths that must not leak into the dump (they differ between machines).
std::vector<std::pair<std::string, std::string>> g_pathSubstitutions;

std::string takeLog()
{
    std::string s;
    s.swap(g_log);
    replaceAll(s, "\r\n", "\n");
    for (const auto &[from, to] : g_pathSubstitutions)
        replaceAll(s, from, to);
    if (!s.empty() && s.back() != '\n')
        s += '\n';
    return s;
}

// ---------------------------------------------------------------------------
// state dump
// ---------------------------------------------------------------------------

std::string num(double x)
{
    if (std::fabs(x) < 1e-12)
        x = 0.0; // also maps -0 to 0
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.10g", x);
    return buf;
}

std::string hex(int x)
{
    char buf[16];
    std::snprintf(buf, sizeof(buf), "0x%x", static_cast<unsigned>(x));
    return buf;
}

// Garamon multivector: dumped in the display basis (the basis of the Gaigen algebra
// it replaces, see ga/traits.h), so that the dump is the same as with Gaigen.
template <ga::garamonMV MV> void dumpMv(std::ostringstream &out, const MV &mv)
{
    out << "  mv\n";
    const auto &basis = ga::algebraTraits<MV>::displayBasis;
    for (std::size_t i = 0; i < basis.size(); i++)
    {
        double c = ga::coordinate(mv, i);
        if (num(c) != "0")
            out << "    " << basis[i].name << " " << num(c) << "\n";
    }
}

void dumpInt(std::ostringstream &out, const mvInt &I)
{
    out << "  int type=" << hex(I.m_type) << " valid=" << I.m_valid << "\n";
    for (int i = 0; i < MVINT_NB_POINTS; i++)
        out << "    point" << i << " " << num(I.m_point[i][0]) << " " << num(I.m_point[i][1]) << " "
            << num(I.m_point[i][2]) << "\n";
    for (int i = 0; i < MVINT_NB_VECTORS; i++)
        out << "    vector" << i << " " << num(I.m_vector[i][0]) << " " << num(I.m_vector[i][1]) << " "
            << num(I.m_vector[i][2]) << "\n";
    out << "    scalar";
    for (int i = 0; i < MVINT_NB_SCALAR; i++)
        out << " " << num(I.m_scalar[i]);
    out << "\n";
}

const char *typeName(int t)
{
    switch (t)
    {
    case OT_TEXT:
        return "text";
    case OT_E3GA:
        return "e3ga";
    case OT_P3GA:
        return "p3ga";
    case OT_C3GA:
        return "c3ga";
    case OT_C5GA:
        return "c5ga";
    case OT_I2GA:
        return "i2ga";
    case OT_POLYGON:
        return "polygon";
    case OT_MESH:
        return "mesh";
    default:
        return "unknown";
    }
}

std::string dumpObjects()
{
    std::ostringstream out;
    for (const auto &[key, o] : g_state->m_objects)
    {
        const float *c = o->fgColor();
        out << "object " << o->m_name << " " << typeName(o->m_type) << " drawMode=" << hex(o->m_drawMode)
            << " color=" << num(c[0]) << " " << num(c[1]) << " " << num(c[2]) << " " << num(c[3]) << "\n";

        switch (o->m_type)
        {
        case OT_E3GA: {
            auto *eo = static_cast<e3gaObject *>(o);
            dumpMv(out, eo->m_mv);
            dumpInt(out, eo->m_int);
            break;
        }
        case OT_P3GA: {
            auto *po = static_cast<p3gaObject *>(o);
            dumpMv(out, po->m_mv);
            dumpInt(out, po->m_int);
            break;
        }
        case OT_C3GA: {
            auto *co = static_cast<c3gaObject *>(o);
            dumpMv(out, co->m_mv);
            dumpInt(out, co->m_int);
            break;
        }
        case OT_C5GA: {
            auto *co = static_cast<c5gaObject *>(o);
            dumpMv(out, co->m_mv);
            dumpInt(out, co->m_int);
            break;
        }
        case OT_I2GA: {
            auto *io = static_cast<i2gaObject *>(o);
            dumpMv(out, io->m_mv);
            dumpInt(out, io->m_int);
            break;
        }
        case OT_TEXT: {
            auto *to = static_cast<textObject *>(o);
            std::string text = to->m_text ? to->m_text : "";
            replaceAll(text, "\n", "\\n");
            out << "  text \"" << text << "\"\n";
            if (!to->m_pointName.empty())
                out << "  at " << to->m_pointName << "\n";
            else
                dumpMv(out, to->m_pos);
            break;
        }
        case OT_POLYGON: {
            // dynamic polygons refer to their vertices by name, static ones store coordinates
            auto *po = static_cast<polygonObject *>(o);
            for (int i = 0; i < po->m_nbVertices; i++)
            {
                out << "  vertex" << i;
                if (po->m_vertexName)
                    out << " " << po->m_vertexName[i];
                else if (po->m_vertex)
                    out << " " << num(po->m_vertex[3 * i]) << " " << num(po->m_vertex[3 * i + 1]) << " "
                        << num(po->m_vertex[3 * i + 2]);
                out << "\n";
            }
            break;
        }
        default:
            break;
        }
    }
    return out.str();
}

// ---------------------------------------------------------------------------
// tolerant comparison
// ---------------------------------------------------------------------------

bool isIdentChar(char ch)
{
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_' || ch == '^';
}

bool isDigit(char ch)
{
    return std::isdigit(static_cast<unsigned char>(ch)) != 0;
}

struct Token
{
    bool isNumber;
    double value;
    std::string text;
};

// Splits a line into numbers and non-number words (whitespace separates tokens).
// A number is only recognized when it does not continue an identifier, so
// 'e1', 'e1^e2' or 'FIG_2_2' are words, but '1.5' in '1.5*e1' is a number.
std::vector<Token> tokenize(const std::string &line)
{
    std::vector<Token> tokens;
    size_t i = 0, n = line.size();
    std::string word;
    auto flushWord = [&] {
        if (!word.empty())
            tokens.push_back({false, 0.0, word});
        word.clear();
    };
    while (i < n)
    {
        char ch = line[i];
        if (std::isspace(static_cast<unsigned char>(ch)))
        {
            flushWord();
            i++;
            continue;
        }
        bool prevIdent = (i > 0) && isIdentChar(line[i - 1]);
        bool signOrDot = (ch == '-' || ch == '+' || ch == '.');
        bool startsNumber =
            isDigit(ch) || (signOrDot && i + 1 < n &&
                            (isDigit(line[i + 1]) || (line[i + 1] == '.' && i + 2 < n && isDigit(line[i + 2]))));
        if (startsNumber && !prevIdent)
        {
            const char *begin = line.c_str() + i;
            char *end = nullptr;
            double v = std::strtod(begin, &end);
            if (end != begin)
            {
                flushWord();
                tokens.push_back({true, v, std::string(begin, static_cast<size_t>(end - begin))});
                i += static_cast<size_t>(end - begin);
                continue;
            }
        }
        word += ch;
        i++;
    }
    flushWord();
    return tokens;
}

// A printed multivector "<name> = <c>*<blade> + <c>*<blade> + ..." (or "<name> = 0"), as the
// console prints it: parsed into 'name' and a map basis blade -> coefficient ("1" for the scalar).
bool parseMvLine(const std::string &line, std::string &name, std::map<std::string, double> &terms)
{
    const std::string::size_type eq = line.find(" = ");
    if (eq == std::string::npos)
        return false;
    name = line.substr(0, eq);
    terms.clear();
    const std::string rhs = line.substr(eq + 3);
    if (rhs == "0")
        return true;
    std::string::size_type start = 0;
    for (;;)
    {
        const std::string::size_type plus = rhs.find(" + ", start);
        const std::string term = rhs.substr(start, (plus == std::string::npos) ? std::string::npos : plus - start);
        const char *begin = term.c_str();
        char *end = nullptr;
        const double c = std::strtod(begin, &end);
        if (end == begin)
            return false;
        std::string blade;
        if (*end == 0)
            blade = "1";
        else if (*end == '*')
            blade = end + 1;
        else
            return false;
        if (blade.empty() || (blade.find(' ') != std::string::npos))
            return false;
        terms[blade] += c;
        if (plus == std::string::npos)
            return true;
        start = plus + 3;
    }
}

bool linesMatch(const std::string &expected, const std::string &actual, double tol)
{
    if (expected == actual)
        return true;

    // Printed multivectors are compared coefficient by coefficient, a missing term being 0, with a
    // tolerance relative to the largest coefficient: a coordinate of rounding noise near the print
    // threshold (1e-14, below which a term is not printed) may then be printed on one side only.
    std::string ne, na;
    std::map<std::string, double> me, ma;
    if (parseMvLine(expected, ne, me) && parseMvLine(actual, na, ma) && (ne == na))
    {
        double scale = 1.0;
        for (const auto &t : me)
            scale = std::max(scale, std::fabs(t.second));
        for (const auto &t : ma)
            scale = std::max(scale, std::fabs(t.second));
        for (const auto *m : {&me, &ma})
            for (const auto &t : *m)
            {
                const double a = me.count(t.first) ? me.at(t.first) : 0.0;
                const double b = ma.count(t.first) ? ma.at(t.first) : 0.0;
                if (std::fabs(a - b) > tol * scale)
                    return false;
            }
        return true;
    }

    auto te = tokenize(expected), ta = tokenize(actual);
    if (te.size() != ta.size())
        return false;
    for (size_t k = 0; k < te.size(); k++)
    {
        if (te[k].isNumber != ta[k].isNumber)
            return false;
        if (te[k].isNumber)
        {
            double a = te[k].value, b = ta[k].value;
            if (std::fabs(a - b) > tol * std::max(1.0, std::max(std::fabs(a), std::fabs(b))))
                return false;
        }
        else if (te[k].text != ta[k].text)
            return false;
    }
    return true;
}

std::vector<std::string> splitLines(const std::string &s)
{
    std::vector<std::string> lines;
    std::istringstream in(s);
    std::string line;
    while (std::getline(in, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        lines.push_back(line);
    }
    return lines;
}

// Returns the number of mismatching lines (reports the first few on stderr).
int compare(const std::string &expected, const std::string &actual, double tol)
{
    auto le = splitLines(expected), la = splitLines(actual);
    int mismatches = 0;
    const int maxReported = 10;
    size_t n = std::max(le.size(), la.size());
    for (size_t i = 0; i < n; i++)
    {
        const std::string *e = (i < le.size()) ? &le[i] : nullptr;
        const std::string *a = (i < la.size()) ? &la[i] : nullptr;
        if (e && a && linesMatch(*e, *a, tol))
            continue;
        if (mismatches < maxReported)
        {
            std::fprintf(stderr, "line %zu:\n  expected: %s\n  actual:   %s\n", i + 1, e ? e->c_str() : "<end of file>",
                         a ? a->c_str() : "<end of file>");
        }
        mismatches++;
        if (!e || !a)
        {
            // one file ended: count the rest as a single mismatch, no point in listing them all
            break;
        }
    }
    if (le.size() != la.size())
        std::fprintf(stderr, "line count: expected %zu, actual %zu\n", le.size(), la.size());
    return mismatches;
}

bool readFile(const fs::path &p, std::string &content)
{
    std::ifstream in(p, std::ios::binary);
    if (!in)
        return false;
    std::ostringstream ss;
    ss << in.rdbuf();
    content = ss.str();
    replaceAll(content, "\r\n", "\n");
    return true;
}

bool writeFile(const fs::path &p, const std::string &content)
{
    if (p.has_parent_path())
        fs::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary);
    out << content;
    return static_cast<bool>(out);
}

int usage()
{
    std::fprintf(stderr, "usage: gaviewer_regress [-I <dir>]... [-l <geo>] [-s <geo>] [-o <actual>] [-g <golden> [-u]] "
                         "[-t <tol>] [-f <n>] <command>\n");
    return 2;
}

} // namespace

int main(int argc, char *argv[])
{
    std::vector<fs::path> includeDirs;
    fs::path outPath, goldenPath;
    bool update = false;
    double tol = 1e-6;
    int maxFrames = 100;
    std::string command;
    fs::path loadPath, savePath;

    for (int i = 1; i < argc; i++)
    {
        std::string_view a = argv[i];
        auto next = [&]() -> const char * { return (i + 1 < argc) ? argv[++i] : nullptr; };
        const char *v = nullptr;
        if (a == "-I" && (v = next()))
            includeDirs.emplace_back(v);
        else if (a == "-o" && (v = next()))
            outPath = v;
        else if (a == "-g" && (v = next()))
            goldenPath = v;
        else if (a == "-u")
            update = true;
        else if (a == "-t" && (v = next()))
            tol = std::atof(v);
        else if (a == "-f" && (v = next()))
            maxFrames = std::atoi(v);
        else if (a == "-l" && (v = next()))
            loadPath = v;
        else if (a == "-s" && (v = next()))
            savePath = v;
        else if (!a.empty() && a[0] != '-' && command.empty())
            command = a;
        else
            return usage();
    }
    if ((command.empty() && loadPath.empty()) || (update && goldenPath.empty()))
        return usage();

    g_cprintfHook = captureCprintf;

    // headless: gui_state stays NULL
    g_state = new state();
    g_state->m_nbNetServers = 0; // never open the network port

    std::string dump = "# gaviewer_regress\n# command: " + command + "\n";

    // load the .g source files (sorted, so the order -- and output -- is deterministic)
    for (const auto &dir : includeDirs)
    {
        std::error_code ec;
        fs::path absDir = fs::absolute(dir, ec);
        std::string generic = absDir.generic_string(), native = absDir.string();
        g_pathSubstitutions.emplace_back(generic, "<dir>");
        g_pathSubstitutions.emplace_back(native, "<dir>");

        std::vector<fs::path> files;
        for (const auto &entry : fs::directory_iterator(absDir, ec))
            if (entry.is_regular_file() && entry.path().extension() == ".g")
                files.push_back(entry.path());
        if (ec)
        {
            std::fprintf(stderr, "cannot read directory '%s': %s\n", native.c_str(), ec.message().c_str());
            return 2;
        }
        std::sort(files.begin(), files.end());
        for (const auto &f : files)
        {
            g_state->consoleSourceFileRead(f.generic_string().c_str());
            if (!g_log.empty() && g_log.back() != '\n')
                g_log += '\n'; // "Parsing '<file>'" is not terminated by a newline
        }
    }
    dump += "== load ==\n" + takeLog();

    if (!loadPath.empty())
    {
        // the GUI reads an opened file from its timer, a part at a time: read it to the end
        if (g_state->openGAfile(loadPath.generic_string().c_str()) != 0)
        {
            std::fprintf(stderr, "cannot open '%s'\n", loadPath.string().c_str());
            return 2;
        }
        for (int k = 0; g_state->currentGAfile() && (k < 100000); k++)
            g_state->currentGAfile()->read(g_state);
        dump += "== open ==\n" + takeLog();
    }

    // run the command, then keep resuming while the script is suspended
    if (!command.empty())
        g_consoleCommandCallback(command.c_str(), g_state);
    int frame = 0;
    for (;;)
    {
        dump += "== frame " + std::to_string(frame) + " ==\n";
        dump += "-- console --\n" + takeLog();
        dump += "-- objects --\n" + dumpObjects();

        if (g_state->m_globalScope->nbSuspendedStmtLists() <= 0)
            break;
        if (++frame >= maxFrames)
        {
            dump += "== frame limit reached ==\n";
            break;
        }
        g_state->consoleCommandCallback(""); // empty input resumes the suspended statement list
    }

    if (!savePath.empty() && (g_state->writeStateToGeoFile(savePath.generic_string().c_str()) != 0))
    {
        std::fprintf(stderr, "cannot write '%s'\n", savePath.string().c_str());
        return 2;
    }

    if (!outPath.empty() && !writeFile(outPath, dump))
    {
        std::fprintf(stderr, "cannot write '%s'\n", outPath.string().c_str());
        return 2;
    }

    if (goldenPath.empty())
    {
        if (outPath.empty())
            std::fwrite(dump.data(), 1, dump.size(), stdout);
        return 0;
    }

    if (update)
    {
        if (!writeFile(goldenPath, dump))
        {
            std::fprintf(stderr, "cannot write '%s'\n", goldenPath.string().c_str());
            return 2;
        }
        std::printf("golden reference written: %s\n", goldenPath.string().c_str());
        return 0;
    }

    std::string expected;
    if (!readFile(goldenPath, expected))
    {
        std::fprintf(stderr, "golden reference '%s' not found (build target 'regress_update_golden' to create it)\n",
                     goldenPath.string().c_str());
        return 1;
    }
    int mismatches = compare(expected, dump, tol);
    if (mismatches)
    {
        std::fprintf(stderr, "%d mismatching line(s) against '%s'%s%s\n", mismatches, goldenPath.string().c_str(),
                     outPath.empty() ? "" : "; actual output: ", outPath.empty() ? "" : outPath.string().c_str());
        return 1;
    }
    return 0;
}
