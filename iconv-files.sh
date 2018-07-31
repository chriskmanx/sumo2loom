#!/bin/bash

cd /Users/chris/sumo

echo "Converting UTF-8 to ISO_8859-1..."
for i in $( ls *.kif); do
    echo "Processing file $i"
#    /usr/bin/iconv -c -f UTF8 -t ISO_8859-1//TRANSLIT $i >  /Users/chris/sumoloom/$i.ISO_8859
    cp $i /Users/chris/sumoloom/$i.ISO_8859
done



