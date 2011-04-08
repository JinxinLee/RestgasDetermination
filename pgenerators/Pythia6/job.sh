
echo compilation...

g77 -O $1.f  \
$SIMPATH/generators/pythia6/pythia6319.o \
-o $1.exe

echo ++++++++++++++COMPILATION FINISHED++++++++++++++++++
