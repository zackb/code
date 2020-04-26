
SWIFT := swift
APP := neuro

default: build

build:
	$(SWIFT) build -c release
	cp .build/x86_64-apple-macosx/release/$(APP) ./$(APP)

run:
	$(SWIFT) run 

clean:
	swift package clean
	rm -f $(APP)
