#!/bin/bash
set -o errexit
tmpdir=`mktemp -d`
testDataDir=$1
for t in `find $testDataDir -type f -name "*.in" | sort`
do
    echo -n "$t ... "
    testBase=`basename $t .in`
    outFile=$testBase.out
    ./main $t > $tmpdir/$outFile
    diff $testDataDir/$outFile $tmpdir/$outFile
    echo "OK"
done
rm -rf $tmpdir
echo "ALL TESTS PASSED"
