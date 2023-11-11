#include "MenuContext.h"

MenuContext::MenuContext() {}
MenuContext::~MenuContext() {}

void MenuContext::startMenu(Menu menu)
{
    // Clear the screen
    cout << "\x1b[2J";
    // Move the cursor to the top left
    cout << "\x1b[H";

    menu.start(*this);
}

void MenuContext::push(Menu menu)
{
    context.push_back([menu]()
                      { return menu; });
    startMenu(menu);
}

void MenuContext::push(function<Menu()> menu)
{
    context.push_back(menu);
    startMenu(menu());
}

bool MenuContext::isRoot()
{
    return context.size() == 1;
}

void MenuContext::pop()
{
    if (context.size() == 1)
    {
        return;
    }

    context.pop_back();
    startMenu(context.back()());
}

void MenuContext::run(Menu menu)
{
    MenuContext context;
    context.push(menu);
}