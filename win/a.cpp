#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

class Animal
{
public:
    virtual void speak()
    {
        cout << "动物在说话" << endl;
    }

    virtual void eat(int a)
    {
        cout << "动物在吃饭" << endl;
    }
};

class Cat : public Animal
{
public:
    void speak()
    {
        cout << "小猫在说话" << endl;
    }

    void eat(int a)
    {
        cout << "小猫在吃饭" << endl;
    }
};

class Dog : public Animal
{
public:
    void speak()
    {
        cout << "小狗在说话" << endl;
    }
};

void doSpeak(Animal &animal) // Animal & animal = cat;
{
    // 如果地址早就绑定好了，地址早绑定，属于静态联编
    // 如果想调用小猫说话，这个时候函数的地址就不能早就绑定好，而是在运行阶段再去绑定函数地址，属于地址晚绑定，叫动态联编
    animal.speak();
}

void test01()
{
    Cat cat;
    doSpeak(cat);

    Dog dog;
    doSpeak(dog);

    Animal animal;
    doSpeak(animal);
}

int main(void)
{
    test01();
    return 0;
}
