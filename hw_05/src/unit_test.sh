#!/bin/bash
exitCode=0
failures=0
totalTests=0
tmpdir=`mktemp -d`
testDataDir=$1
spim_output_head="SPIM Version 7.4 of January 1, 2009\nCopyright 1990-2004 by James R. Larus (larus@cs.wisc.edu).\nAll Rights Reserved.\nSee the file README for a full copyright notice.\nLoaded: /usr/lib/spim/exceptions.s"
for t in `find $testDataDir -type f -name "*.in" | sort`
do
    totalTests=$(($totalTests + 1))
    echo -n "$t ... "
    testDir=`dirname $t`
    testBase=`basename $t .in`
    expectedCompilationOutput=$testBase.out
    rm -f out.s

    # test that parsing/compilation doesn't segfault and matches expected output
    ./main $t > $tmpdir/$expectedCompilationOutput
    diff $testDir/$expectedCompilationOutput $tmpdir/$expectedCompilationOutput
    testResult=$?
    if [ $testResult -ne 0 ]
    then
        exitCode=1
        failures=$(($failures + 1))
        echo Expected: $testDir/$expectedCompilationOutput
        echo "FAIL: compilation output mismatch"
        continue
    fi

    simpleExpectedExecutionOutput=$testBase.out2
    if [ -e $testDir/$simpleExpectedExecutionOutput ]
    then
        actualExecutionOutput=$tmpdir/spimout
        rm -f $actualExecutionOutput
        expectedExecutionOutput=$tmpdir/expected
        echo -e "$spim_output_head" > $expectedExecutionOutput
        cat $testDir/$simpleExpectedExecutionOutput >> $expectedExecutionOutput
        executionInput=$testDir/$testBase.stdin
        # test that assembled C-- code produces expected output when run in SPIM
        if [ -e $executionInput ]
        then
            spim out.s > $actualExecutionOutput < $executionInput
        else
            spim out.s > $actualExecutionOutput
        fi
        diff $expectedExecutionOutput $actualExecutionOutput
        testResult=$?
        if [ $testResult -ne 0 ]
        then
            exitCode=1
            failures=$(($failures + 1))
            echo "FAIL: execution output mismatch"
            continue
        fi
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
