#Script for updating files that use PndTpc-classes to the new naming convention
#Use in the folder where the files to be changed are


#!/bin/bash

mkdir fixed

for file in *
do

echo $file
sed 's/PndTpc/PndPndTpc/g' $file >"fixed/"$file

done

echo "All done. Updated files have been written to the 'fixed' directory.
Backup your old files and test everything please."