@ECHO off
IF NOT EXIST Darmstadt.osm.pbf (
	wget --no-check-certificate https://download.bbbike.org/osm/bbbike/Darmstadt/Darmstadt.osm.pbf
) ELSE (
	ECHO "Darmstadt.osm.pbf" already exists.
)
PAUSE
