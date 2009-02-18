#!/bin/bash
set -o errexit
tmpdir=`mktemp -d`
testDataDir=$1
for t in `\ls $testDataDir/*.in`
do
    echo "test: $t"
    testBase=`basename $t .in`
    outFile=$testBase.out
    ./scan < $t | tee $tmpdir/$outFile
    diff $testDataDir/$outFile $tmpdir/$outFile
done
rm -rf $tmpdir
echo "ALL TESTS PASSED"
