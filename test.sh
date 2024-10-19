if [ `uname` = 'Linux' ]; then
  set -ex
  tup bin/test
  bin/test
else
  buck2 test //...
fi
