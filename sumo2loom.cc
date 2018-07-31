// $Id: sumo2loom.cc,v 1.52 2002/11/06 15:58:53 dflater Exp $

// This software was developed at the National Institute of Standards
// and Technology by employees of the Federal Government in the course
// of their official duties.  Pursuant to title 17 Section 105 of the
// United States Code this software is not subject to copyright
// protection and is in the public domain.
// 
// You can redistribute it and/or modify it freely provided that any
// derivative works bear some notice that they are derived from it,
// and any modified versions bear some notice that they have been
// modified.  We would appreciate acknowledgement if the software is
// used.
// 
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  NIST does not
// assume legal liability or responsibility for anything done with
// this software.

#include "header.hh"

// Generate line number printouts for debugging?
#undef COUNT_BUGS

// Do you want everything inside one big tell command or do you want
// separate tells for each assertion?
#undef ONE_BIG_TELL

// Enable problematic code having to do with disjoint coverings.
#undef DISJOINTS

// Enable unrolling of @ROW instead of rejecting it?
#define UNROLL_ROWS

// @ROW variables must be unrolled ?ROW1, ?ROW1 ?ROW2, ... etc. up to
// some limit.  This is that limit.
#define ROW_VARIABLE_UNROLLING_LIMIT 5

// String used to initialize elements in the domain vectors.  In theory
// you should never see this appear in the output.
#define DOMAINFILLER "******ERROR******"

#ifdef COUNT_BUGS
unsigned long bug_count = 0;
#endif

// struct cstruct is used to accumulate the information needed to
// generate a defconcept statement.

struct cstruct {
  std::set<Dstr> isa;
  Dstr partitions;
  bool primitive;
};

// struct rstruct is used to accumulate the information needed to
// generate a defrelation statement.

struct rstruct {
  std::set<Dstr> isa;
  unsigned arity;
  std::vector<Dstr> domain;
  Dstr range;
  Dstr inverse;
  bool symmetric;
  bool commutative;
};

// This table is used in translate_proposition.  It is also used in
// note_rejects, but only to determine which of the things that have
// Formula in their domain should be tolerated.  (If it's in this
// table, it must be tolerable.)  The third column is used in
// determining which implications are translatable.

#define numreps 8
char *prop_direct_replacements[numreps][3] = {
  {"and", ":and", "t"},
  {"or", ":or", "t"},
  {"not", ":fail", "t"},
    // {"exists", ":for-some", "t"},
    //  {"forall", ":for-all", "t"},
  {"=>", ":implies", "t"},
  {"equal", "=", NULL},
  {"instance", "instance-of", NULL},
  {"subrelation", "superrelations", NULL},
  {"subclass", "superrelations", NULL}};
// The previous line looks like a cut-and-paste error but it's not.
// superrelations does concepts too.

// add_parent
// in:  isa, set of parents
//      newparent, the new parent
//      myname, the name of the inheritee
// out:  isa, possibly modified
// Attempts to duplicate are flagged.
void add_parent (std::set<Dstr> &isa, Dstr &newparent, const Dstr &myname) {
  std::set<Dstr>::iterator it = isa.find(newparent);
  if (it != isa.end())
    fprintf (stderr, "Warning:  %s asserted to inherit from %s more than once\n",
      myname.aschar(), newparent.aschar());
  else
    isa.insert (newparent);
}

// genpsym
// in:   buf, contents irrelevant.
// out:  buf, containing a unique name for a new LOOM partition
//       (over 4 billion served).

void genpsym (Dstr &buf) {
  static unsigned long s = 1;
  buf = "$PARTITION_";
  buf += s++;
  buf += '$';
}

// comment_out
// in:   buf, containing a single SUO-KIF statement.
// out:  buf, containing that statement, commented out.

void comment_out (Dstr &buf) {
  buf.repstr ("\n", "\n;; ");
  assert (buf.length() > 3);
  assert (!strncmp (buf.ascharfrom(buf.length()-3), ";; ", 3));
  buf -= buf.length()-3;
  buf *= ";; ";
}

// note_rejects
//
// in:   buf, containing a single SUO-KIF statement.
//       rejects, accumulated rejected functions/relations/predicates.
// out:  buf, contents unspecified.
//       rejects, possibly with new stuff added.
//
// see also, unsupported.

void note_rejects (Dstr &buf, std::map<Dstr, Dstr *> &rejects) {
  if (buf[0] != '(')
    return;
  buf /= 1;
  Dstr arg0;
  buf /= arg0;
  if (arg0 == "domain") {
    Dstr arg1, arg2, arg3;
    buf /= arg1;
    buf /= arg2;
    buf /= arg3;
    if (arg3 == "Formula") {
      for (unsigned i=0; i<numreps; i++)
        if (arg1 == prop_direct_replacements[i][0])
          return; // You're allowed.
      if (arg1 == "<=>" || arg1 == "exists")
        return; // You're allowed too.
      Dstr *reason = rejects[arg1];
      if (!reason) {
        assert (reason = new Dstr());
        if (arg1 == "forall")
          *reason = "translates to uncomputable Loom query expressions";
        else
          *reason = "has Formula in its domain";
        rejects[arg1] = reason;
      }
    }
  } else if (arg0 == "instance") {
    Dstr arg1, arg2;
    buf /= arg1;
    buf /= arg2;
    if (arg2 == "VariableArityRelation" && arg1 != "ListFn") {
      Dstr *reason = rejects[arg1];
      if (!reason) {
        assert (reason = new Dstr ("is a VariableArityRelation"));
	rejects[arg1] = reason;
      }
    }
  }
}

// find_or_create
// in:   defrelations, accumulated relation information.
//       rname, name of relation wanted.
// out:  defrelations, possibly with new relation added.
//       return value, pointer to the rstruct for rname.
rstruct *find_or_create (std::map<Dstr, rstruct *> &defrelations, Dstr &rname)
{
  rstruct *r = defrelations[rname];
  if (!r) {
    assert (r = new rstruct);
    defrelations[rname] = r;
    r->arity = 0;
    r->symmetric = r->commutative = false;
  }
  return r;
}

// find_or_create
// in:   defconcepts, accumulated concept information.
//       cname, name of concept wanted.
// out:  defconcepts, possibly with new concept added.
//       return value, pointer to the cstruct for cname.
cstruct *find_or_create (std::map<Dstr, cstruct *> &defconcepts, Dstr &cname)
{
  cstruct *c = defconcepts[cname];
  if (!c) {
    assert (c = new cstruct);
    defconcepts[cname] = c;
    c->primitive = true;
  }
  return c;
}

// add_tell
// in:   buf, containing a translated statement.
//       body, accumulated tells for the "body" of the output.
// out:  buf, contents indeterminate.
//       body, with the contents of buf added as a new tell.

void add_tell (Dstr &buf, Dstr &body) {
#ifndef ONE_BIG_TELL
  buf *= "(tell ";
  if (buf[buf.length()-1] == '\n')
    buf -= buf.length()-1;
  buf += ")\n";
#ifdef COUNT_BUGS
  body += "(print '(bug ";
  body += bug_count++;
  body += "))\n";
#endif
#endif
  body += buf;
}

// extract_vars {recursive}
// in:  vars, a set to get the variables.
//      l, the Suo-Kif statement or portion thereof.
// out:  vars, with variables added.
//
// This function was reverted from the extract_free_vars in rev 1.48
// back to the dumb implementation of rev. 1.32 when it became clear
// that nested quantifiers never work anyway.
//
void extract_vars (std::set<Dstr> &vars, const Dstr &l) {
  assert (!(l.isNull()));
  switch (l[0]) {
  case '(':
    {
      Dstr remainder(l), arg;
      remainder /= 1;
      remainder /= arg;
      while (!(arg.isNull())) {
        extract_vars (vars, arg);
        remainder /= arg;
      }
    }
    break;
  case '?':
    vars.insert (l);
    break;
  default:
    ;
  }
}

// check_word
// in:  word, the word to check.
//      words, the words to find.
// out:  if found, returns pointer to the first component of
//       the relevant element in the words map.  Otherwise, NULL.
const Dstr *check_word (const Dstr &word, std::map<Dstr, Dstr *> &words) {
  std::map<Dstr, Dstr *>::iterator it = words.begin();
  while (it != words.end()) {
    if (word == it->first)
      return &(it->first);
    it++;
  }
  return NULL;
}

// find_words {recursive}
// in:  buf, the Suo-Kif statement or portion thereof.
//      words, the words to find.
//      applied, true if only looking for position 1.
//      excepted, true if top-level application is exempt.
// out:  if found, returns pointer to the first component of
//       the relevant element in the words map.  Otherwise, NULL.
const Dstr *find_words (const Dstr &buf, std::map<Dstr, Dstr *> &words,
bool applied, bool excepted) {
  assert (!(buf.isNull()));
  if (buf[0] == '(') {
    Dstr remainder(buf), arg;
    remainder /= 1;
    remainder /= arg;
    if (arg[0] != '(') {
      if (!excepted) {
        const Dstr *r = check_word (arg, words);
        if (r)
          return r;
      }
      remainder /= arg;
    }
    while (!(arg.isNull())) {
      const Dstr *r = find_words (arg, words, applied, false);
      if (r)
        return r;
      remainder /= arg;
    }
    return NULL;
  } else {
    if (applied)
      return NULL;
    else
      return check_word (buf, words);
  }
}

// do_renamings
// in:  buf, containing a Suo-Kif or Loom statement.
// out:  buf, with words replaced.  See comments below.
//
// Logical operators are replaced in translate_proposition, not here.

void do_renamings (Dstr &buf) {
  // Clashes with builtins.
  buf.repstr ("Character",     "Sumocharacter");
  buf.repstr ("Class",         "Sumoclass");
  buf.repstr ("Collection",    "Sumocollection");
  buf.repstr ("domain",        "sumodomain");
  buf.repstr ("Function",      "Sumofunction");
  buf.repstr ("Integer",       "Sumointeger");
  buf.repstr ("List",          "Sumolist");
  buf.repstr ("Number",        "Sumonumber");
  buf.repstr ("Proposition",   "Sumoproposition");
  buf.repstr ("property",      "sumoproperty");
  buf.repstr ("range",         "sumorange");
  buf.repstr ("Relation",      "Sumorelation");
  buf.repstr ("Set",           "Sumoset");
  buf.repstr ("Sequence",      "Sumosequence");
  buf.repstr ("subset",        "Sumosubset");
  buf.repstr ("disjoint",      "sumodisjoint");
  buf.repstr ("documentation", "sumodocumentation");
  buf.repstr ("inverse",       "sumoinverse");
  // Case sensitivity clashes.
  buf.repstr ("agent", "agent-rel");          // agent != Agent
  buf.repstr ("attribute", "attribute-rel");  // attribute != Attribute
  // Replace interesting Fns with LOOM builtins.
  buf.repstr ("(SumolistFn ",  "(:the-list ");
  buf.repstr ("(SumolistFn\n", "(:the-list\n");
  buf.repstr ("(SumolistFn(",  "(:the-list (");
}

// do_additional_setfn_renamings
// in:  buf, containing a Suo-Kif or Loom statement.
// out:  buf, with words replaced.
//
// UnionFn and IntersectionFn are needed for :domain and :range but
// cause trouble for implications.

void do_additional_setfn_renamings (Dstr &buf) {
  buf.repstr ("(UnionFn ",  "(:or ");
  buf.repstr ("(UnionFn\n", "(:or\n");
  buf.repstr ("(UnionFn(",  "(:or (");
  buf.repstr ("(IntersectionFn ",  "(:and ");
  buf.repstr ("(IntersectionFn\n", "(:and\n");
  buf.repstr ("(IntersectionFn(",  "(:and (");
}

// translate_proposition {recursive}
// in:  buf, containing a SUO-KIF proposition.
// out:  buf, possibly containing a translation of that proposition.
//       return value, true iff proposition is "hairy."
//
// A proposition is "hairy" if it would require a :satisfies to express
// it in Loom.  You cannot use :satisfies on both sides of a Loom
// implies statement.
//
// This function does not invoke do_renamings or create new
// definitions.

bool translate_proposition (Dstr &buf) {
  bool ret = false;
  if (buf[0] != '(')
    return false;
  bool newlterm = (buf[buf.length()-1] == '\n');
  Dstr arg0, remainder(buf);
  remainder /= 1; // Skip paren
  remainder /= arg0;
  for (unsigned i=0; i<numreps; i++) {
    if (arg0 == prop_direct_replacements[i][0]) {
      ret = (prop_direct_replacements[i][2] != NULL);
      buf = '(';
      buf += prop_direct_replacements[i][1];
      Dstr argn;
      remainder /= argn;
      while (!(argn.isNull())) {
	if (translate_proposition (argn))
          ret = true;
	buf += ' ';
	buf += argn;
	remainder /= argn;
      }
      buf += ')';
      if (newlterm)
        buf += '\n';
      return ret;
    }
  }
  if (arg0 == "<=>") {
    Dstr arg1, arg2;
    remainder /= arg1;
    remainder /= arg2;
    translate_proposition (arg1);
    translate_proposition (arg2);
    buf = "(:and (:implies ";
    buf += arg1;
    buf += ' ';
    buf += arg2;
    buf += ") (:implies ";
    buf += arg2;
    buf += ' ';
    buf += arg1;
    buf += "))";
    if (newlterm)
      buf += '\n';
    return true;
  } else if (arg0 == "exists") {
    // We are going to have an enclosing :for-some anyway so this
    // one can be scrapped.
    Dstr arg1, arg2;
    remainder /= arg1;
    remainder /= buf;
    translate_proposition (buf);
    if (newlterm)
      buf += '\n';
    return true;
  } else {
    // Generic (some-rel X Y):  X or Y could be hairy.
    buf = '(';
    buf += arg0;
    Dstr argn;
    remainder /= argn;
    while (!(argn.isNull())) {
      if (translate_proposition (argn))
	ret = true;
      buf += ' ';
      buf += argn;
      remainder /= argn;
    }
    buf += ')';
    if (newlterm)
      buf += '\n';
    return ret;
  }
}

// translate_arrow
//
// A top level => is an assertion that translates to (implies.
// A lower level => is a proposition that translates to (:implies.
// This function only does the former.
//
// in:  arg1 and arg2, the antecedent and consequent.
//      out, contents irrelevant.
//      orig_statement, containing the original Suo-Kif statement.
// out:  out, containing a translation if one exists.
//       null means that it was too hairy to translate.

void translate_arrow (const Dstr &arg1_in, const Dstr &arg2_in, Dstr &out,
const Dstr &orig_statement) {
  assert (!(arg1_in.isNull()));
  assert (!(arg2_in.isNull()));
  Dstr antecedent(arg1_in), consequent(arg2_in);
  if (translate_proposition (consequent))
    out = (char *)NULL;
  else {
    translate_proposition (antecedent);

    // Get variables in the hairy one.
    assert (antecedent[0] == '(');
    std::set<Dstr> antecedentvars;
    extract_vars (antecedentvars, antecedent);

    // Get everything in consequent.

    // Although we needed to call translate_proposition to determine
    // that the consequent was simple, we don't actually want to use
    // the result because we can't assert against Loom's builtin
    // relations (instance-of, =, etc.).
    // Dstr remainder (consequent);
    Dstr remainder (arg2_in);

    assert (remainder[0] == '(');
    remainder /= 1; // Skip paren
    std::vector<Dstr> consequentargs;
    Dstr consequentfn, temparg;
    remainder /= consequentfn; // Save the fn for later.
    remainder /= temparg;
    while (!(temparg.isNull())) {
      consequentargs.push_back (temparg);
      remainder /= temparg;
    }

    if (!(antecedentvars.size()) || !(consequentargs.size())) {
      fprintf (stderr, "Error:  I just *know* this is missing a variable somewhere:\n%s", orig_statement.aschar());
      exit (-1);
    }

    // Generate the verbiage.
    Dstr satisfies ("(:satisfies (");
    for (unsigned i1=0; i1<consequentargs.size(); i1++) {
      if (i1)
	satisfies += ' ';
      satisfies += "?FOO";
      satisfies += i1;
    }
    satisfies += ") (:for-some (";
    std::set<Dstr>::iterator it1 = antecedentvars.begin();
    while (it1 != antecedentvars.end()) {
      if (it1 != antecedentvars.begin())
	satisfies += ' ';
      satisfies += *it1;
      it1++;
    }
    satisfies += ") (:and\n  ";
    satisfies += antecedent;
    satisfies += "\n";
    std::vector<Dstr>::iterator it2 = consequentargs.begin();
    unsigned i2=0;
    while (it2 != consequentargs.end()) {
      satisfies += "  (= ?FOO";
      satisfies += i2++;
      satisfies += ' ';
      satisfies += *it2;
      satisfies += ")\n";
      it2++;
    }
    satisfies += ")))";

    out = "(implies ";
    out += satisfies;
    out += ' ';
    out += consequentfn;
    out += ")\n";
  }
}

// add_implication
// in:  implications, accumulated implications
//      newone, the new one
// out:  implications, with newone and possibly a debug printout added
void add_implication (Dstr &implications, Dstr &newone) {
#ifdef COUNT_BUGS
  implications += "(print '(bug ";
  implications += bug_count++;
  implications += "))\n";
#endif
  implications += newone;
}

// translate {slightly recursive}
// in:   buf, containing a single SUO-KIF statement.
//       body, accumulated miscellany for the "body" of the output.
//       defrelations, accumulated relation information.
//       defconcepts, accumulated concept information.
//       implications, accumulated implications
//       reject_invoked_toplevel, reject_invoked_anywhere,
//         reject_at_mere_mention, rejects lists of increasing
//         brutality.
//       accept_toplevel, overrides reject_invoked_anywhere at toplevel.
// out:  buf, contents indeterminate.
//       body, possibly with new stuff added.
//       defrelations, possibly with new defrelations added.
//       defconcepts, possibly with new concepts added.
//       implications, possibly with new stuff added.

void translate (Dstr &buf, Dstr &body, std::map<Dstr, rstruct *> &defrelations,
std::map<Dstr, cstruct *> &defconcepts, Dstr &implications,
std::map<Dstr, Dstr *> &reject_invoked_toplevel,
std::map<Dstr, Dstr *> &reject_invoked_anywhere,
std::map<Dstr, Dstr *> &reject_at_mere_mention,
std::set<Dstr> &accept_toplevel) {

  // -------------------------------------------------------------------
  // Blank lines and comments
  // -------------------------------------------------------------------

  if (buf[0] != '(') {
    body += buf;
    return;
  }

  // -------------------------------------------------------------------
  // Documentation -- must do this before unrolling ROW variables.
  // -------------------------------------------------------------------

  if (!strncmp (buf.aschar(), "(documentation", 14)) {
    do_renamings (buf);
    add_tell (buf, body);
    return;
  }

  Dstr buf_orig(buf), remainder(buf), arg0;
  remainder /= 1; // Skip paren
  remainder /= arg0;

  // -------------------------------------------------------------------
  // Detect rejects.
  // -------------------------------------------------------------------

  {
    if (const Dstr *r1 = find_words (buf, reject_at_mere_mention, false, false)) {
      comment_out (buf_orig);
      Dstr temp (";; sumo2loom is dropping the following statement because it mentions\n;; ");
      temp += *r1;
      temp += ", which ";
      temp += *(reject_at_mere_mention[*r1]);
      temp += ":\n";
      buf_orig *= temp;
      body += buf_orig;
      return;
    }

    // Tricky bit.  If the offender is on accept_toplevel, skip that
    // but look for other rejects.

    bool excepted = false;
    std::set<Dstr>::iterator at_it = accept_toplevel.begin();
    while (at_it != accept_toplevel.end()) {
      if (arg0 == *at_it) {
	excepted = true;
	break;
      }
      at_it++;
    }

    if (const Dstr *r2 = find_words (buf, reject_invoked_anywhere, true, excepted)) {
      comment_out (buf_orig);
      Dstr temp (";; sumo2loom is dropping the following statement because it uses\n;; ");
      temp += *r2;
      temp += ", which ";
      temp += *(reject_invoked_anywhere[*r2]);
      temp += ":\n";
      buf_orig *= temp;
      body += buf_orig;
      return;
    }

    std::map<Dstr, Dstr *>::iterator rt_it = reject_invoked_toplevel.begin();
    while (rt_it != reject_invoked_toplevel.end()) {
      if (arg0 == rt_it->first) {
	comment_out (buf_orig);
	Dstr temp (";; sumo2loom is dropping the following statement because it\n;; ");
	temp += *(rt_it->second);
	temp += ":\n";
	buf_orig *= temp;
	body += buf_orig;
	return;
      }
      rt_it++;
    }
  }

  // -------------------------------------------------------------------
  // Unroll @ROW variables.
  // This is the only recursion.
  // -------------------------------------------------------------------

  if (buf.strstr ("@ROW") != -1) {
#ifdef UNROLL_ROWS
    Dstr expansion, outbuf;
    for (int i=1; i<=ROW_VARIABLE_UNROLLING_LIMIT; i++) {
      if (i>1)
        expansion += ' ';
      expansion += "?ROW";
      expansion += i;
      Dstr temp(buf);
      temp.repstr ("@ROW", expansion);
      translate (temp, body, defrelations, defconcepts, implications, reject_invoked_toplevel, reject_invoked_anywhere, reject_at_mere_mention, accept_toplevel);
    }
#else
    comment_out (buf);
    buf *= ";; sumo2loom is skipping this statement because it contains @ROW.  Although\n;; @ROW could be unrolled, it usually appears in statements that use variable-\n;; arity predicates or other things that won't translate.\n";
    body += buf;
#endif
    return;
  }

  // -------------------------------------------------------------------
  // Do renamings.
  // -------------------------------------------------------------------

  do_renamings (buf);
  remainder = buf;
  remainder /= 1; // Skip paren
  remainder /= arg0;

  // -------------------------------------------------------------------
  // partition / disjointDecomposition
  // (exhaustiveDecomposition is not handled; the covering bit is
  // what's broken about disjointDecomposition)
  // -------------------------------------------------------------------

  if (arg0 == "partition" || arg0 == "sumodisjointDecomposition") {
    Dstr partitionee, tempnam, outbuf;
#ifdef DISJOINTS
    bool disjoint = (arg0[0] == 's');
#endif
    remainder /= partitionee;
    remainder.trim();
    assert (remainder.length() > 1);
    assert (remainder[remainder.length()-1] == ')');
    // Remainder now contains the list of partitions with ) at the end.
    cstruct *p = find_or_create (defconcepts, partitionee);
    genpsym (tempnam);
    outbuf = "    (";
    outbuf += tempnam;
    outbuf += " (";
    outbuf += remainder;
    outbuf += ")\n";
    // :exhaustive-partitions caused all sorts of trouble, so use a
    // regular partition and an implies instead.
    p->partitions += outbuf;
#ifdef DISJOINTS
    // Alas, this also causes all sorts of trouble.
    if (disjoint) {
      implications += "(implies ";
      implications += partitionee;
      implications += " (:or ";
      implications += remainder;
      implications += ")\n";
    }
#endif
    return;
  }

  // -------------------------------------------------------------------
  // disjointRelation
  // -------------------------------------------------------------------

  if (arg0 == "sumodisjointSumorelation") {
    std::set<Dstr> rels;
    Dstr arg;
    remainder /= arg;
    while (!arg.isNull()) {
      rels.insert (arg);
      remainder /= arg;
    }
    assert (rels.size() > 1);
    std::set<Dstr>::iterator it1 = rels.begin();
    // This is probably a waste of time anyway.  In simple tests, Loom
    // didn't seem to do anything about these implications.
    while (it1 != rels.end()) {
      std::set<Dstr>::iterator it2 = rels.begin();
      while (it2 != rels.end()) {
        if (it1 != it2) {
          Dstr temp ("(implies ");
	  temp += *it1;
	  temp += " (:satisfies (?x ?y) (:not (";
	  temp += *it2;
	  temp += " ?x ?y))))\n";
          add_implication (implications, temp);
        }
        it2++;
      }
      it1++;
    }
    return;
  }

  // -------------------------------------------------------------------
  // contraryAttribute, exhaustiveAttribute
  // -------------------------------------------------------------------

  // FIXME?

  // -------------------------------------------------------------------
  // Implications
  // -------------------------------------------------------------------

  if (arg0 == "=>") {
    Dstr arg1, arg2, temp;
    remainder /= arg1;
    remainder /= arg2;
    translate_arrow (arg1, arg2, temp, buf_orig);
    if (temp.isNull()) {
      comment_out (buf_orig);
      buf_orig *= ";; sumo2loom is skipping this implication because it has a nontrivial\n;; consequent:\n";
      body += buf_orig;
    } else
      add_implication (implications, temp);
    return;
  }
  if (arg0 == "<=>") {
    Dstr arg1, arg2, temp;
    bool firsthairy, secondhairy;
    remainder /= arg1;
    remainder /= arg2;
    translate_arrow (arg1, arg2, temp, buf_orig);
    firsthairy = temp.isNull();
    add_implication (implications, temp);
    translate_arrow (arg2, arg1, temp, buf_orig);
    secondhairy = temp.isNull();
    add_implication (implications, temp);

    if (firsthairy && secondhairy) {
      comment_out (buf_orig);
      buf_orig *= ";; sumo2loom is skipping this implication because both sides are nontrivial:\n";
      body += buf_orig;
    } else if (firsthairy || secondhairy) {
      comment_out (buf_orig);
      buf_orig *= ";; sumo2loom is only translating half of this implication because the other\n;; half has a nontrivial consequent:\n";
      body += buf_orig;
    }

    return;
  }

  // -------------------------------------------------------------------
  // Definitions (other than the variable-arity things handled previously)
  // -------------------------------------------------------------------

  // Some relations may pass through here more than once.

  if (arg0 == "instance") {
    Dstr arg1, arg2;
    remainder /= arg1;
    remainder /= arg2;
    if (arg2 == "BinarySumorelation" || arg2 == "BinaryPredicate" || arg2 == "UnarySumofunction"
	// Special cases:
	|| arg2 == "CaseRole"  // Is a subclass of BinaryPredicate.
	|| arg2 == "SpatialSumorelation") { // Has ambiguous arity.
      // Don't override arity for spatial relations.
      rstruct *r = find_or_create(defrelations,arg1);
      if (r->arity < 2)
	r->arity = 2;
    } else if (arg2 == "TernarySumorelation" || arg2 == "TernaryPredicate" || arg2 == "BinarySumofunction")
      find_or_create(defrelations,arg1)->arity = 3;
    else if (arg2 == "QuaternarySumorelation" || arg2 == "QuaternaryPredicate" || arg2 == "TernarySumofunction")
      find_or_create(defrelations,arg1)->arity = 4;
    else if (arg2 == "QuintarySumorelation" || arg2 == "QuintaryPredicate" || arg2 == "QuaternarySumofunction")
      find_or_create(defrelations,arg1)->arity = 5;

    else if (arg2 == "SymmetricSumorelation")
      find_or_create(defrelations,arg1)->symmetric = true;
    else if (arg2 == "CommutativeSumofunction")
      find_or_create(defrelations,arg1)->commutative = true;

    // Concepts
    else if (arg2 == "Sumoclass" || arg2 == "InheritableSumorelation" ||
    arg2 == "DeonticAttribute") {
      (void) find_or_create (defconcepts, arg1);
    }

    // Anyhow, declare the instances.
    // Special case for (instance ?THING Entity)
    if (arg2 != "Entity") {
      buf = "(";
      buf += arg2;
      buf += " ";
      buf += arg1;
      buf += ")\n";
      add_tell (buf, body);
    }

  } else if (arg0 == "subrelation") {
    Dstr arg1, arg2;
    remainder /= arg1;
    remainder /= arg2;
    rstruct *r1 = find_or_create (defrelations, arg1);
    add_parent (r1->isa, arg2, arg1);

  } else if (arg0 == "sumoinverse") {
    Dstr arg1, arg2;
    remainder /= arg1;
    remainder /= arg2;
    rstruct *r1 = find_or_create (defrelations, arg1);
    rstruct *r2 = find_or_create (defrelations, arg2);
    r1->inverse = arg2;
    r2->inverse = arg1;

  } else if (arg0 == "sumorange" || arg0 == "sumorangeSubclass") {
    Dstr arg1, arg2;
    remainder /= arg1;
    remainder /= arg2;
    rstruct *r1 = find_or_create (defrelations, arg1);
    do_additional_setfn_renamings (arg2);
    r1->range = arg2;

  } else if (arg0 == "sumodomain" || arg0 == "sumodomainSubclass") {
    Dstr arg1, arg2, arg3;
    unsigned parmnum;
    remainder /= arg1;
    remainder /= arg2;
    remainder /= arg3;

    if (!isdigit(arg2[0])) {
      fprintf (stderr, "Error:  syntax:  %s\n", buf.aschar());
      exit (-1);
    }

    parmnum = arg2.asunsigned();
    rstruct *r1 = find_or_create (defrelations, arg1);
    // This just causes problems with variable arity relations and functions.
    // if (parmnum > r1->arity)
    //   r1->arity = parmnum;
    Dstr filler (DOMAINFILLER);
    while (r1->domain.size() < parmnum+1)
      r1->domain.push_back (filler);
    do_additional_setfn_renamings (arg3);
    r1->domain[parmnum] = arg3;
  }

  else if (arg0 == "subclass") {
    Dstr arg1, arg2;
    remainder /= arg1;
    remainder /= arg2;

    // Don't want this:
    // (subclass (MinimalCutSetFn ?GRAPH) (CutSetFn ?GRAPH))
    // (they since took that out)
    if (arg1[0] != '(') {
      cstruct *p = find_or_create (defconcepts, arg1);
      add_parent (p->isa, arg2, arg1);
    } else {
      comment_out (buf);
      buf *= ";; FIXME:\n";
      body += buf;
    }

  } else if (arg0 == "sumodisjoint") {
    Dstr arg1, arg2, temp;
    remainder /= arg1;
    remainder /= arg2;
    if (remainder[0] != ')')
      fprintf (stderr, "Warning:  arity > 2 in disjoint not handled: %s", buf.aschar());
    temp = "(implies ";
    temp += arg1;
    temp += " (not ";
    temp += arg2;
    temp += "))\n";
    add_implication (implications, temp);
    temp = "(implies ";
    temp += arg2;
    temp += " (not ";
    temp += arg1;
    temp += "))\n";
    add_implication (implications, temp);

  // -------------------------------------------------------------------
  // Pass trivial propositions through.
  // -------------------------------------------------------------------
  } else
    add_tell (buf, body);
}

// flag_nonprimitive_concepts {recursive}
// in:   defconcepts, accumulated concept information.
//       cname, the top of a (sub)tree of abstract concepts.
// out:  defconcepts with primitive bools modified to try to prevent
//       incoherent concepts from happening.

void flag_nonprimitive_concepts (std::map<Dstr, cstruct *> &defconcepts,
const Dstr &cname) {

  // Special cases
  if (cname == "ObjectAttitude" || cname == "SumopropositionalAttitude")
    return;

  std::map<Dstr, cstruct *>::iterator it = defconcepts.begin();
  while (it != defconcepts.end()) {

    // Special cases
    if (it->first != "ObjectAttitude" && it->first != "SumopropositionalAttitude") {
      cstruct *c = it->second;
      std::set<Dstr>::iterator isa_it = c->isa.begin();
      while (isa_it != c->isa.end()) {
        if (*isa_it == cname) {
          if (c->isa.size() > 1)
            c->primitive = false;
	  flag_nonprimitive_concepts (defconcepts, it->first);
          break;
	}
        isa_it++;
      }
    }
    it++;
  }
}

// unsupported
// used to initialize the reject lists.
// in:   reject, name of unsupported function/relation/predicate.
//       rejects, the list to use.
//       reason, the excuse to give.
// out:  rejects, with new one added.
void unsupported (char *reject, std::map<Dstr, Dstr *> &rejects, const Dstr &reason_in) {
  Dstr *reason = new Dstr (reason_in);
  rejects[reject] = reason;
}

// main
// in:   SUO-KIF file specified by argv[1].
// out:  LOOM translation on standard output.

int main (int argc, char **argv) {
  if (argc != 2) {
    fprintf (stderr, "Usage:  sumo2loom input.kif > output.loom\n");
    exit (-1);
  }

  FILE *fp = fopen (argv[1], "r");
  if (!fp) {
    perror (argv[1]);
    exit (-1);
  }

  Dstr body, implications;
  std::map<Dstr, cstruct *> defconcepts;
  std::map<Dstr, rstruct *> defrelations;
  std::map<Dstr, Dstr *> reject_invoked_anywhere, reject_invoked_toplevel,
    reject_at_mere_mention;
  std::set<Dstr> accept_toplevel;

  // Initialize rejects and exceptions to rejects.
  unsupported ("equal", reject_invoked_toplevel, "wants to be an implication");
  unsupported ("exists", reject_invoked_toplevel, "wants to be an implication");
  unsupported ("not", reject_invoked_toplevel, "wants to be an implication");
  unsupported ("or", reject_invoked_toplevel, "wants to be an implication");
  unsupported ("initialList", reject_invoked_toplevel, "wants to be an implication");
  unsupported ("precondition", reject_invoked_anywhere, "sometimes takes propositions as arguments");
  unsupported ("connected", reject_at_mere_mention, "is linked to enigmatic Loom errors");
  {
    Dstr except ("disjointRelation");
    accept_toplevel.insert (except);
    except = "partition";
    accept_toplevel.insert (except);
    except = "disjointDecomposition";
    accept_toplevel.insert (except);
  }

  // We must make two passes because SUMO unfortunately uses things
  // before they are defined.

  for (unsigned pass = 1; pass <= 2; pass++) {
    enum statenum {Normal, Buffering, Processing, ProcessingEOF, Finished};
    statenum state = Normal;
    int parenlevel = 0;
    unsigned linenum=0, missingparenlinenum=0;
    Dstr buf, longbuf;

    rewind (fp);
    while (state != Finished) {
      assert (state == Normal || state == Buffering);
      buf.getline (fp);
      linenum++;
      if (buf.isNull()) {
	switch (state) {
	case Normal:
	  state = Finished;
	  break;
	case Buffering:
	  state = ProcessingEOF;
	  break;
	default:
	  assert (0);
	}
      } else {
	if (state == Normal)
	  missingparenlinenum = linenum;
	state = Buffering;
      }
      if (state == Buffering) {
	longbuf += buf;
	parenlevel += buf.parenbalance();
	if (parenlevel < 0) {
	  fprintf (stderr, "Error:  Too many close parens at line %u.\n", linenum);
	  fprintf (stderr, "Note:  This program does not recognize string quoting or comments.  All parens\nare significant.\n");
	  exit (-1);
	}
	if (!parenlevel)
	  state = Processing;
      }
      if (state == Processing || state == ProcessingEOF) {
        longbuf.ltrim();
        switch (pass) {
        case 1:
          // First pass: build list of rejects.
          note_rejects (longbuf, reject_invoked_anywhere);
          break;
        case 2:
          // Second pass: do everything else.
  	  translate (longbuf, body, defrelations, defconcepts, implications, reject_invoked_toplevel, reject_invoked_anywhere, reject_at_mere_mention, accept_toplevel);
          break;
        default:
          assert (0);
        }
        longbuf = "";
	if (state == ProcessingEOF) {
	  state = Finished;
	} else
	  state = Normal;
      }
    }

    if (parenlevel) {
      fprintf (stderr, "Error:  Unmatched parenthesis starting around line %u.\n", missingparenlinenum);
      fprintf (stderr, "Note:  This program does not recognize string quoting or comments.  All parens\nare significant.\n");
      exit (-1);
    }
  }

  {
    Dstr temp ("Abstract");
    flag_nonprimitive_concepts (defconcepts, temp);
  }

  // Resolve domains of subrelations where the parent's domain was
  // only partly overridden.
  {
    bool done;
    do {
      done = true;
      std::map<Dstr, rstruct *>::iterator it = defrelations.begin();
      while (it != defrelations.end()) {
        rstruct *r = it->second;
        size_t d_arity = r->domain.size();
        if (d_arity > 1) { // The zero element is unused.
          for (unsigned i=1; i<d_arity; i++) {
            if (r->domain[i] == DOMAINFILLER) {
              if (r->isa.size()) {
                rstruct *parent;
                if (r->isa.size() > 1)
                  fprintf (stderr, "Warning:  %s inherits domain from multiple parents -- choosing one.\n", it->first.aschar());
 		std::set<Dstr>::iterator isa_it = r->isa.begin();
                const Dstr &parent_name = *isa_it;
                parent = defrelations[parent_name];
                if (!parent) {
                  fprintf (stderr, "Error:  %s is subrelation of non-existent %s\n",
		     it->first.aschar(), parent_name.aschar());
                  exit (-1);
                }
                // The following assumption can be invalidated if the
                // overridden position was not the final one.  May
                // need to fix this at some point.  FIXME?
                assert (r->domain.size() == parent->domain.size());
                r->domain[i] = parent->domain[i];
                if (r->domain[i] == DOMAINFILLER)
                  done = false;
              }
            }
          }
	}
	it++;
      }
    } while (!done);
  }

  // Finished processing input.

  // Dollar sign (\044) obfuscated because we want the literal to
  // appear in the output and not get substituted when this source
  // file is checked in.
  printf (";; \044Id$\n;; This file was generated by sumo2loom.\n\n\
(make-package \"SUMO\")\n\
(in-package \"SUMO\")\n\
(loom:use-loom \"SUMO\")\n\
(defkb \"SUMO-KB\" nil :pathname \"/proj/sis/tools/loom/sumo-saved-kb\")\n\
(change-kb \"SUMO-KB\")\n\
(creation-policy :classified-instance)\n\
\n\
;; Sumo2Loom generates output in two parts.  The first part contains\n\
;; definitions that have been built up from possibly many Suo-Kif\n\
;; statements.  The second part contains the translation of the remainder\n\
;; of the Suo-Kif input, including commentary and asserted relations, in\n\
;; the same order that these appeared in the input.\n\
\n\
;; NOTICE:\n\
;; The output of Sumo2Loom is a modified derivative of its input.  For\n\
;; copyright and license terms, therefore, please refer to the commentary\n\
;; in Part 2, which is carried over from the input file.\n\
\n\
;; ----- SUMO2LOOM OUTPUT PART 1: DEFINITIONS AND IMPLICATIONS -----\n\n");

  {
    std::map<Dstr, cstruct *>::iterator it = defconcepts.begin();
    while (it != defconcepts.end()) {
      printf ("(defconcept %s", it->first.aschar());
      cstruct *c = it->second;
      if (!(c->isa.size()))
        printf (" :is-primitive Thing");
      else {
        if (c->primitive)
          printf (" :is-primitive");
        else
          printf (" :is");
        if (c->isa.size() > 1)
          printf (" (:and");
 	std::set<Dstr>::iterator isa_it = c->isa.begin();
        while (isa_it != c->isa.end()) {
          printf (" %s", isa_it->aschar());
          isa_it++;
        }
        if (c->isa.size() > 1)
          printf (")");
      }
      if (!(c->partitions.isNull()))
	printf ("\n  :partitions (\n%s  )", c->partitions.aschar());
      printf (")\n");
      it++;
    }
  }

  {
    bool flaggederror = false;
    std::map<Dstr, rstruct *>::iterator it = defrelations.begin();
    while (it != defrelations.end()) {
      printf ("(defrelation %s", it->first.aschar());
      rstruct *r = it->second;

      // The zero element is unused.
      size_t d_arity = r->domain.size();
      // SUMO:  relations only have domains.
      // LOOM:  the final arg to a relation is the range.
      // But if we only have one "domain" and no range leave it alone.
      if (r->range.isNull() && d_arity > 2) {
        r->range = r->domain.back();
        r->domain.pop_back();
        d_arity--;
      }

      // Loom defaults to arity 2 and gripes if you supply more than
      // that in the :domains and :range.
      if (r->arity == 0 && d_arity > 1)
        r->arity = d_arity;

      if (r->arity > 0)
        printf (" :arity %u", r->arity);

      if (r->isa.size()) {
        if (r->isa.size() > 1)
          printf (" :is (:and");
        else
          printf (" :is-primitive");
 	std::set<Dstr>::iterator isa_it = r->isa.begin();
        while (isa_it != r->isa.end()) {
          printf (" %s", isa_it->aschar());
          isa_it++;
        }
        if (r->isa.size() > 1)
          printf (")");
      }

      if (d_arity == 2) {
	printf (" :domain %s", r->domain[1].aschar());
        if (r->domain[1] == DOMAINFILLER)
          flaggederror = true;
      } else if (d_arity > 2) {
	printf (" :domains (");
	for (unsigned i=1; i<d_arity; i++) {
	  if (i>1)
	    printf (" ");
	  printf ("%s", r->domain[i].aschar());
          if (r->domain[i] == DOMAINFILLER)
            flaggederror = true;
	}
	printf (")");
      }
      if (!(r->range.isNull()))
        printf (" :range %s", r->range.aschar());
      if (!(r->inverse.isNull()))
        printf (" :inverse %s", r->inverse.aschar());
      if (r->symmetric || r->commutative) {
        printf (" :characteristics (");
        if (r->symmetric) {
          printf (":symmetric");
          if (r->commutative)
            printf (" ");
	}
        if (r->commutative)
          printf (":commutative");
        printf (")");
      }
      printf (")\n");
      it++;
    }
    if (flaggederror)
      fprintf (stderr, "Warning: at least one relation had a broken domain.  Check output.\n");
  }

  printf ("%s", implications.aschar());

  printf ("\n;; ----- SUMO2LOOM OUTPUT PART 2: COMMENTARY AND ASSERTIONS -----\n\n");

#ifdef ONE_BIG_TELL
  printf ("(tell\n%s)\n", body.aschar());
#else
  printf ("%s", body.aschar());
#endif

  exit (0);
}
