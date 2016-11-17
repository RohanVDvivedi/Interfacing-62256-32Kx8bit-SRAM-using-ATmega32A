#include <iostream>
#include <SerialClass.h>  // search in my github repository serial-communication-setup
#include <thread>
#include <mutex>

using namespace std;

Serial s( ((char*)"COM4"),4800);   // i am using my microcontroller on COM4
int i=1;

enum acc{read,write};                     // to define access to the communication class
mutex mt;                                 // globally declaring the mutex
static class communication
{
private :
    int i=1;                             // i=0 means the child thread has to exit,
                                         //it continue until parent thread writes 1 to it
    int readi()
    {
        return i;
    }
    int writei(int u)
    {
        return (i=u);
    }
public:
    void access(acc access,int* result)     // i can be set or cleared only by this method
    {
        mt.lock();
        // lock the mutes to prevent the other thread from accessing the element i
        switch (access)
        {
            case read :
                *result = readi();
                break;
            case write :
                writei(*result);
                break;
        }
        // unlock
        mt.unlock();
    }
} commu;

// main thread in the code takes care of the communication string sent to the microcontroller
// child thread calls the receive function to show the string that has been sent from the microcontroller

void receive()
{
    char f[100];int n;int res;
    commu.access(read,&res);
    while(res)
    {
        Sleep(550);
        if((n=s.ReadData(f,100))>0)
        {
            cout<<"microcontroller : ";
            for(int i=0;i<n;i++)
            {
                cout<<f[i];
            }
            cout<<endl<<"Admin : ";
        }
        commu.access(read,&res);
    }
}

int main()
{
    char c[100];char temp='a';
    unsigned int i=0;
    if(!(s.IsConnected()))
    {
        return 0;
    }
    cout<<"Admin : ";
    thread t(receive);
    while(true)
    {
        i=0;
        while(temp!='\n')
        {
            temp=getchar();
            if(temp==':')   // sending : will exit the program
            {
                goto Exit;
            }
            c[i++]=temp;
        }
        i--;
        s.WriteData(c,i);  // the collected character array at the end on \n will be sent to the microcontroller
        temp='a';
    }
    Exit:
    int a=0;
    commu.access(write,&a);  // to terminate the child thread loop
    t.join();                // join the child thread and exit the program
    return 0;
}
