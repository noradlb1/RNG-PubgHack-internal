#pragma once
#include "xCheto.hpp"
#include <ThemidaSDK.h>


using namespace xcheto;

using namespace std;

std::string name = XorStr("HADES");
std::string ownerid = XorStr("AFNzDzMucJ");
std::string secret = XorStr("910941c291e3b8656bd179e7af60cad48d78c892d8ff6caf918604bef1380346");
std::string version = XorStr("1.0");
api xChetoApp(name, ownerid, secret, version);


inline bool exists(const std::string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}
std::string GetClipboardText()
{
entryclipboard:
    // Try opening the clipboard
    if (!OpenClipboard(nullptr))
    {

        /*// Release the clipboard
        CloseClipboard();
        Sleep(3000);
        goto entryclipboard;*/
    }

    // Get handle of clipboard object for ANSI text
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr)
    {
        /*// Release the lock
        GlobalUnlock(hData);

        // Release the clipboard
        CloseClipboard();
        Sleep(3000);
        goto entryclipboard;*/
    }

    // Lock the handle to get the actual text pointer
    char *pszText = static_cast<char *>(GlobalLock(hData));
    if (pszText == nullptr)
    {
        // Release the lock
        GlobalUnlock(hData);

        // Release the clipboard
        CloseClipboard();
        Sleep(2000);
        goto entryclipboard;
    }

    // Save text in a string class instance
    std::string text(pszText);

    // Release the lock
    GlobalUnlock(hData);

    // Release the clipboard
    CloseClipboard();

    return text;
}
std::string GetClipboardText2()
{
entryclipboard:
    // Try opening the clipboard
    if (!OpenClipboard(nullptr))
    {

        /*// Release the clipboard
        CloseClipboard();
        Sleep(3000);
        goto entryclipboard;*/
    }

    // Get handle of clipboard object for ANSI text
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr)
    {
        /*// Release the lock
        GlobalUnlock(hData);

        // Release the clipboard
        CloseClipboard();
        Sleep(3000);
        goto entryclipboard;*/
    }

    // Lock the handle to get the actual text pointer
    char *pszText = static_cast<char *>(GlobalLock(hData));
    if (pszText == nullptr)
    {
        // MessageBoxA(0, "Copy your license key to the clipboard.", 0, 0);
        ShellExecuteA(0, "open", "cmd.exe", "/C TITLE KEY NOT FOUND! & COLOR 4 & ECHO COPY YOUR LICENSE KEY TO CLIPBOARD AND START AGAIN & PAUSE > NULL", 0, SW_SHOWNORMAL);
        return "";
    }

    // Save text in a string class instance
    std::string text(pszText);

    // Release the lock
    GlobalUnlock(hData);

    // Release the clipboard
    CloseClipboard();

    return text;
}

string readFile(string location)
{
    string myText;
    ifstream MyReadFile(location);
    while (getline(MyReadFile, myText))
    {
        cout << myText;
    }
    MyReadFile.close();
    return myText;
}

bool login()
{
    VM_START
    static char str0[128] = "";
    std::string token = "";
    bool login = false;
    if (exists(XorStr("C:\\hack_key.ini")))
    {
        std::ifstream MyFile(XorStr("C:\\hack_key.ini"));
        getline(MyFile, token);
        MyFile.close();
        if (!token.empty())
            strcpy(str0, token.c_str());
        login = xChetoApp.license(token);
        if (!login)
        {
            // Invalid Key
        }
        else if (login)
            return true;
    }
    else
    {
    again:
        string token = GetClipboardText2();
        login = xChetoApp.license(token);
        if (!login)
        {
            // Error Try again

            goto again;
        }
        else if (login)
        {
            // manual Success
        }
    }
    VM_END
    return false;
}

void vercheck()
{
    VM_START
    STR_ENCRYPT_START
    std::string variable = xChetoApp.var("ErrorTec");
    if (variable != "trrtrtr2rt12r1tr2t1fg1fg2f1g2f1r5r1r2t1r21g2f1gf2g1")
    {
        exit(39);
    }
    STR_ENCRYPT_END
    VM_END
}