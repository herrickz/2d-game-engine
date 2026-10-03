
#include "Hello.h"
#include "Goodbye.h"
#include "Render/Render.h"
#include "Window/Window.h"

int main(int argc, char **argv) {

    Render render;
    Window window;
    Hello hello;

    hello.saySomething();
    render.doSomething();
    window.doSomething();
}