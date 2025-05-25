OUT=excelsive
default:
	g++ main.cpp \
    -std=c++20 \
    -I/home/zack/Downloads/libxl-4.6.0/include_cpp \
    -L/home/zack/Downloads/libxl-4.6.0/lib-aarch64 \
    -Wl,-rpath,/home/zack/Downloads/libxl-4.6.0/lib-aarch64 \
    -lxl \
    -o $(OUT)

clean:
	rm -rf build $(OUT)
