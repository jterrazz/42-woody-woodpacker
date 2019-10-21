#!/bin/sh
if [ -z $1 ];
then echo "param is unset, seting default path to /bin"; path="/bin";
else echo "path is set to '$1'"; path=$1;
fi

if [ -z $2 ];
then echo "param is unset, seting default coder to JBE"; encoder="./jbe_encode"; decoder="./jbe_decode";
else echo "coder is set to '$2'";
case "$2" in
	jbe)
		echo "JBE code";
		encoder="./jbe_encode"; decoder="./jbe_decode";
		;;

	rle)
		echo "RLE code";
		encoder="./rle_encode"; decoder="./rle_decode";
		;;

	mtf)
		echo "MTF code";
		encoder="./mtf_encode"; decoder="./mtf_decode";
		;;
	*)
		echo "Usage: $0 path {jbe|rle|mtf}"
		exit 1
esac
fi

find $path -name "*" -type f -exec $encoder {} .tmp_enc \; -exec $decoder .tmp_enc .tmp_dec \; -exec diff {} .tmp_dec \;
exit 0
