#pragma once

// Criware shit because idk where else to put it rn

struct MemoryRouter {
    int64_t moduleAllocator; //MemoryRouterAllocator*
    int64_t debugAllocator; //MemoryRouterAllocator*
};

struct InplaceTempUri {
    int64_t vftable;
    const char* buffer;
    int count;
    int capacity;
    int64_t allocator;
    char bufferr[128];
};

struct Locale {
    int localeId{ 1 };
    const char* localeName{ "" };
};

struct ResourceLoader {
    int64_t data[0x25];
};
