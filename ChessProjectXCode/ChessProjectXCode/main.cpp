#include <iostream>
#include "UIManager.h"
#include "globalFunctions.h"

int main() {
    UIManager manager;
    globalFunctions::clearConsole();
    manager.menu();
    return 0;
}
