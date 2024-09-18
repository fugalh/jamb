if [ `uname` = 'Linux' ]; then
  set -ex
  tup test/jamb
  test/jamb
else
  buck2 test //...
fi
