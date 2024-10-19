# Run this on a fresh checkout to prepare the build
if [ ! -d .tup ]; then
    tup init
    { echo "[updater]"; echo "num_jobs=1"; } >> .tup/options
    echo "bootstrapped tup"
else
    echo "tup already bootstrapped"
fi
if [ `uname` = 'Darwin' ]; then
    buck2 kill
fi
