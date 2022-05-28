#include <iostream>
#include <sstream>
#include <fstream>
#include <fstream>
#include <vector>
#include <string.h>
#include <cstring>

#define nl std::cout<<"\n";

using namespace std;

void Usge(){
    nl nl
    std::cout << "Syntax- "; nl nl
    std::cout << "  Encrpytion:"; nl
    std::cout << "  [If Key == int]"; nl
    std::cout << "\t Enx --str <\"Word to encrypt\"> --keyN <key> +E"; nl
    std::cout << "  [If Key == Word]"; nl
    std::cout << "\t Enx --str <\"Word to encrypt\"> --keyA <key> +E"; nl nl
    std::cout << "  Decryption:"; nl
    std::cout << "  [If Key == int]"; nl
    std::cout << "\t Enx --str <\"Word to decrypt\"> --keyN <key> +D"; nl
    std::cout << "  [If Key == Word]"; nl
    std::cout << "\t Enx --str <\"Word to decrypt\"> --keyA <key> +D"; nl nl
}

int main(int argc, char* argv[])
{
    if(strcmp(argv[0],"Enx")==0 && strcmp(argv[1],"--help")==0){
        Usge();
    }

    else if(strcmp(argv[0],"Enx")==0 && strcmp(argv[1],"--str")==0 && strcmp(argv[3],"--keyN")==0 && strcmp(argv[5],"+E")==0)
    {

        long int C = sizeof(argv[2]);
        char St[C];
        strcpy(St, argv[2]);
        int key = atoi(argv[4]);
        for(int x = 0; x < C; x++){
            St[x] += key;
        }
        cout<<"Encrypted Text ~ "; nl nl
        cout<<St; nl nl
        cout<<"Key => "<<argv[4];
    }

    else if(strcmp(argv[0],"Enx")==0 && strcmp(argv[1],"--str")==0 && strcmp(argv[3],"--keyA")==0 && strcmp(argv[5],"+E")==0)
    {

        long int C = sizeof(argv[2]);
        char St[C];
        strcpy(St, argv[2]);

        int key = sizeof(argv[4]);
        for(int x = 0; x < C; x++){
            St[x] += key;
            //Out << St[x];
        }
        cout<<"Enx> Encrypted Text ~ "; nl
        cout<<St; nl
        cout<<"[+] With Key => "<<argv[4];
    }

    else if(strcmp(argv[0],"Enx")==0 && strcmp(argv[1],"--str")==0 && strcmp(argv[3],"--keyN")==0 && strcmp(argv[5],"+D")==0)
    {
        long int C = sizeof(argv[2]);
        char St[C];
        strcpy(St, argv[2]);
        int key = atoi(argv[4]);
        for(int x = 0; x < C; x++){
            St[x] -= key;
        }
        cout << "Enx> Decrypted Text ~ "; nl nl
        cout<<St; nl nl
        cout << "[+] With Key => "<<argv[4];
    }

    else if(strcmp(argv[0],"Enx")==0 && strcmp(argv[1],"--str")==0 && strcmp(argv[3],"--keyA")==0 && strcmp(argv[5],"+D")==0)
    {

        long int C = sizeof(argv[2]);
        char St[C];
        strcpy(St, argv[2]);
        int key = sizeof(argv[4]);
        for(int x = 0; x < C; x++){
            St[x] -= key;
        }
        cout<< "Enx> Decrypted Text ~ "; nl nl
        cout<<St; nl nl
        cout<<"[+] With Key => "<<argv[4];
    }
nl nl     return 0;
}
