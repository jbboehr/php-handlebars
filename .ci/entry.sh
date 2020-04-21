#!/usr/bin/env bash

set -e -o pipefail

function print_test_outputs() {
    for i in `find tests -name "*.out" 2>/dev/null`; do
        echo "-- START ${i}";
        cat ${i};
        echo "-- END";
    done
    for i in `find tests -name "*.mem" 2>/dev/null`; do
        echo "-- START ${i}";
        cat ${i};
        echo "-- END";
    done
}

trap "print_test_outputs" ERR

make test

exit 0
