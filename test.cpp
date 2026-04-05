#include <iostream>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

using namespace std;

class Student
{
public:
    string name;
    int age;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & name;
        ar & age;
    }
};

int main()
{
    Student s1;
    s1.name = "Nam";
    s1.age = 19;

    {
        ofstream ofs("student.txt");
        boost::archive::text_oarchive oa(ofs);
        oa << s1;
    }

    Student s2;

    {
        ifstream ifs("student.txt");
        boost::archive::text_iarchive ia(ifs);
        ia >> s2;
    }

    cout << s2.name << endl;
    cout << s2.age << endl;
}