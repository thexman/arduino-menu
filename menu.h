/*
 Menu library - shows text menu on LCD. 
 Requires 3 buttons: up, down, menu. 
 Up/down are used to navigate trough the menu items, the "menu" button is used to select the current item.
 If no input from the user for "timeout" seconds, the menu exits
*/

#ifndef Menu_h
#define Menu_h

#include <Button.h>
#include <LiquidCrystal.h>

class TimeProvider {
  public:
    unsigned long getTime();
};

class TimeSavePoint {
  protected:
    int time;
    TimeProvider& timeProvider;
  public:
    TimeSavePoint(TimeProvider& tp);
    virtual void save();
    virtual unsigned long elapsedMs();
    virtual unsigned long elapsedSeconds();
};

class Lcd {
  protected:
    LiquidCrystal &lcd;
    int cols;
    int rows;
    char emptyChar = ' ';
  public:
    Lcd(LiquidCrystal &lcd, int cols, int rows);
    Lcd(LiquidCrystal &lcd, int cols, int rows, char emptyChar);

    virtual void animate(const String &s, const int &delay, const int &loopCount);


    virtual void showString(const String& s, const int &row);
};


class MenuCallback;


class MenuInternals {
  public:
    Lcd& lcd;
    Button& btnUp;
    Button& btnDown;
    Button& btnMenu;
    int currentIndex = 0;
    int itemsCount;
    int timeoutSeconds;
    String* items;    
    MenuCallback** itemsCallbacks;
    void* customData;
    MenuInternals(Lcd &l, Button &up, Button &down, Button &menu, String* menuItems, int menuItemsCount, int timeoutSeconds, MenuCallback** menuItemsCallbacks, void* customData);
};

typedef bool (*MenuCallbackFunc)(MenuInternals* menuInternals);

class MenuCallback {
  public:
    virtual bool execute(MenuInternals* menuInternals) = 0;
};

class ExitMenuCallback : public MenuCallback {
  public:
    virtual bool execute(MenuInternals* menuInternals);
};

class SimpleMenuCallback : public MenuCallback {
  private:
    MenuCallbackFunc callback;
  public:
    SimpleMenuCallback(MenuCallbackFunc callback);
    virtual bool execute(MenuInternals* menuInternals);
};


class Menu {
  private:
    TimeSavePoint t;
    bool lcdModified = false;
    MenuInternals internals;
    int numberOfCallbacks = 0;

    virtual void init();
    virtual void showCurrentMenu();
    virtual void buttonUpDownClicked(int delta);


  public:
    Menu(TimeProvider &tp, Lcd &lcd, Button &btnUp, Button &btnDown, Button &btnMenu, String* menuItems, int menuItemsCount, int timeoutSeconds, void* customData, MenuCallback* menuItemsCallbacks[]);
    Menu(TimeProvider &tp, Lcd &lcd, Button &btnUp, Button &btnDown, Button &btnMenu, String* menuItems, int menuItemsCount, int timeoutSeconds, void* customData, int numberOfCallbacks...);
    ~Menu();
    virtual void show();
};


#endif

