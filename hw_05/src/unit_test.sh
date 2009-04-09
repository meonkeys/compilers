#!/bin/bash
exitCode=0
failures=0
totalTests=0
tmpdir=`mktemp -d`
testDataDir=$1
for t in `find $testDataDir -type f -name "*.in" | sort`
do
    totalTests=$(($totalTests + 1))
    echo -n "$t ... "
    testDir=`dirname $t`
    testBase=`basename $t .in`
    outFile=$testBase.out
    ./main $t > $tmpdir/$outFile
    # testResult=$?
    # if [ $testResult -ne 0 ]
    # then
    #     exitCode=1
    #     failures=$(($failures + 1))
    #     echo "FAIL"
    #     continue
    # fi
    diff $testDir/$outFile $tmpdir/$outFile
    testResult=$?
    if [ $testResult -ne 0 ]
    then
        exitCode=1
        failures=$(($failures + 1))
        echo Expected: $testDir/$outFile
        echo "FAIL"
        continue
    fi
    echo "OK"
done
rm -rf $tmpdir
if [ $exitCode -eq 0 ]
then
    echo "$totalTests TESTS PASSED"
else
    echo "$failures TESTS FAILED OUT OF $totalTests"
fi
exit $exitCode
