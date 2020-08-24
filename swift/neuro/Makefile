
SWIFT := swift
APP := neuro

default: build

build:
	$(SWIFT) build -c release --static-swift-stdlib
	-cp .build/x86_64-apple-macosx/release/$(APP) ./$(APP) 

# https://github.com/BiAtoms/Socket.swift/issues/12
linux:
	#export LD_LIBRARY_PATH='/usr/local/lib:/usr/local/opt/libressl/lib:$LD_LIBRARY_PATH'
	#LD_LIBRARY_PATH='/usr/local/lib:/usr/local/opt/libressl/lib:$LD_LIBRARY_PATH' swift build -c release --static-swift-stdlib

run:
	$(SWIFT) run 

docker:
	docker build -t zackb/neuro:latest .
	# docker push zackb/neuro:latest

clean:
	swift package clean
	rm -f $(APP)
