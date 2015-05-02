/*
 Menu library - shows text menu on LCD. 
 Requires 3 buttons: up, down, menu. 
 Up/down are used to navigate trough the menu items, the "menu" button is used to select the current item.
 If no input from the user for "timeout" seconds, the menu exits
 */

#ifndef Menu_h
#define Menu_h

//#include <Button.h>
//#include <LiquidCrystal.h>

class TimeProvider {
public:
	unsigned long getTime();
};

class Logger {
public:
	virtual bool isLogEnabled();
	virtual void log(const char* msg, bool newLine = true);
	virtual void log(const __FlashStringHelper* msg, bool newLine = true);
	virtual void logInt(int value, bool newLine = true);
	virtual void logLong(long value, bool newLine = true);
	virtual void logFloat(float value, bool newLine = true);
	virtual void logDouble(double value, bool newLine = true);
};

class TimeSavePoint {
protected:
	unsigned long time;
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
	Logger& logger;
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
	MenuInternals(Logger& logger, Lcd &l, Button &up, Button &down, Button &menu, String* menuItems, int menuItemsCount, int timeoutSeconds, MenuCallback** menuItemsCallbacks, void* customData);
};

typedef bool (*MenuCallbackFunc)(MenuInternals* menuInternals);

class MenuCallback {
public:
	virtual bool execute(MenuInternals* menuInternals) = 0;
};

class ExitMenuCallback: public MenuCallback {
public:
	virtual bool execute(MenuInternals* menuInternals);
};

class SimpleMenuCallback: public MenuCallback {
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

protected:
	virtual void init();
	virtual void showCurrentMenu();
	virtual void buttonUpDownClicked(int delta);
	virtual void waitUntilButtonIsUnpressed(Button& btn);
	virtual bool isLogEnabled();
	virtual void log(const char* msg, bool newLine = true);
	virtual void log(const __FlashStringHelper* msg, bool newLine = true);
	virtual void logInt(int value, bool newLine = true);

public:
	Menu(Logger &logger, TimeProvider &tp, Lcd &lcd, Button &btnUp, Button &btnDown, Button &btnMenu, String* menuItems, int menuItemsCount, int timeoutSeconds, void* customData, MenuCallback* menuItemsCallbacks[]);
	Menu(Logger &logger, TimeProvider &tp, Lcd &lcd, Button &btnUp, Button &btnDown, Button &btnMenu, String* menuItems, int menuItemsCount, int timeoutSeconds, void* customData, int numberOfCallbacks...);
	~Menu();
	virtual void show();
};

#endif

