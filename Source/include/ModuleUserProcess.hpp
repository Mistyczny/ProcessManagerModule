#pragma once

class ModuleUserProcess {
private:
public:
    /* Use this to initialize environment before running main loop */
    ModuleUserProcess();
    /* Use this to deinitialize environment before module terminating */
    virtual ~ModuleUserProcess();
    /* Main of your program */
    int main(int argc, char* argv[]);
};