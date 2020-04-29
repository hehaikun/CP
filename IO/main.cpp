#include <iostream>
#include <fstream>

using namespace std;
int main()
{
    ifstream fio("text");
    string str;
    while(!fio.eof())
    {
        //fio >> str;       //使用 >> 每次读取一个单词
        getline(fio, str);  //一次读取一行的数据
        cout << str << endl;
    }
    
    return 0;
}