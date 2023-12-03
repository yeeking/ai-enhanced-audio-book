#include <thread>
#include <iostream>
#include <chrono>
#include "MarkovManager.h"

std::thread* thread1;
std::thread* thread2;
MarkovManager man;

void setupMarkov()
{
    for (auto i=0;i<100;i++)
    {   

        man.putEvent(std::to_string(i% 101) + std::to_string(i));
    }
}

void poll(std::string msg)
{
    std::cout << msg << " polls " << std::endl;
}

void updater(int id)
{
    int i{0};
    //for (auto i=0;i<10000;i++)
    while(true)
    {   
        i = i % 1000;
        poll("update start");
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        man.putEvent(std::to_string(id) + std::to_string(i));
        man.getEvent();
        poll("update end");
        
    }
}
void resetter(int id)
{
    //for (auto i=0;i<10000;i++)
    while(true)
    {   
        poll("reset start");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        man.reset();  
        setupMarkov();
        poll("reset en=======");
        
    }
}




int main()
{
    setupMarkov ();
    thread1 = new std::thread(updater, 1);
    thread2 = new std::thread(resetter, 2);


    
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    thread1->join();
    thread2->join();

    delete thread1; 
    delete thread2;
      
}