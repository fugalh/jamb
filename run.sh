if [ `uname` = 'Linux' ]; then
	set -ex
	tup linux/jamb
	{
		sleep 1
		aconnect -l
	} &
	exec linux/jamb
else
	buck2 run //:jamb
fi
