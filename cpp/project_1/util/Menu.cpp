#include "Menu.h"

string Menu::numberedOption(int i, bool chosed)
{
    string optionPrefix;

    if (chosed)
    {
        optionPrefix += "> ";
    }
    else
    {
        optionPrefix += "  ";
    }

    return optionPrefix + "[" + to_string(i) + "] " + options[i].title;
}

void Menu::printHighlighted(string text)
{
    // Green color
    cout << "\x1b[32m";

    cout << text;

    // Reset color
    cout << "\x1b[34m";
}

void Menu::printLineAt(int selected, bool highlight)
{
    cout << "\r\x1b[K";

    if (highlight)
    {
        printHighlighted(numberedOption(selected, highlight));
        return;
    }

    cout << numberedOption(selected);
}

Menu::Menu(vector<MenuOption> options, string title)
    : options(options), title(title) {}

Menu::~Menu() {}

void Menu::start(MenuContext context)
{
    vector<MenuOption> opitons = this->options;

    options.push_back({
        context.isRoot() ? "Exit" : "Back",
        [](MenuContext context)
        {
            context.pop();
        },
    });

    int selected = 0;

    cout << "\x1b[?25l";

    cout << "\x1b[34m";

    if (title != "")
    {

        cout << "\x1b[4m";

        cout << endl;
        cout << title + ":" << endl;

        cout << "\x1b[24m";

        cout << "\x1b[34m";
    }

    if (options.size() == 0)
    {
        cout << "No options to show" << endl;
        return;
    }

    for (int i = 0; i < options.size(); i++)
    {
        printLineAt(i);

        if (i != options.size() - 1)
        {
            cout << endl;
        }
    }

    // Save cursor position
    cout << "\x1b[s";

    // Move cursor up
    if (options.size() > 1)
    {
        cout << "\x1b[" + to_string(options.size() - 1 - selected) + "A";
    }

    printLineAt(selected, true);

    char c;

    while (c != 13)
    {

        system("stty raw");
        c = getchar();
        system("stty cooked");

        switch (c)
        {

        // Up arrow
        case 65:
            // If the selected option is not the first one
            if (selected > 0)
            {
                printLineAt(selected);
                selected--;
                cout << "\x1b[A";
                printLineAt(selected, true);
                break;
            }

            if (loopOptions && options.size() > 1)
            {
                printLineAt(selected);
                selected = options.size() - 1;
                cout << "\x1b[" + to_string(options.size() - 1) + "B";
            }

            printLineAt(selected, true);
            break;
        // Down arrow
        case 66:

            if (selected < options.size() - 1)
            {
                printLineAt(selected);
                selected++;
                cout << "\x1b[B";
                printLineAt(selected, true);
                break;
            }

            if (loopOptions && options.size() > 1)
            {
                printLineAt(selected);
                selected = 0;
                cout << "\x1b[" + to_string(options.size() - 1) + "A";
            }

            printLineAt(selected, true);
            break;
        default:

            printLineAt(selected, true);
            break;
        }
    }

    cout << "\x1b[u"
         << "\x1b[0m"
         << "\x1b[?25h" << endl;

    options[selected].func(context);
}
