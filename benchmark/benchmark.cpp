#include <benchmark/benchmark.h>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include <iostream>
#include "database.h"
#include "util.h"

using namespace naivedb;



static void Write(benchmark::State &state) {
    Database database(std::tmpnam(nullptr));
    const int KeySize = 16, ValueSize = 100;
    while (state.KeepRunning()) {
        for (int i = 0; i < state.range_x(); i++) {
            state.PauseTiming();
            const std::string key = generateRandomString(KeySize - 1), value = generateRandomString(ValueSize);
            state.ResumeTiming();

            database.set(key, value.c_str(), ValueSize, true);
        }
    }
    state.SetBytesProcessed((KeySize + ValueSize) * state.iterations() * state.range_x());
    state.SetItemsProcessed(state.iterations() * state.range_x());
}

static void Read(benchmark::State &state) {
    static bool setup = false;
    static Database database(std::tmpnam(nullptr));

    const int KeySize = 16, ValueSize = 100;
    static std::map<std::string ,std::string> items;

    if(!setup){

        for(int i = 0;i < 1000000; i++){
            const std::string key = generateRandomString(KeySize - 1), value = generateRandomString(ValueSize);
            items[key] = value;
            database.set(key, value.c_str(), ValueSize, true);
        }
        setup = true;
    }

    char buf[ValueSize];
    while(state.KeepRunning()) {
        for (int i = 0; i < state.range_x(); i++) {
            state.PauseTiming();
            auto it = items.begin();
            std::advance(it, rand() % items.size());
            const std::string key = it->first, value = it->second;
            state.ResumeTiming();

            database.get(key, buf);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range_x());
    state.SetBytesProcessed((KeySize+ValueSize) * state.iterations() * state.range_x());
}

static void Remove(benchmark::State &state) {

}

BENCHMARK(Write)->Range(2, 1<<15);
BENCHMARK(Read)->Range(2, 1<<15);

BENCHMARK_MAIN();