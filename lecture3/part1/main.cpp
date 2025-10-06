#include <iostream>
using namespace std;

class Material {
public:
    Material();
    ~Material();
    void print();

private:
    int count_;
};

Material::Material() : count_(0)
{
    cout << "Material Default Constructor!" << endl;
}

Material::~Material()
{
    cout << "Material Destructor!It has been read " << count_ << " times" << endl;
}

void Material::print()
{
    count_++;
    cout << "This is a Material object!" << endl;
}

int main()
{
    cout << "--- 构造函数 ---" << endl;
    Material m;

    cout << "\n--- print函数 ---" << endl;
    m.print();

    cout << "\n--- 自动进行析构函数 ---" << endl;
    return 0;
}
