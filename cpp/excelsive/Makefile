OUT=excelsive
default:
	g++ main.cpp schedule.cpp \
    -std=c++20 \
    -I/home/zack/Downloads/libxl-4.6.0/include_cpp \
    -L/home/zack/Downloads/libxl-4.6.0/lib-aarch64 \
    -Wl,-rpath,/home/zack/Downloads/libxl-4.6.0/lib-aarch64 \
    -lxl \
    -o $(OUT)
	./$(OUT) m:1pm-5pm,t:11:15am-3pm,w:off,th:9am-12pm,f:1pm-6pm

clean:
	rm -rf build $(OUT)
