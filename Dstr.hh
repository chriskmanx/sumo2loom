// $Id: Dstr.hh,v 1.4 2002/09/24 19:42:33 dflater Exp $
// Dstr:  Dave's String class.

// This source is shared among several of my projects and is public
// domain.  Operations are added when needed, changed when convenient
// and deleted when they become more trouble than they are worth.
// There is not necessarily a canonical version.

class Dstr {
public:
  Dstr();
  Dstr(const char *val);
  Dstr(char val);
  Dstr(const Dstr &val);
  ~Dstr();

  unsigned length() const;
  int isNull() const;

  // Read a line.  On EOF, Dstr becomes null.
  Dstr& getline (FILE *fp);
  // Scan a string like fscanf (fp, "%s")
  Dstr& scan (FILE *fp);
  // Scan a line from a Dstr, stripping newline.
  void getline (Dstr &line_out);

  // Assign
  Dstr& operator= (const char *val);
  Dstr& operator= (char val);
  Dstr& operator= (const Dstr &val);

  // Append
  Dstr& operator+= (const char *val);
  Dstr& operator+= (char val);
  Dstr& operator+= (const Dstr &val);
  Dstr& operator+= (int val);
  Dstr& operator+= (unsigned int val);
  Dstr& operator+= (long int val);
  Dstr& operator+= (long unsigned int val);
  Dstr& operator+= (double val);

  // Prepend
  Dstr& operator*= (const char *val);
  Dstr& operator*= (char val);
  Dstr& operator*= (const Dstr &val);

  // Truncate
  Dstr& operator-= (unsigned at_index);

  // Break off the first substring delimited by whitespace or parens
  // and assign it to val.  The parens are NOT removed, and if the
  // argument is terminated by the end-of-line rather than a matching
  // paren, you'll get the unbalanced parens back.
  Dstr& operator/= (Dstr &val);

  // Remove all text before the specified index
  Dstr& operator/= (unsigned at_index);

  // Convert contents to a double.
  double asdouble() const;
  // Convert contents to an unsigned.
  unsigned asunsigned() const;

  // Get index; returns -1 if not found
  int strchr (char val) const;
  int strrchr (char val) const;
  int strstr (const Dstr &val) const;
  int strstr (const char *val) const;

  // Get character at index
  char operator[] (unsigned at_index) const;

  // Return "paren balance" of string,
  // count of open parens - count of close parens
  int parenbalance() const;

  // Pad to length with spaces.
  Dstr &pad (unsigned to_length);
  // Strip leading and trailing whitespace.
  Dstr &trim ();
  // Strip only leading whitespace.  Stop at newlines.
  Dstr &ltrim ();

  // Retrieve value as character string.  This will actually be theBuffer
  // unless it's NULL -- in which case an empty string will be
  // substituted.
  char *aschar() const;
  // Same thing, but strdup'd
  char *asdupchar() const;
  // Same thing, but starting at index.
  char *ascharfrom(unsigned from_index) const;
  // Retrieve value as a character string, no NULL masking.
  char *asrawchar() const;

  // Replace all instances of character X with character Y; returns number
  // of reps.
  unsigned repchar (char X, char Y);

  // Replace all instances of substring X with substring Y, making
  // only a single pass through the string.  Returns number of reps.
  unsigned repstr (const char *X, const char *Y);
  unsigned repstr (const char *X, const Dstr &Y);

  // Replace all instances of substring X with substring Y, even if
  // substring X resulted from a previous replacement.  Returns number
  // of reps.
  unsigned recursiverepstr (const char *X, const char *Y);

  // Smash case
  Dstr &lowercase();

protected:
  char *theBuffer;
  unsigned max;   // Total max buffer size including \0
  unsigned used;  // Length not including \0
};

// Compare
int operator== (const Dstr &val1, const char *val2);
int operator== (const char *val1, const Dstr &val2);
int operator== (const Dstr &val1, const Dstr &val2);
int operator!= (const char *val1, const Dstr &val2);
int operator!= (const Dstr &val1, const char *val2);
int operator!= (const Dstr &val1, const Dstr &val2);

// This was wanted by the map template.
bool operator< (const Dstr &val1, const Dstr &val2);
