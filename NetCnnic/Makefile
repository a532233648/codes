
all: 
	mkdir -p ./output
	#make -C src/NetGather
	make -C src/plugin
	make -C gather_controls

clean:
	rm -rf ./output/bin/
	rm -rf ./output/plugin/
	#make clean -C src/NetGather
	make clean -C src/plugin
	make clean -C gather_controls

