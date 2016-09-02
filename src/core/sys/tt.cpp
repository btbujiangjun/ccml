#include <iostream>       // std::cout
#include <thread>         // std::thread
 
void foo(){
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cout << 1 << std::endl;
}
void bar(int x){
    std::cout << x << std::endl;
}
int main(){
    std::thread first (foo);     // spawn new thread that calls foo(\<Right>
    std::thread second (bar,10);  // spawn new thread that calls bar(0\<Right>
    std::cout << "main, foo and bar now execute concurrently...\n";
    first.join();                // pauses until first finishes
    second.join();               // pauses until second finishes
    std::cout << "foo and bar completed.\n";
    return 0;
}
