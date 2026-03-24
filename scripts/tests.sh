#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 app_name [filters]"
  echo "filters: if provided, will be passed as --gtest_filter to the test binary"
  exit 1
fi

APP_NAME=$1

FILTERS=""
if [ $# -ge 2 ]; then
  FILTERS="--gtest_filter=$2"
fi

REPEAT=""
if [ $# -ge 3 ]; then
  REPEAT="--gtest_repeat=$3"
fi

./bin/${APP_NAME} ${FILTERS} ${REPEAT}
