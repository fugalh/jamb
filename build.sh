if [ `uname` = Linux ]; then
  tup linux/jamb
else
  buck2 build //...
fi