OUT=airpods

default: build

build:
	gcc -o $(OUT) airpods.c -lbluetooth

run: build
	sudo ./$(OUT)
