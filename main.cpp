#include <QCoreApplication>

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <cstdio>

using namespace std;

struct Data{     // TODO: modify the struct to contain the real data
    // TODO: put real data here
    char *data;
    // default constructor
    Data(){ data=NULL; }
    // constructor
    Data(char *_data):data(_data){}
    // copy constructor
    Data(const Data &x){
        int n=strlen(x.data);
        data=new char[n+1];
       for(int i=0;i<=n;i++)
           data[i]=x.data[i];
    }
    // move constructor
    Data(Data &&x){
        data=x.data;
        x.data=NULL;
    }
    // move operator
    Data & operator = (Data &&x){
        data=x.data;
        x.data=NULL;
        return *this;
    }

    bool isEmpty() const { return data==NULL; }

    void show() const {
        printf("%s\n", data);  // for DEBUG
    }
    ~Data(){
        if(data!=NULL)
            delete [] data;
    }
};

class Manager{

    bool finished;
    int dataid;

    queue<Data> tasks;
    mutex Mutex;
    condition_variable cond;

    Data readData(){                      // TODO: modify the function to read the real data
        dataid++;
        if(dataid==9) return Data();   // End Of Data
        char *tmp=new char [2];
        tmp[0]=char(dataid+'0');
        tmp[1]='\0';
        Data res(tmp);
        return res;
    }

    void doSomethings(const Data &d){  // TODO: replace the function "doSomethings()" to process the data
        d.show();
    }

    // sub thread function
    function<void()> process=[&]{
        while(true){
            unique_lock<mutex> lock(Mutex);
            if(finished&&(tasks.empty()))
            {
                printf("end!\n");
                break;
            }
            if(!finished)
                cond.wait(lock);        // sleep and wait for notification from main thread
            while(!tasks.empty()){      // when not empty, process the data in the queue pushed by main thread
                if(!lock.owns_lock()) lock.lock();
                Data x=tasks.front();   // copy construction - copy data from the queue to x
                tasks.pop();
                lock.unlock();          // there is no need to lock the queue when processing data, so unlock
                doSomethings(x);
            }
        }
    };


public:

    void run(){
        dataid=-1;
        finished=false;
        Data x;
        thread processor(process);
        while(!((x=readData()).isEmpty())){
            {
                lock_guard<mutex> lock(Mutex);
                tasks.push(move(x));
            }
            cond.notify_all();
            // notify sub thread to process data after sending data to the global queue
        }
        finished=true;
        cond.notify_all();
        processor.join();
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Manager m,n;
    m.run();
    n.run();
    return 0;
}
