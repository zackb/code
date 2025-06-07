#include <cstdlib>
#include <iostream>
#include <vector>

class Mem {
public:
    Mem() : data(0) { std::cout << "constructed\n"; }
    Mem(int d) : data(d) { std::cout << "arg constructed: " << data << "\n"; }
    Mem(Mem& m) {
        data = m.data + 1;
        std::cout << "copied: " << data << "\n";
    }
    Mem(Mem&& m) {
        data = m.data + 2;
        std::cout << "moved: " << data << "\n";
    }
    ~Mem() { std::cout << "destructed: " << data << "\n"; }

    void foo() { std::cout << "foo: " << data << "\n"; }

    static std::vector<Mem> createVec() {
        std::vector<Mem> v;

        v.emplace_back(100);
        v.push_back(200);
        return v;
    }

    int data;
};

int main(int argc, const char* argv[]) {
    Mem m1;
    Mem m2(m1);
    Mem m3(std::move(m1));

    m1.foo();
    m2.foo();
    m3.foo();
    std::cout << "---------------------------------\n";

    std::vector<Mem> v;
    v.push_back(10);
    std::cout << "---------------------------------\n";

    auto vv = Mem::createVec();

    // arg constructed = 200
    // moved into vector = 201
    // copied here = 203
    auto copied = vv.at(1); // copy constructor
    auto& ref = vv.at(1);   // no constructor
    auto&& moved = vv.at(1);
    std::cout << "copy =  " << copied.data << "\n"; // 203
    std::cout << "ref = " << ref.data << "\n";      // 202
    std::cout << "moved = " << moved.data << "\n";  // 202

    return EXIT_SUCCESS;
}
