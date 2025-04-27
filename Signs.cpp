//
// Created by prw on 4/26/25.
//
#include <iostream>
#include <chrono>
#include <random>

int main()
{

    std::random_device rd;
    std::mt19937 gen(rd());

    // Generate a random integer between 1 and 10
    std::uniform_int_distribution<> distrib(-1000, 1000);
    int sz = 100000;
    std::vector<int> v(sz);

    std::iota(v.begin(), v.end(), (sz/2)*-1);
    std::shuffle(v.begin(), v.end(), gen);
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    long ie_dur{};
    long tern_dur{};
    long su1_dur{};
    long opp_signs{};


    int gt{0};
    int iter_count{100};

    for (int iter = 0; iter < iter_count; ++iter)
    {
        start = std::chrono::high_resolution_clock::now();
        for (auto i : v)
        {
            if (i >= 0)
                gt = 1;
            else
                gt = -1;
        }

        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        ie_dur += duration.count();


        std::shuffle(v.begin(), v.end(), gen);


        start = std::chrono::high_resolution_clock::now();
        for (auto i : v)
        {
            gt = i >=0 ? 1 : -1;
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        tern_dur += duration.count();

        std::shuffle(v.begin(), v.end(), gen);
        start = std::chrono::high_resolution_clock::now();
        for (auto i : v)
        {
            gt = -(i<0);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        su1_dur += duration.count();

        std::shuffle(v.begin(), v.end(), gen);

        start = std::chrono::high_resolution_clock::now();
        for (auto i : v)
        {
            gt = ((1^i)<0);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        opp_signs += duration.count();
    }

    std::cout << "If/Else: " << static_cast<double>(ie_dur)/iter_count<< std::endl;
    std::cout << "Tern: " << static_cast<double>(tern_dur)/iter_count<< std::endl;
    std::cout << "SpeedUp1: " << static_cast<double>(su1_dur)/iter_count<< std::endl;
    std::cout << "OppSigns1: " << static_cast<double>(opp_signs)/iter_count<< std::endl;


    return 0;
}
