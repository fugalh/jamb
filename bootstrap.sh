if [ ! -d .tup ]; then
	tup init
        { echo "[updater]"; echo "num_jobs=1"; } >> .tup/options
	echo "bootstrapped"
else
	echo "already bootstrapped"
fi
