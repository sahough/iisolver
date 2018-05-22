main:
	mpic++ -std=gnu++11 -static-libstdc++ -O3 \
	-I ./boost/boost_1_61_0 \
	-o iisolver code/main.cpp \
	./boost/boost_1_61_0/stage/lib/libboost_serialization.a

rmdata:
	rm -r data/*.dat
