#!/bin/bash

for i in $(find . -name "*.h" -o -name "*.hh" -o -name "*.cpp" -o -name "*.cc") ;
do
	sed -i -e 's/(\s*DEBUG\s*,/(EVT_DEBUG,/g' $i
	sed -i -e 's/(\s*INFO\s*,/(EVT_INFO,/g' $i
	sed -i -e 's/(\s*WARNING\s*,/(EVT_WARNING,/g' $i
	sed -i -e 's/(\s*NOTICE\s*,/(EVT_NOTICE,/g' $i
	sed -i -e 's/(\s*ERROR\s*,/(EVT_ERROR,/g' $i
	sed -i -e 's/(\s*CRITICAL\s*,/(EVT_CRITICAL,/g' $i
	sed -i -e 's/(\s*ALERT\s*,/(EVT_ALERT,/g' $i
	sed -i -e 's/(\s*EMERGENCY\s*,/(EVT_EMERGENCY,/g' $i
done

for i in $(find . -name EvtReport.hh) ; do
	sed -i -e 's/^\s*DEBUG\s*/  EVT_DEBUG/g' $i
	sed -i -e 's/^\s*INFO\s*,/  EVT_INFO,/g' $i
	sed -i -e 's/^\s*WARNING\s*,/  EVT_WARNING,/g' $i
	sed -i -e 's/^\s*NOTICE\s*,/  EVT_NOTICE,/g' $i
	sed -i -e 's/^\s*ERROR\s*,/  EVT_ERROR,/g' $i
	sed -i -e 's/^\s*CRITICAL\s*,/  EVT_CRITICAL,/g' $i
	sed -i -e 's/^\s*ALERT\s*,/  EVT_ALERT,/g' $i
	sed -i -e 's/^\s*EMERGENCY\s*,/  EVT_EMERGENCY,/g' $i
done
