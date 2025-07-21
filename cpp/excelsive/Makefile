OUT=excelsive
default:
	g++ main.cpp schedule.cpp \
    -std=c++20 \
    -I/home/zackb/Downloads/libxl-4.6.0/include_cpp \
    -L/home/zackb/Downloads/libxl-4.6.0/lib64 \
    -Wl,-rpath,/home/zackb/Downloads/libxl-4.6.0/lib64 \
    -lxl \
    -o $(OUT)
	./$(OUT) m:1pm-5pm,t:11:15am-3pm,w:off,th:9am-12pm,f:1pm-6pm,sa:9am-12pm,su:1:30pm-5:30pm

clean:
	rm -rf build $(OUT)
