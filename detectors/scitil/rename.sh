#! /bin/sh
# usage: rename.sh old new
FILELIST='*.cxx *.h *.C CMakeLists.txt'
#echo $FILELIST
if [ $# -ne 2 ] ; then
echo 'args: "oldstring" "newstring"'
else
echo Replace $1 by $2 in
echo "*.cxx *.cpp *.h CMakeLists.txt"
for FILENAME in $FILELIST;
do
#echo $FILENAME;
cat  $FILENAME | awk -v rep=$2 -v str=$1 '{ gsub(str,rep) ; print }' > temp.txt
mv temp.txt $FILENAME;
NEWNAME=`echo $FILENAME | awk -v rep=$2 -v str=$1 '{ gsub(str,rep) ; print }'`;
echo $NEWNAME;
mv $FILENAME $NEWNAME;
done
fi

