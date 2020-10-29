#include <tchar.h>
#include <windows.h>
#include <shlwapi.h>
#include <vector>
#include <string.h>
#include <iostream>
#pragma comment(lib, "shlwapi.lib")
#include <SFML/Audio.hpp>

int file_count = 0;
using namespace std;

void EmunFiles(TCHAR* StartPath)
{
    HANDLE hFind;
    WIN32_FIND_DATA ffd = { 0 };
    TCHAR SearchPath[MAX_PATH];
    lstrcpy(SearchPath, StartPath);
    PathRemoveBackslash(SearchPath);
    lstrcat(SearchPath, _T("\\*.*"));
    hFind = FindFirstFile(SearchPath, &ffd);
    if (INVALID_HANDLE_VALUE == hFind) return;
    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (lstrcmp(ffd.cFileName, _T(".")) && lstrcmp(ffd.cFileName, _T(".."))) {
                TCHAR BeginPath[MAX_PATH];
                lstrcpy(BeginPath, StartPath);
                PathAddBackslash(BeginPath);
                lstrcat(BeginPath, ffd.cFileName);
                EmunFiles(BeginPath);
            }
        }
        else {
            TCHAR* FoundPath = new TCHAR[MAX_PATH];
            lstrcpy(FoundPath, StartPath);
            PathAddBackslash(FoundPath);
            lstrcat(FoundPath, ffd.cFileName);
            file_count++;
            delete[] FoundPath;
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
}

int main() {
    setlocale(LC_ALL, "");

    sf::Music music;
    if (!music.openFromFile("signal.wav")) {
        system("cls");
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(handle, FOREGROUND_RED);
        cout << "Неудалось загрузить звук сигнала \'signal.wav\'.\nУбедитесь что файл не повреждён и находится в папке с исполняемым файлом.";
        getchar();
        return 0;
    }

    music.setLoop(true);
    int count_afk = 0;
    int count_old = 0;
    int count_afk_limit = 0;
    cout << "Через сколько секунд бездействия включать сигнализацию?\nВвод: ";
    cin >> count_afk_limit;

    while (true) {
       TCHAR path[128] = TEXT("C:\\YourPath"); // Путь к папке
       EmunFiles(path);
       system("cls");
       if (count_old != file_count) {
           count_afk = 0;
           count_old = file_count;
       }
       else  {
           count_afk++;
           cout << "Секунд без изменений: " << count_afk << endl;
       }
       if (count_afk >= count_afk_limit) {
           if (music.getStatus() != sf::Music::Playing) {
               music.play();
           }
       }
       else {
           music.stop();
       }

       file_count = 0;
       Sleep(1000);
    }
    return 0;
}