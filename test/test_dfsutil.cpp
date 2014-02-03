

#include <iostream>

using namespace std;

int OriStr_selfTest(void);
int OriUtil_selfTest(void);
int OriFile_selfTest(void);
int LRUCache_selfTest(void);
int KVSerializer_selfTest(void);
int OriCrypt_selfTest(void);
int Key_selfTest(void);

int main(int argc, const char *argv[])
{
    int result = 0;
    result += OriStr_selfTest();
    result += OriUtil_selfTest();
    result += OriFile_selfTest();
    result += LRUCache_selfTest();
    result += KVSerializer_selfTest();
    result += OriCrypt_selfTest();
    //result += Key_selfTest();

    if (result == 0) {
        cout << "All tests passed!" << endl;
    } else {
        cout << -result << " errors occurred." << endl;
    }

    return 0;
}

