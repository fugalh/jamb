if [ ! -d .tup ]; then
	tup init
	echo -e "[updater]\nnum_jobs=1" >> .tup/options
	echo "bootstrapped"
else
	echo "already bootstrapped"
fi
