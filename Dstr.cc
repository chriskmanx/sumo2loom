// $Id: Dstr.cc,v 1.6 2002/10/16 13:25:16 dflater Exp $
// Dstr:  Dave's String class.

// This source is shared among several of my projects and is public
// domain.  Operations are added when needed, changed when convenient
// and deleted when they become more trouble than they are worth.
// There is not necessarily a canonical version.

#include "header.hh"

Dstr::Dstr () {
  theBuffer = NULL;
}

Dstr::Dstr (const char *val) {
  if (val) {
    theBuffer = strdup (val);
    used = strlen (val);
    max = used + 1;
  } else
    theBuffer = NULL;
}

Dstr::Dstr (char val) {
  char t[2];
  t[0] = val;
  t[1] = '\0';
  theBuffer = strdup (t);
  used = 1;
  max = 2;
}

Dstr::Dstr (const Dstr &val) {
  if (!(val.isNull())) {
    theBuffer = val.asdupchar();
    used = val.length();
    max = used + 1;
  } else
    theBuffer = NULL;
}

Dstr::~Dstr () {
  if (theBuffer)
    free (theBuffer);
}

Dstr&
Dstr::getline (FILE *fp) {
  char buf[512], *ret;
  if (!(ret = fgets (buf, 512, fp)))
    (*this) = (char *)NULL;
  else {
    (*this) = "";
    while (ret) {
      (*this) += buf;
      if (used > 0)
        if (theBuffer[used-1] == '\n') {
          // (*this) -= used-1;
          break;
        }
      ret = fgets (buf, 512, fp);
    }
  }
  return (*this);
}

void Dstr::getline (Dstr &line_out) {
  line_out = (char *)NULL;
  while (((*this).length() > 0) && (*this)[0] != '\n') {
    line_out += (*this)[0];
    (*this) /= 1;
  }
  if ((*this)[0] == '\n')
    (*this) /= 1;
}

// Scan a string like fscanf (fp, "%s")
Dstr&
Dstr::scan (FILE *fp) {
  int c;
  (*this) = (char *)NULL;
  // Skip whitespace
  do {
    c = getc(fp);
    if (c == EOF)
      return (*this);
  } while (isspace (c));
  // Get the string
  (*this) = (char)c;
  while (1) {
    c = getc(fp);
    if (c == EOF)
      return (*this);
    if (isspace (c))
      return (*this);
    (*this) += (char)c;
  }
}

Dstr &
Dstr::pad (unsigned to_length) {
  while (length() < to_length)
    (*this) += " ";
  return (*this);
}

Dstr &Dstr::trim () {
  while (isspace ((*this)[0]))
    (*this) /= 1;
  while (isspace ((*this)[this->length()-1]))
    (*this) -= (this->length()-1);
  return (*this);
}

Dstr &Dstr::ltrim () {
  while (isspace ((*this)[0]) && (*this)[0] != '\n')
    (*this) /= 1;
  return (*this);
}

Dstr&
Dstr::operator-= (unsigned at_index) {
  if (theBuffer) {
    if (at_index < used) {
      theBuffer[at_index] = '\0';
      used = at_index;
    }
  }
  return (*this);
}

int
Dstr::strchr (char val) const {
  if (!theBuffer)
    return -1;
  char *c = ::strchr (theBuffer, val);
  if (!c)
    return -1;
  return (c - theBuffer);
}

int
Dstr::strrchr (char val) const {
  if (!theBuffer)
    return -1;
  char *c = ::strrchr (theBuffer, val);
  if (!c)
    return -1;
  return (c - theBuffer);
}

int
Dstr::strstr (const Dstr &val) const {
  if (!theBuffer)
    return -1;
  if (!val.theBuffer)
    return -1;
  char *c = ::strstr (theBuffer, val.theBuffer);
  if (!c)
    return -1;
  return (c - theBuffer);
}

int
Dstr::strstr (const char *val) const {
  Dstr temp (val);
  return strstr (temp);
}

Dstr &
Dstr::operator= (const char *val) {
  if (theBuffer)
    free (theBuffer);
  if (val) {
    theBuffer = strdup (val);
    used = strlen (val);
    max = used + 1;
  } else
    theBuffer = NULL;
  return (*this);
}

Dstr&
Dstr::operator= (char val) {
  char t[2];
  t[0] = val;
  t[1] = '\0';
  (*this) = t;
  return (*this);
}

Dstr &
Dstr::operator= (const Dstr &val) {
  (*this) = val.theBuffer;
  return (*this);
}

Dstr&
Dstr::operator+= (const char *val) {
  if (val) {
    if (!theBuffer)
      (*this) = val;
    else {
      unsigned l;
      if ((l = strlen (val))) {
        while (l + used >= max) {  // Leave room for terminator
          // Expand
          max *= 2;
          assert (theBuffer = (char *) realloc (theBuffer, max*sizeof(char)));
        }
        strcpy (theBuffer+used, val);
        used += l;
      }
    }
  }
  return (*this);
}

Dstr&
Dstr::operator+= (char val) {
  char t[2];
  t[0] = val;
  t[1] = '\0';
  (*this) += t;
  return (*this);
}

Dstr&
Dstr::operator+= (const Dstr &val) {
  (*this) += val.theBuffer;
  return (*this);
}

Dstr&
Dstr::operator+= (int val) {
  char t[80];
  sprintf (t, "%d", val);
  (*this) += t;
  return (*this);
}

Dstr&
Dstr::operator+= (unsigned int val) {
  char t[80];
  sprintf (t, "%u", val);
  (*this) += t;
  return (*this);
}

Dstr&
Dstr::operator+= (long int val) {
  char t[80];
  sprintf (t, "%ld", val);
  (*this) += t;
  return (*this);
}

Dstr&
Dstr::operator+= (long unsigned int val) {
  char t[80];
  sprintf (t, "%lu", val);
  (*this) += t;
  return (*this);
}

Dstr&
Dstr::operator+= (double val) {
  char t[80];
  sprintf (t, "%f", val);
  (*this) += t;
  return (*this);
}

Dstr&
Dstr::operator*= (const char *val) {
  Dstr temp (*this);
  (*this) = val;
  (*this) += temp;
  return (*this);
}

Dstr&
Dstr::operator*= (char val) {
  Dstr temp (*this);
  (*this) = val;
  (*this) += temp;
  return (*this);
}

Dstr&
Dstr::operator*= (const Dstr &val) {
  Dstr temp (*this);
  (*this) = val;
  (*this) += temp;
  return (*this);
}

Dstr&
Dstr::operator/= (unsigned at_index) {
  if (theBuffer) {
    Dstr temp ((*this).ascharfrom(at_index));
    (*this) = temp;
  }
  return (*this);
}

Dstr&
Dstr::operator/= (Dstr &val) {
  val = (char *)NULL;
  if (theBuffer) {
    // Eat whitespace
    while (used > 0 && isspace ((*this)[0]))
      (*this) /= 1;
    // Anything left?
    if (used == 0) {
      // Nothing left.
      (*this) = (char *)NULL;
    } else {
      if ((*this)[0] == '(') {
        // List
        val += '(';
        (*this) /= 1;
        int parenbalance = 1;
        while (used > 0 && parenbalance > 0) {
          char c = (*this)[0];
          val += c;
          (*this) /= 1;
          if (c == '(')
            parenbalance++;
          else if (c == ')')
            parenbalance--;
        }
      } else {
        // Atom or whatever
        while (used > 0 && (*this)[0] != ')' && (*this)[0] != '(' && (!(isspace ((*this)[0])))) {
          val += (*this)[0];
          (*this) /= 1;
        }
      }
    }
  }
  return (*this);
}

char
Dstr::operator[] (unsigned at_index) const {
  if (!theBuffer)
    return '\0';
  if (at_index >= used)
    return '\0';
  return theBuffer[at_index];
}

unsigned
Dstr::repchar (char X, char Y) {
  unsigned i, repcount=0;
  for (i=0; i<length(); i++)
    if (theBuffer[i] == X) {
      theBuffer[i] = Y;
      repcount++;
    }
  return repcount;
}

int Dstr::parenbalance () const {
  unsigned i;
  int count=0;
  for (i=0; i<length(); i++)
    if (theBuffer[i] == '(')
      count++;
    else if (theBuffer[i] == ')')
      count--;
  return count;
}

unsigned Dstr::recursiverepstr (const char *X, const char *Y) {
  unsigned repcount=0;
  int i;
  assert (X);
  assert (Y);
  while ((i = this->strstr (X)) > -1) {
    Dstr temp(*this);
    temp -= i;
    temp += Y;
    temp += this->ascharfrom (i+strlen(X));
    (*this) = temp;
    assert (repcount++ < 100); // Break infinite loops
  }
  return repcount;
}

unsigned Dstr::repstr (const char *X, const char *Y) {
  unsigned repcount=0;
  int i;
  assert (X);
  assert (Y);
  Dstr remainder(*this);
  *this = "";
  while ((i = remainder.strstr (X)) > -1) {
    Dstr temp(remainder);
    temp -= i;
    *this += temp;
    *this += Y;
    remainder /= i+strlen(X);
  }
  (*this) += remainder;
  return repcount;
}

unsigned Dstr::repstr (const char *X, const Dstr &Y) {
  return repstr (X, Y.aschar());
}

unsigned
Dstr::length () const {
  if (!theBuffer)
    return 0;
  return used;
}

int
Dstr::isNull () const {
  if (theBuffer)
    return 0;
  return 1;
}

double
Dstr::asdouble() const {
  double t;
  if (sscanf (aschar(), "%lf", &t) != 1)
    fprintf (stderr, "Warning:  illegal conversion of Dstr to double\n");
  return t;
}

unsigned Dstr::asunsigned() const {
  unsigned t;
  if (sscanf (aschar(), "%u", &t) != 1)
    fprintf (stderr, "Warning:  illegal conversion of Dstr to unsigned: '%s'\n", aschar());
  return t;
}

char *
Dstr::aschar () const {
  if (theBuffer)
    return theBuffer;
  return "";
}

char *
Dstr::asdupchar() const {
  return strdup (aschar());
}

char *
Dstr::ascharfrom(unsigned from_index) const {
  if (!theBuffer)
    return "";
  if (from_index >= used)
    return "";
  return theBuffer + from_index;
}

char *
Dstr::asrawchar () const {
  return theBuffer;
}

int operator== (const Dstr &val1, const char *val2) {
  if ((!val2) && (val1.isNull()))
    return 1;
  if ((!val2) || (val1.isNull()))
    return 0;
  return (!(strcmp (val1.aschar(), val2)));
}

int operator== (const char *val1, const Dstr &val2) {
  return (val2 == val1);
}

int operator== (const Dstr &val1, const Dstr &val2) {
  return (val1 == val2.asrawchar());
}

int operator!= (const char *val1, const Dstr &val2) {
  return (!(val1 == val2));
}

int operator!= (const Dstr &val1, const char *val2) {
  return (!(val1 == val2));
}

int operator!= (const Dstr &val1, const Dstr &val2) {
  return (!(val1 == val2));
}

bool operator< (const Dstr &val1, const Dstr &val2) {
  if (val1.isNull() || val2.isNull())
    return false;
  return (strcmp (val1.asrawchar(), val2.asrawchar()) < 0);
}

Dstr &
Dstr::lowercase() {
  unsigned i;
  for (i=0; i<length(); i++)
    theBuffer[i] = tolower(theBuffer[i]);
  return (*this);
}
