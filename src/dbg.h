//-----------------------------------------------------------------------------
// Dump utilites.
// 
// Copyright 2019-2020 Marat Shaimardanov.
// Program is licensed under the following three licenses as alternatives:
//    1. GNU Lesser General Public License (LGPL) V2.1 or any newer version
//    2. GNU General Public License (GPL) V2 or any newer version
//    3. Apache License, V2.0 or any newer version
//-----------------------------------------------------------------------------
#ifndef SOLVESPACE_DBG_H
#define SOLVESPACE_DBG_H

#include "solvespace.h"

// author of StringBuilder Pablo Aliskevicius
template <typename chr>
class StringBuilder {
private:
    typedef std::basic_string<chr> string_t;
    // Tried also vector and list. Deque wan, albeit by a narrow margin.
    typedef std::vector<string_t> container_t;
    // Reuse the size type in the string.
    typedef typename string_t::size_type size_type;
    container_t m_Data;
    size_type m_totalSize;
    void append(const string_t &src) {
        m_Data.push_back(src);
        m_totalSize += src.size();
    }
    // No copy constructor, no assignement.
    StringBuilder(const StringBuilder &);
    StringBuilder & operator = (const StringBuilder &);
public:
    int lno;
    StringBuilder(const string_t &src) {
        if (!src.empty()) {
            m_Data.push_back(src);
        }
        m_totalSize = src.size();
    }

    StringBuilder() {
        m_totalSize = 0;
        lno = 0;
    }

    void Clear() {
        m_Data.clear();
        m_totalSize = 0;
        lno = 0;
    }

    StringBuilder & Append(const string_t &src) {
        append(src);
        return *this; // allow chaining.
    }

    // This one lets you add any STL container to the string builder. 
    template<class inputIterator>
    StringBuilder & Add(const inputIterator &first, const inputIterator &afterLast) {
        // std::for_each and a lambda look like overkill here.
        // <b>Not</b> using std::copy, since we want to update m_totalSize too.
        for (inputIterator f = first; f != afterLast; ++f) {
            append(*f);
        }
        return *this; // allow chaining.
    }

    StringBuilder & AppendLine(const string_t &src) {
        static chr lineFeed[]{ 10, 0 }; 
        m_Data.push_back(src + lineFeed);
        m_totalSize += 1 + src.size();
        lno++;
        return *this; // allow chaining.
    }

    StringBuilder & AppendLine() {
        static chr lineFeed[]{ 10, 0 };
        m_Data.push_back(lineFeed);
        ++m_totalSize;
        lno++;
        return *this; // allow chaining.
    }

    // Like C# StringBuilder.ToString()
    // Note the use of reserve() to avoid reallocations. 
    string_t ToString() const {
        string_t result;
        result.reserve(m_totalSize + 1);
        // result = std::accumulate(m_Data.begin(), m_Data.end(), result);
        // This would lose the advantage of 'reserve'
        for (auto iter = m_Data.begin(); iter != m_Data.end(); ++iter) {
            result += *iter;
        }
        return result;
    }

    // like javascript Array.join()
    string_t Join(const string_t &delim) const {
        if (delim.empty()) {
            return ToString();
        }
        string_t result;
        if (m_Data.empty()) {
            return result;
        }
        size_type st = (delim.size() * (m_Data.size() - 1)) + m_totalSize + 1;
        result.reserve(st);
        // If you need reasons to love C++11, here is one.
        struct adder {
            string_t m_Joiner;
            adder(const string_t &s) : m_Joiner(s) {
              // This constructor is NOT empty.
            }
            // This functor runs under accumulate() without
            // reallocations, if 'l' has reserved enough memory. 
            string_t operator()(string_t &l, const string_t &r) {
                l += m_Joiner;
                l += r;
                return l;
            }
        } adr(delim);
        auto iter = m_Data.begin();
        // Skip the delimiter before the first element in the container.
        result += *iter;
        return std::accumulate(++iter, m_Data.end(), result, adr);
    }

};

// Fast random generator, compatible with Delphi
class SRandom {
public:
    static int seed;
    static uint32_t Def() {
        seed = static_cast<uint32_t>(seed) * 0x08088405 + 1;
        return seed;
    }
    static void Seed(int v) {
        seed = v;
    }
    static double Dbl() {
        const double two2neg32 = ((1.0 / 0x10000) / 0x10000);  // 2^-32
        double f = Def();
        return f * two2neg32;
    }
    static int Int(const int range) {
        uint32_t t = Def();
        return (static_cast<uint64_t>(static_cast<uint64_t>(range)) * static_cast<uint64_t>(t)) >> 32;
    }
};

// Class for dump SolveSpase data structures
class SDump {
public:
    StringBuilder<char> log;
    int count, breaks, trn;
    bool on, tr, bspon;
    std::string root;
    void BreakPoint(int lo, int hi);
    void Init();
    void Clear();
    void Save(const std::string *filename, int n);

    double Rz(double v);
    std::string _Str(const char *name, const std::string &s);
    std::string _Hex(const char *name, int v);
    std::string _Int(const char *name, int v);
    std::string _Flt(const char *name, double f);
    // Types
    char * _SurfaceCombine(SSurface::CombineAs type);
    char * _GroupCombine(Group::CombineAs how);
    char * _ShellKind(SShell::Class kind);
    // Objects
    void _DoubleList(const char *name, List<double> *list);
    void _Entity(const char *name, const Entity *e);
    void _EntityList(const char *name, IdList<Entity, hEntity> *el);
    void _Params(const char *name, ParamList *list);
    void _Param(const char *name, Param *p);
    void _Request(const char *name, Request *r);
    void _Vector(const char *name, const Vector *v);
    void _Quaternion(const char *name, Quaternion *q);
    void _BezierLoopSet(const char *name, SBezierLoopSet *bls);
    void _BezierList(const char *name, SBezierList *bls);
    void _Bezier(const char *name, SBezier *b);
    void _Polygon(const char *name, SPolygon *poly);
    void _Contour(const char *name, SContour *sc);
    void _Shell(const char *name, SShell *sh);
    void _Surface(const char *name, SSurface *srf);
    void _TrimByList(const char *name, List<STrimBy> *trim);
    void _TrimBy(const char *name, STrimBy *stb);
    void _Edges(const char *name, SEdgeList *edges);
    void _Edge(const char *name, SEdge *e);
    void _Points(const char *name, SPointList *pts);
    void _Curve(const char *name, SCurve *sc);
    void _Mesh(const char *name, SMesh *m);
    void _Triangle(const char *name, const STriangle *tr);
    void _Bsp3(const char *name, SBsp3 *root);
    void Preorder3(const char * name, int level, SBsp3 *node);
    void _Bsp2(const char *name, int level, SBsp2 *root);
    void _BspUv(const char *name, SBspUv *root);
    // Boolean operations
    void _MakeFromBoolean(const char *name,
        SShell *a, SShell *b, SShell *r,
        SSurface::CombineAs type);
    void _GenerateForBoolean(const char *name,
        SShell *prevs, SShell *thiss, SShell *outs,
        Group::CombineAs how);
};

extern SDump dump;

#endif
