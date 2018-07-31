#!/bin/bash

cd /Users/chris/sumoloom

# Pre-process various files with misplaced parens
cat english_format.kif.ISO_8859 |\
    sed "s/^; The \'format\' ternary predicate associates a concept (either relation or/; The \'format\' ternary predicate associates a concept either relation or/" > /tmp/out.txt
cp -f /tmp/out.txt english_format.kif.ISO_8859

cat naics.kif.ISO_8859 |\
    sed "s/^Services Allied to Motion Picture Production (except casting bureaus,/Services Allied to Motion Picture Production except casting bureaus,/" > /tmp/out.txt
cp -f /tmp/out.txt naics.kif.ISO_8859

cat domainEnglishFormat.kif.ISO_8859 |\
    sed "s/^;;termFormat EnglishLanguage RawFood \"raw food\")/;;termFormat EnglishLanguage RawFood \"raw food\"/" > /tmp/out.txt
cp -f /tmp/out.txt domainEnglishFormat.kif.ISO_8859

cat Food.kif.ISO_8859 |\
    sed "s/^(documentation ingredientAmount EnglishLanguage \"(\&%ingredientAmount ?ING ?FOOD ?AMT/(documentation ingredientAmount EnglishLanguage \"\&%ingredientAmount ?ING ?FOOD ?AMT/" > /tmp/out.txt
cp -f /tmp/out.txt Food.kif.ISO_8859



cat Mid-level-ontology.kif.ISO_8859 |\
    sed "s/^;; a.) Street Number (can contain letters)/;; a.] Street Number (can contain letters) --> postStreetNumber from Media.kif/" |\
    sed "s/^;; b.) Floor Code (can contain letters)/;; b.] Floor Code (can contain letters) --> floorCode/" |\
    sed "s/^;; c.) Unit Number (can contain letters)/;; c.] Unit Number (can contain letters) --> unitNumber/" |\
    sed "s/^;; d.) Street Name/;; d.] Street Name/" |\
    sed "s/^;; e.) Subdivision/;; e.] Subdivision/" |\
    sed "s/^;; f.) City/;; f.] City/" |\
    sed "s/^;; g.) State/;; g.] State/" |\
    sed "s/^;; h.) Country/;; h.] Country/" |\
    sed "s/^;; i.) Post Office Box Number/;; i.] Post Office Box Number/" |\
    sed "s/^;; j.) Building Name/;; j.] Building Name/" |\
    sed "s/^;; k.) Province Code/;; k.] Province Code/" |\
    sed "s/^;; l.) Postal Code (can contain letters)/;; l.] Postal Code (can contain letters)/" |\
    sed "s/^;; Initially modeled Contract (which is an enforcaeble agreement as a subclass of/;; Initially modeled Contract which is an enforcaeble agreement as a subclass of/" |\
    sed "s/^;; notion of pulling (like 'pulling the trigger' instead of being specifically/;; notion of pulling like 'pulling the trigger' instead of being specifically/" |\
    sed "s/^;;For example, (\&%orientation \&%Cuba \&%UnitedStates \&%Outside).\")/;;For example, (\&%orientation \&%Cuba \&%UnitedStates \&%Outside).\"/" |\
    sed "s/^;;For example, (\&%orientation \&%Virginia \&%UnitedStates \&%Inside).\")/;;For example, (\&%orientation \&%Virginia \&%UnitedStates \&%Inside).\"/" > /tmp/out.txt
cp -f /tmp/out.txt Mid-level-ontology.kif.ISO_8859

                                                                    
cat UXExperimentalTerms.kif.ISO_8859 |\
    sed "s/is the same as the end of the process (i.e. when WebListing began to be listed./is the same as the end of the process i.e. when WebListing began to be listed./" |\
    sed "s/(documentation prohibitedItem EnglishLanguage \"(prohibitedItem ?Object ?SITE means that ?ITEM is/(documentation prohibitedItem EnglishLanguage \"prohibitedItem ?Object ?SITE means that ?ITEM is/" |\
    sed "s/(documentation qualifiedExperiment EnglishLanguage \"(qualifiedExperiment ?EXPERIMENT ?HUMAN means/(documentation qualifiedExperiment EnglishLanguage \"qualifiedExperiment ?EXPERIMENT ?HUMAN means/" |\
    sed "s/Human (i.e. not a dealership.\")/Human i.e. not a dealership.\")/" > /tmp/out.txt
cp -f /tmp/out.txt UXExperimentalTerms.kif.ISO_8859

cat Transportation.kif.ISO_8859 |\
    sed "s/^Great Britain, and most of Western Europe (but not in Ireland,/Great Britain, and most of Western Europe but not in Ireland,/" |\
    sed "s/^TexDOT), or the type of vehicles or travellers accommodated (e.g., /TexDOT), or the type of vehicles or travellers accommodated e.g., /" > /tmp/out.txt
cp -f /tmp/out.txt Transportation.kif.ISO_8859

cat Dining.kif.ISO_8859 |\
    sed "s/^;; a view represents some physical thing (could be a process, such as \"view of the/;; a view represents some physical thing could be a process, such as \"view of the/"  > /tmp/out.txt
cp -f /tmp/out.txt Dining.kif.ISO_8859

cat Media.kif.ISO_8859 |\
    sed "s/^;; (i.e., the grooves of a vinyl record, the gold or aluminu/;; i.e., the grooves of a vinyl record, the gold or aluminum/"  > /tmp/out.txt
cp -f /tmp/out.txt Media.kif.ISO_8859

                                                        
cat Geography.kif.ISO_8859 |\
    sed "s/^\&%Angola (\&%RecurringTimeIntervalFn \&%May \&%October\")/\&%Angola \&%RecurringTimeIntervalFn \&%May \&%October\")/" |\
    sed "s/^during the \&%TimeInterval ?INTERVAL.  For example, (\&%coolSeasonInArea/during the \&%TimeInterval ?INTERVAL.  For example, \&%coolSeasonInArea/"  > /tmp/out.txt
cp -f /tmp/out.txt Geography.kif.ISO_8859


cat Merge.kif.ISO_8859 |\
    sed "s/^;; (exists ... inside the scope of holdsDuring./;; exists ... inside the scope of holdsDuring./" |\
    sed "s/^;; NS: delete.  It's best not to put either (exists ... or (not/;; NS: delete.  It's best not to put either exists ... or not/"  > /tmp/out.txt
cp -f /tmp/out.txt Merge.kif.ISO_8859
 
cat Music.kif.ISO_8859 |\
    sed "s/albumArtist (i.e. there exists two different artists/albumArtist i.e. there exists two different artists/"  > /tmp/out.txt
cp -f /tmp/out.txt Music.kif.ISO_8859
  
       
cat MilitaryDevices.kif.ISO_8859 |\
    sed "s/^to the M2 BMG.  2) Lengths of rounds were repeatedly pulled from the 100/to the M2 BMG.  2] Lengths of rounds were repeatedly pulled from the 100/" |\
    sed "s/^equipment.  3) Lack of a flash suppressor, which made this weapon almost/equipment.  3] Lack of a flash suppressor, which made this weapon almost/" |\
    sed "s/^Marines in 2001 included: 1) Significantly increased reload times compared/Marines in 2001 included: 1] Significantly increased reload times compared/" |\
    sed "s/^and 3) a significantly longer barrel life.  The first difference means/and 3] a significantly longer barrel life.  The first difference means/" |\
    sed "s/^1) The new weapon uses an open bolt 2) has an internal recoil spring,/1] The new weapon uses an open bolt 2] has an internal recoil spring,/"  > /tmp/out.txt
cp -f /tmp/out.txt MilitaryDevices.kif.ISO_8859

                                                             
cat Languages.kif.ISO_8859 |\
    sed "s/^639-2: cpp. Population: 5,000 (1998 S. and T. Graham. Region: Angolar is/639-2: cpp. Population: 5,000 1998 S. and T. Graham. Region: Angolar is/" |\
    sed "s/^possible dialect spoken by the Haratine (former slaves of the Ouargli people./possible dialect spoken by the Haratine former slaves of the Ouargli people./" |\
    sed "s/^(Bloomfield 1933). Extinct. Bible 520. Also spoken in: Bulgaria. (Language/(Bloomfield 1933). Extinct. Bible 520. Also spoken in: Bulgaria. Language/" |\
    sed "s/^minorities.' (I. Hancock). Also spoken in: Australia. (Language name:/minorities.' (I. Hancock). Also spoken in: Australia. Language name:/" |\
    sed "s/^mother tongue speakers. Bible 1478-1993. Also spoken in: Italy. (Language/mother tongue speakers. Bible 1478-1993. Also spoken in: Italy. Language/" |\
    sed "s/^TV. Christian, secular. Bible 1478-1993. Also spoken in: Andorra. (Language/TV. Christian, secular. Bible 1478-1993. Also spoken in: Andorra. Language/" |\
    sed "s/^do not speak the language. Bible 1879. Also spoken in: Canada. (Language name:/do not speak the language. Bible 1879. Also spoken in: Canada. Language name:/" |\
    sed "s/^Muslim, Christian. Bible 1933-1983. Also spoken in: Botswana. (Language name:/Muslim, Christian. Bible 1933-1983. Also spoken in: Botswana. Language name:/" |\
    sed "s/^bananas, tubers, tobacco. NT 1984. Also spoken in: Brazil. (Language name:/bananas, tubers, tobacco. NT 1984. Also spoken in: Brazil. Language name:/" |\
    sed "s/^various Hebrew loan words. Jewish. Also spoken in: Georgia. (Language name:/various Hebrew loan words. Jewish. Also spoken in: Georgia. Language name:/" |\
    sed "s/^language. SOV. Riverine. NT 1982. Also spoken in: Colombia. (Language name:/language. SOV. Riverine. NT 1982. Also spoken in: Colombia. Language name:/" |\
    sed "s/^agriculturalists. lower than 200 meters. Also spoken in: Venezuela. (Language/agriculturalists. lower than 200 meters. Also spoken in: Venezuela. Language/" |\
    sed "s/^group (1993 Ramon D. Rivas. Alternate names: SUMO, SUMU, SOUMO, SUMOO/group 1993 Ramon D. Rivas. Alternate names: SUMO, SUMU, SOUMO, SUMOO/" |\
    sed "s/^NOCTENES, BOLIVIAN 'MATACO'. Also spoken in: Argentina. (Language name: WICHI/NOCTENES, BOLIVIAN 'MATACO'. Also spoken in: Argentina. Language name: WICHI/" |\
    sed "s/^(Wopolu I, Wopolu II (Nokonufa), Kauvia (Kawiya), Yonuwai, all on hills rising/Wopolu I, Wopolu II (Nokonufa), Kauvia (Kawiya), Yonuwai, all on hills rising/" |\
    sed "s/^Toulousse. Bible portions 1824-1975. Also spoken in: Italy. (Language name:/Toulousse. Bible portions 1824-1975. Also spoken in: Italy. Language name:/" |\
    sed "s/^Most occasions. Bible portions 1830. Also spoken in: Switzerland. (Language/Most occasions. Bible portions 1830. Also spoken in: Switzerland. Language/" |\
    sed "s/^Italian. Not endangered. Bible portions 1860. Also spoken in: France. (Language/Italian. Not endangered. Bible portions 1860. Also spoken in: France. Language/" |\
    sed "s/^Comments: Bible portions 1860. Also spoken in: Monaco. (Language name:/Comments: Bible portions 1860. Also spoken in: Monaco. Language name:/" |\
    sed "s/^(Language name: BALOCHI, WESTERN. Population: 200,000 in Afghanistan (1979/Language name: BALOCHI, WESTERN. Population: 200,000 in Afghanistan (1979/" |\
    sed "s/^Emirates. (Language name: PASHTO, NORTHERN. Population: 100,000 in UAE (1986)./Emirates. Language name: PASHTO, NORTHERN. Population: 100,000 in UAE (1986)./" |\
    sed "s/^grain, cotton, peanuts. Christian. NT 1987. Also spoken in: USA. (Language/grain, cotton, peanuts. Christian. NT 1987. Also spoken in: USA. Language/" |\
    sed "s/^Standard German. Christian. NT 1987. Also spoken in: Costa Rica. (Language/Standard German. Christian. NT 1987. Also spoken in: Costa Rica. Language/" |\
    sed "s/They are 20% monolingual in Qawasqar (O. Aguilera/They are 20% monolingual in Qawasqar O. Aguilera/" |\
    sed "s/^NT 1981. Also spoken in: Malaysia (Sabah). (Language name: CHAVACANO./NT 1981. Also spoken in: Malaysia (Sabah). Language name: CHAVACANO./" > /tmp/out.txt
cp -f /tmp/out.txt Languages.kif.ISO_8859





echo "Pruning foreign language doc strings:"
echo "Result files are postfixed with .kif2..."
for i in $( ls *.kif.ISO_8859); do
    /Users/chris/sumo2loom/strip-language.sh $i Chinese
done



