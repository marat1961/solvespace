//-----------------------------------------------------------------------------
// Dump utilites.
//
// Copyright 2019-2020 Marat Shaimardanov.
// Program is licensed under the following three licenses as alternatives:
//    1. GNU Lesser General Public License (LGPL) V2.1 or any newer version
//    2. GNU General Public License (GPL) V2 or any newer version
//    3. Apache License, V2.0 or any newer version
//-----------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "solvespace.h"
#include "config.h"
#include "dbg.h"

int SRandom::seed = 0;
SDump dump = {};

void SDump::BreakPoint(int lo, int hi)
{
    if (log.lno >= lo && log.lno <= hi) {
        breaks++;
    }
}

void SDump::Init()
{
    on = true;
    count = 0;
    breaks = 0;
    Clear();
    root = "c:\\test\\group\\";
}

void SDump::Clear()
{
    log.Clear();
}

void SDump::Save(const std::string *filename, int n)
{
    std::string s = root;
    s.append(filename->c_str());
    std::ofstream fs;
    fs.open(s);
    fs << log.ToString() << std::endl;
    fs.close();
    count = n;
}

double SDump::Rz(double v) {
    if (fabs(v) < 1e-8) v = 0.0;
    return v;
 }

std::string SDump::_Str(const char *name, const std::string &s) {
    if (s.empty())
        return "";
    else
        return ssprintf("%s=%s", name, s.c_str());
}

std::string SDump::_Hex(const char *name, int v) {
    if (v == 0)
        return "";
    else
        return ssprintf("%s=%08x", name, v);
}

std::string SDump::_Int(const char *name, int v) {
    if (v == 0)
        return "";
    else
        return ssprintf("%s=%d", name, v);
}

std::string SDump::_Flt(const char *name, double f) {
    if EXACT(f == 0.0)
        return "";
    else
        return ssprintf("%s=%.4f", name, Rz(f));
}

char *SDump::_SurfaceCombine(SSurface::CombineAs type) {
    switch (type) {
    case SSurface::CombineAs::UNION: return "UNION";
    case SSurface::CombineAs::DIFFERENCE: return "DIFFERENCE";
    case SSurface::CombineAs::INTERSECTION: return "INTERSECTION";
    default: return "UNKNOWN";
    }
}

char *SDump::_GroupCombine(Group::CombineAs how) {
    switch (how) {
    case Group::CombineAs::UNION: return "UNION";
    case Group::CombineAs::DIFFERENCE: return "DIFFERENCE";
    case Group::CombineAs::INTERSECTION: return "INTERSECTION";
    case Group::CombineAs::ASSEMBLE: return "ASSEMBLE";
    default: return "UNKNOWN";
    }
}

char * SDump::_ShellKind(SShell::Class kind) {
    switch (kind) {
    case SShell::Class::INSIDE: return "INSIDE";
    case SShell::Class::OUTSIDE: return "OUTSIDE";
    case SShell::Class::COINC_SAME: return "COINC_SAME";
    case SShell::Class::COINC_OPP: return "COINC_OPP";
    default: return "UNKNOWN";
    }
}

void SDump::_DoubleList(const char *name, List<double> *list) {
    dbp("%s Count=%d", name, list->n);
    for (int i = 0; i < list->n; i++) {
        double v = list->Get(i);    
        dbp("  v[%d]=%.4f ", i, v);
    }
}

void SDump::_Entity(const char *name, const Entity *e) {
    std::string s;
    if (e->h.isFromRequest()) {
        Request *r = SK.GetRequest(e->h.request());
        s = r->DescriptionString();
    }
    else {
        Group *g = SK.GetGroup(e->h.group());
        s = ssprintf("g%03x-%s", g->h.v, g->TypeToString().c_str());
    }
    dbp("%s=%08x %s type=%s", name, e->h.v, s.c_str(), e->TypeToString().c_str());
    s = _Int("  construction", e->construction);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Int("  style", e->style.v);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Str("  str", e->str);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Str("  font", e->font);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Str("  file", e->file.raw);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Hex("  style=", e->style.v);
    if (!s.empty()) dbp("%s", s.c_str());
    for (int i = 0; i < MAX_POINTS_IN_ENTITY; i++) {
        if (e->point[i].v) {
            dbp("  point[%d]=%08x", i, e->point[i].v);
        }
    }
    s = _Int("extraPoints", e->extraPoints);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Hex("  normal", e->normal.v);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Hex("  distance", e->distance.v);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Hex("  workplane", e->workplane.v);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Flt("  actPoint.x", e->actPoint.x);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Flt("  actPoint.y", e->actPoint.y);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Flt("  actPoint.z", e->actPoint.z);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Flt("  actNormal.w", e->actNormal.w);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Flt("  actNormal.vx", e->actNormal.vx);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Flt("  actNormal.vy", e->actNormal.vy);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Flt("  actNormal.vz", e->actNormal.vz);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Flt("  actDistance", e->actDistance);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Int("  actVisible", e->actVisible);
    if (!s.empty()) dbp("%s", s.c_str());
}

void SDump::_EntityList(const char *name, IdList<Entity, hEntity> *el) {
    dbp("%s Count=%d", name, el->n);
    for (int i = 0; i < el->n; i++) {
        Entity *e = &el->Get(i);
        _Entity(ssprintf("  e[%d]", i).c_str(), e);
    }
}

void SDump::_Params(const char *name, ParamList *list) {
    dbp("%s Count=%d", name, list->n);
    for (int i = 0; i < list->n; i++) {
        Param *p = &list->Get(i);
        _Param(ssprintf("  p[%d]", i).c_str(), p);
    }
}

void SDump::_Param(const char *name, Param *p) {
    dbp("%s h.v=%08x val=%.4f", name, p->h.v, Rz(p->val));
}

void SDump::_Request(const char *name, Request *r) {
    dbp("%s %s group=%03x", name, r->DescriptionString().c_str(), r->group.v);
    if (r->construction) dbp("  construction=True");
}

void SDump::_Vector(const char *name, const Vector *v) {
    dbp("%s (x=%.4f y=%.4f z=%.4f)", name, Rz(v->x), Rz(v->y), Rz(v->z));
}

void SDump::_Quaternion(const char *name, Quaternion *q) {
    dbp("%s (w=%.4f x=%.4f y=%.4f z=%.4f)", name, Rz(q->w), Rz(q->vx), Rz(q->vy), Rz(q->vz));
}

void SDump::_BezierLoopSet(const char *name, SBezierLoopSet *bls) {
    dbp("%s Count=%d", name, bls->l.n);
    _Vector("  normal", &bls->normal);
    _Vector("  point", &bls->point);
    std::string s = _Flt("  area", bls->area);
    if (!s.empty()) dbp("%s", s.c_str());
    for (int i = 0; i < bls->l.n; i++) {
        dbp("  List[%d]", i);
        SBezierLoop *loop = &bls->l.Get(i);
        for (int j = 0; j < loop->l.n; j++) {
            SBezier *b = &loop->l.Get(i);
            SDump::_Bezier(ssprintf("  Bezier[%d]", j).c_str(), b);
        };
    };
}

void SDump::_BezierList(const char * name, SBezierList * bls) {
    dbp("%s Count=%d", name, bls->l.n);
    for (int i = 0; i < bls->l.n; i++) {
        SBezier *b = &bls->l.Get(i);
        SDump::_Bezier(ssprintf("  Bezier[%d]", i).c_str(), b);
    };
}

void SDump::_Bezier(const char *name, SBezier *b) {
    dbp(name);
    for (int i = 0; i <= b->deg; i++) {
        Vector v = b->ctrl[i];
        double w = b->weight[i];
        dbp("  ctrl[%d] (x=%.4f, y=%.4f, z=%.4f) weight=%.4f", i, Rz(v.x), Rz(v.y), Rz(v.z), Rz(w));
    }
}

void SDump::_Polygon(const char * name, SPolygon * poly) {
    dbp(name);
    for (int i = 0; i < poly->l.n; i++) {
        SContour *sc = &(poly->l.Get(i));
        std::string s = ssprintf("  sc[%d]", i);
        _Contour(s.c_str(), sc);
    }
}

void SDump::_Contour(const char *name, SContour *sc) {
    dbp(name);
    for (int i = 0; i < sc->l.n; i++) {
        SPoint *pt = &(sc->l.Get(i));
        dbp("  p[%d] (x=%.4f, y=%.4f, z=%.4f)", i, Rz(pt->p.x), Rz(pt->p.y), Rz(pt->p.z));
    }
}

void SDump::_Shell(const char *name, SShell *sh) {
    dbp(name);
    dbp("  surfaces=%d", sh->surface.n);
    for (int i = 0; i < sh->surface.n; i++) {
        SSurface *srf = &sh->surface.Get(i);
        std::string s = ssprintf("  srf[%d]", i);
        _Surface(s.c_str(), srf);
    }
    dbp("  curves=%d", sh->curve.n);
    for (int i = 0; i < sh->curve.n; i++) {
        SCurve *crv = &sh->curve.Get(i);
        std::string s = ssprintf("  crv[%d]", i);
        _Curve(s.c_str(), crv);
    }
}

void SDump::_Surface(const char *name, SSurface *srf) {
    dbp("%s degm=%d, degn=%d", name, srf->degm, srf->degn);
    std::string s = _Hex("  h", srf->h.v);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Hex("  face", srf->face);
    if (!s.empty()) dbp("%s", s.c_str());
    s = _Hex("  color", srf->color.ToPackedInt());
    if (!s.empty()) dbp("%s", s.c_str());
    for (int i = 0; i <= srf->degm; i++) {
        for (int j = 0; j <= srf->degn; j++) {
            Vector v = srf->ctrl[i][j];
            double w = srf->weight[i][j];
            dbp("  ctrl[%d, %d] (x=%.4f, y=%.4f, z=%.4f) weight=%.4f",
                i, j, Rz(v.x), Rz(v.y), Rz(v.z), Rz(w));
        }
    }
    _TrimByList("  trim", &srf->trim);
    _Edges("  edges", &srf->edges);
}

void SDump::_TrimByList(const char *name, List<STrimBy> *trim) {
    if (trim->n == 0) return;
    dbp("%s Count=%d", name, trim->n);
    for (int i = 0; i < trim->n; i++) {
        STrimBy *stb = &(trim->Get(i));
        std::string s = ssprintf("  trim[%d]", i);
        _TrimBy(s.c_str(), stb);
    }
}

void SDump::_TrimBy(const char *name, STrimBy *stb) {
    dbp(name);
    std::string s = _Hex("  curve", stb->curve.v);
    if (!s.empty()) dbp("%s", s.c_str());
    if (stb->backwards)
        dbp("  backwards=True");
    else
        dbp("  backwards=False");
    _Vector("  start", &(stb->start));
    _Vector("  finish", &(stb->finish));
}

void SDump::_Edges(const char *name, SEdgeList *edges) {
    if (edges->l.n == 0) return;
    dbp("%s Count=%d", name, edges->l.n);
    for (int i = 0; i < edges->l.n; i++) {
        SEdge *e = &edges->l.Get(i);
        std::string s = ssprintf("  [%d]", i);
        _Edge(s.c_str(), e);
    }
}

void SDump::_Edge(const char * name, SEdge * e)
{
    double len = e->Length();
    dbp("%s A(x=%.4f y=%.4f z=%.4f) B(x=%.4f y=%.4f z=%.4f) %.4f",
        name, Rz(e->a.x), Rz(e->a.y), Rz(e->a.z), Rz(e->b.x), Rz(e->b.y), Rz(e->b.z), len);
}

void SDump::_Points(const char * name, SPointList * pts) {
    if (pts->l.n == 0) return;
    dbp("%s Count=%d", name, pts->l.n);
    for (int i = 0; i < pts->l.n; i++) {
        SPoint *pt = &pts->l.Get(i);
        dbp("  [%d] p(x=%.4f y=%.4f z=%.4f) auxv(x=%.4f y=%.4f z=%.4f)",
            i, Rz(pt->p.x), Rz(pt->p.y), Rz(pt->p.z), Rz(pt->auxv.x), Rz(pt->auxv.y), Rz(pt->auxv.z));
    }
}

void SDump::_Curve(const char *name, SCurve *sc) {
    dbp("%s h=%08x", name, sc->h.v);
    switch (sc->source) {
    case SCurve::Source::A:
        dbp("  source=A");
        break;
    case SCurve::Source::B:
        dbp("  source=B");
        break;
    case SCurve::Source::INTERSECTION:
        dbp("  source=INTERSECTION");
        break;
    }
    dbp("  surfA=%08x surfB=%08x", sc->surfA.v, sc->surfB.v);
    _Bezier("  exact", &sc->exact);
    for (int i = 0; i < sc->pts.n; i++) {
        SCurvePt pt = sc->pts.Get(i);
        std::string s;
        if (pt.vertex)
            s = "vertex";
        else
            s = " point";
        dbp("  %s[%d] (x=%.4f, y=%.4f, z=%.4f)", s.c_str(), i, Rz(pt.p.x), Rz(pt.p.y), Rz(pt.p.z));
    }
}

void SDump::_Mesh(const char *name, SMesh *m) {
    dbp("%s Count=%d", name, m->l.n);
    for (int i = 0; i < m->l.n; i++) {
        STriangle *tr = &m->l.Get(i);
        std::string s = ssprintf("  tr[%d]", i);
        _Triangle(s.c_str(), tr);
    }
}

void SDump::_Triangle(const char * name, const STriangle * tr) {
    dbp("%s face=%08x color=%08x", name, tr->meta.face, tr->meta.color.ToPackedInt());
    _Vector("  a", &tr->a);
    _Vector("  b", &tr->b);
    _Vector("  c", &tr->c);
    _Vector("  an", &tr->an);
    _Vector("  bn", &tr->bn);
    _Vector("  cn", &tr->cn);
}

void SDump::Preorder3(const char * name, int level, SBsp3 *node) {
    if (node == NULL) return;
    // number of character spaces for a given indentation
    int spaces = strlen(name) + level;
    dbp("%*s d=%.4f", spaces, name, node->d);
    std::string s = ssprintf("%*s", 1 + level, "n");
    _Vector(s.c_str(), &node->n);
    s = ssprintf("%*s", 3 + level, "tri");
    _Triangle(s.c_str(), &node->tri);
    _Bsp2("edges", level, node->edges);
    Preorder3("pos", level + 1, node->pos);
    Preorder3("neg", level + 1, node->neg);
    Preorder3("more", level + 1, node->more);
}

void SDump::_Bsp3(const char * name, SBsp3 *root) {
    Preorder3(name, 0, root);
}

void SDump::_Bsp2(const char * name, int level, SBsp2 *node) {
    if (node == NULL) return;
    // number of character spaces for a given indentation
    int spaces = strlen(name) + level;
    dbp("%*s d=%.4f", spaces, name, node->d);
    std::string s = ssprintf("%*s", 2 + level, "np");
    _Vector(s.c_str(), &node->np);
    s = ssprintf("%*s", 2 + level, "no");
    _Vector(s.c_str(), &node->no);
    s = ssprintf("%*s", 4 + level, "edge");
    _Edge(s.c_str(), &node->edge);
    _Bsp2("pos", level + 1, node->pos);
    _Bsp2("neg", level + 1, node->neg);
    _Bsp2("more", level + 1, node->more);
}

void Preorder(const char * name, int level, SBspUv *node) {
    if (node == NULL) return;
    // number of character spaces for a given indentation
    int spaces = strlen(name) + level;
    dbp("%*s a(x=%.4f y=%.4f) b(x=%.4f x=%.4f)",
        spaces, name, dump.Rz(node->a.x), dump.Rz(node->a.y), dump.Rz(node->b.x), dump.Rz(node->b.y));
    Preorder("pos", level + 1, node->pos);
    Preorder("neg", level + 1, node->neg);
    Preorder("more", level + 1, node->more);
}

void SDump::_BspUv(const char * name, SBspUv *root) {
    Preorder(name, 0, root);
}

void SDump::_MakeFromBoolean(const char * name,
    SShell * a, SShell * b, SShell * r,
    SSurface::CombineAs type) {
    dbp("%s type=%s", name, _SurfaceCombine(type));
    _Shell("  a", a);
    _Shell("  b", b);
    _Shell("  r", r);
}

void SDump::_GenerateForBoolean(const char * name,
    SShell *prevs, SShell *thiss, SShell * outs,
    Group::CombineAs how) {
    dbp("%s how=%s", name, _GroupCombine(how));
    _Shell("  prevs", prevs);
    _Shell("  thiss", thiss);
    _Shell("  outs", outs);
}
