#include <cstdlib>
#include <iostream>

class Mem {
public:
    Mem() : data(0) { std::cout << "constructed\n"; }
    Mem(Mem& m) {
        data = m.data + 1;
        std::cout << "copied\n";
    }
    Mem(Mem&& m) {
        data = m.data + 2;
        std::cout << "moved\n";
    }
    ~Mem() { std::cout << "destructed: " << data << "\n"; }

    void foo() { std::cout << "foo: " << data << "\n"; }

private:
    int data;
};

int main(int argc, const char* argv[]) {
    Mem m1;
    Mem m2(m1);
    Mem m3(std::move(m1));

    m1.foo();
    m2.foo();
    m3.foo();

    return EXIT_SUCCESS;
}
