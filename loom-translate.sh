#!/bin/bash

cd /Users/chris/sumoloom

echo "Translating to PowerLoom KIF:"
echo "Result files are postfixed with .loom"
for i in $( ls *.kif.ISO_8859.kif2); do
    echo "Process $i...plm"
    /Users/chris/sumo2loom/sumo2loom $i > $i.plm
done


