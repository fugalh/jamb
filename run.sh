set -e
tup linux/jamb
{
	sleep 1
	aconnect -l
} &
exec linux/jamb
