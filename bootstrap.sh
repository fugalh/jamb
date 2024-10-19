if [ ! -d .tup ]; then
    mkdir -p build build/test build/linux bin
    tup init
    { echo "[updater]"; echo "num_jobs=1"; } >> .tup/options
    echo "bootstrapped tup"
else
    echo "tup already bootstrapped"
fi
if [ `uname` = 'Darwin' ]; then
    buck2 clean
fi
