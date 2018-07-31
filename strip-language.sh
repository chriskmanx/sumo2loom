#!/bin/bash
export LC_CTYPE=C
export LANG=C

if [ "$#" -ne 2 ]; then
   echo "Usage: strip-language.sh <file-name> <language>"
   exit 1
fi

#echo "Pruning file $1, removing language: $2"
echo "Pruning file $1..."

export LANGUAGE_TOKEN=$2Language

#echo "Language token: $LANGUAGE_TOKEN"

# 1st SED
# Pull simple foreign language one liner documentation strings
# [A-Za-z ^(]* means a grouping of zero of more (*) characters consisting
#              range A-Z, or a range a-z or a space but not (^) an open paren
#              then match language token followed by zero or more non quotes
#              followed by exactly one quote followed by zero or more non
#              quotes ( the document string ) followed by one quote

# Pull foreign language multi line documentation strings

# 2nd SED
# Start of match is the same up to opening quote
# comma spreads across lines
# second clause says match closing quote followed by closing paren
# that's the end of the documentation string which is duly hidden
# by the comma.

# 3rd SED
# &% -> remove spurious pattern



cat $1 | sed 's/(documentation[A-Za-z ^(]*ChineseLanguage[^"]*\"[^"]*\")//' |\
         sed '/(documentation[A-Za-z ^(]*ChineseLanguage[^"]*\"/,/\")/d' |\
         sed 's/&%//g' |\
         sed  '/(documentation[A-Za-z ^(]*EnglishLanguage[^"]*\"[^"]*\")/ { s/(e.g.[^"]*\"/\.\"/g; }' |\
         sed  '/(documentation[A-Za-z ^(]*EnglishLanguage[^"]*\"[^"]*\")/ { s/(normally rectangular region/normally rectangular region/; }' |\
         sed  '/(documentation[A-Za-z ^(]*EnglishLanguage[^"]*\"[^"]*\")/ { s/(but not in Ireland/but not in Ireland/; }' > $1.kif2



