#!/bin/bash
set -o errexit
tmpdir=`mktemp -d`
#echo "tmpdir: $tmpdir"
for t in `\ls tests/*.in`
do
    echo "test: $t"
    testBase=`basename $t .in`
    outFile=$testBase.out
    ./scan < $t | tee $tmpdir/$outFile
    diff tests/$outFile $tmpdir/$outFile
done
rm -rf $tmpdir
echo "ALL TESTS PASSED"
