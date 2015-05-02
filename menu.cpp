#include <Button.h>
#include <LiquidCrystal.h>
#include "menu.h"

bool Logger::isLogEnabled() {
	return false;
}

void Logger::log(const char* msg, bool newLine) {
	// do nothing
}

void Logger::log(const __FlashStringHelper* msg, bool newLine) {
	// do nothing
}


void Logger::logInt(int value, bool newLine) {
	// do nothing
}

void Logger::logLong(long value, bool newLine) {
	// do nothing
}

void Logger::logFloat(float value, bool newLine) {
	// do nothing
}

void Logger::logDouble(double value, bool newLine) {
	// do nothing
}

unsigned long TimeProvider::getTime() {
	return millis();
}

TimeSavePoint::TimeSavePoint(TimeProvider& tp) :
		timeProvider(tp) {
}

void TimeSavePoint::save() {
	time = timeProvider.getTime();
}

unsigned long TimeSavePoint::elapsedMs() {
	unsigned long currentTime = timeProvider.getTime();
	return currentTime - time;
}

unsigned long TimeSavePoint::elapsedSeconds() {
	return elapsedMs() / 1000;;
}

Lcd::Lcd(LiquidCrystal &l, int c, int r) :
		lcd(l), cols(c), rows(r) {
}

Lcd::Lcd(LiquidCrystal &l, int c, int r, char empty) :
		lcd(l), cols(c), rows(r), emptyChar(empty) {
}

void Lcd::animate(const String &s, const int &d, const int &loopCount) {
	for (int lc = 0; lc < loopCount; lc++) {
		for (int i = 0; i < s.length(); i++) {
			lcd.setCursor(0, 0);
			lcd.print(s.charAt(i));
			delay(d);
		}
	}

}

void Lcd::showString(const String& s, const int &row) {
	lcd.setCursor(0, row);
	lcd.print(s.c_str());
	for (int i = s.length(); i < this->cols; i++) {
		lcd.print(emptyChar);
	}
}

MenuInternals::MenuInternals(Logger &log, Lcd &l, Button &up, Button &down, Button &menu, String* menuItems, int menuItemsCount, int timeoutSeconds, MenuCallback** menuItemsCallbacks, void* customData) :
		logger(log), lcd(l), btnUp(up), btnDown(down), btnMenu(menu) {
	this->items = menuItems;
	this->itemsCount = menuItemsCount;
	this->timeoutSeconds = timeoutSeconds;
	this->itemsCallbacks = menuItemsCallbacks;
	this->customData = customData;
}

bool ExitMenuCallback::execute(MenuInternals* menuInternals) {
	return true;
}

SimpleMenuCallback::SimpleMenuCallback(MenuCallbackFunc callback) {
	this->callback = callback;
}

bool SimpleMenuCallback::execute(MenuInternals* menuInternals) {
	if (callback != NULL) {
		return callback(menuInternals);
	}
	return false;
}

bool Menu::isLogEnabled() {
	return internals.logger.isLogEnabled();
}

void Menu::log(const char* msg, bool newLine) {
	internals.logger.log(msg, newLine);
}

void Menu::log(const __FlashStringHelper* msg, bool newLine) {
	internals.logger.log(msg, newLine);
}

void Menu::logInt(int v, bool newLine) {
	internals.logger.logInt(v, newLine);
}

void Menu::init() {
	if (isLogEnabled()) {
		log("Entering Menu::init()");
	}
	t.save();
	internals.currentIndex = 0;
	lcdModified = true;
	if (isLogEnabled()) {
		log("Leaving Menu::init()");
	}
}

void Menu::showCurrentMenu() {
	if (isLogEnabled()) {
		log(F("Entering Menu::showCurrentMenu()"));
	}

	internals.lcd.showString(internals.items[internals.currentIndex], 0);

	if (isLogEnabled()) {
		log("currentIndex=");
		logInt(internals.currentIndex);
		log("Leaving Menu::showCurrentMenu()");
	}
}

void Menu::buttonUpDownClicked(int delta) {
	if (isLogEnabled()) {
		log("Entering Menu::buttonUpDownClicked(deta=", false);
		logInt(delta, false);
		log(")");
	}


	lcdModified = true;
	t.save();
	internals.currentIndex += delta;
	while (internals.currentIndex < 0) {
		internals.currentIndex = internals.itemsCount + internals.currentIndex;
	}
	while (internals.currentIndex >= internals.itemsCount) {
		internals.currentIndex = internals.currentIndex - internals.itemsCount;
	}

	if (isLogEnabled()) {
		log("internals.currentIndex =", false);
		logInt(internals.currentIndex);
		log("Leaving Menu::buttonUpDownClicked(deta=", false);
		logInt(delta, false);
		log(")");
	}
}

Menu::Menu(Logger& logger, TimeProvider &tp, Lcd &lcd, Button &btnUp, Button &btnDown, Button &btnMenu, String* menuItems, int menuItemsCount, int timeoutSeconds, void* customData, MenuCallback* menuItemsCallbacks[]) :
		t(tp), internals(logger, lcd, btnUp, btnDown, btnMenu, menuItems, menuItemsCount, timeoutSeconds, menuItemsCallbacks, customData) {

	if (isLogEnabled()) {
		log("Entering Menu::Menu");
	}

	init();
	numberOfCallbacks = 0;

	if (isLogEnabled()) {
		log("Leaving Menu::Menu");
	}
}

Menu::Menu(Logger& logger, TimeProvider &tp, Lcd &lcd, Button &btnUp, Button &btnDown, Button &btnMenu, String* menuItems, int menuItemsCount, int timeoutSeconds, void* customData, int numberOfCallbacks...) : t(tp), internals(logger, lcd, btnUp, btnDown, btnMenu, menuItems, menuItemsCount, timeoutSeconds, NULL, customData) {

	if (isLogEnabled()) {
		log("Entering Menu::Menu");
	}

	va_list args;
	va_start(args, numberOfCallbacks);
	this->numberOfCallbacks = numberOfCallbacks;
	internals.itemsCallbacks = new MenuCallback*[numberOfCallbacks];
	for (int i = 0; i < numberOfCallbacks; i++) {
		MenuCallbackFunc f = va_arg(args, MenuCallbackFunc);
		internals.itemsCallbacks[i] = new SimpleMenuCallback(f);
	}
	va_end(args);

	init();

	if (isLogEnabled()) {
		log("Leaving Menu::Menu");
	}
}

Menu::~Menu() {
	if (isLogEnabled()) {
		log("Entering Menu::~Menu");
	}

	if (numberOfCallbacks > 0) {
		if (isLogEnabled()) {
			log("Deleting callbacks");
		}

		for (int i = 0; i < numberOfCallbacks; i++) {
			delete internals.itemsCallbacks[i];
		}
		delete internals.itemsCallbacks;
	}

	if (isLogEnabled()) {
		log("Leaving Menu::!Menu");
	}
}

void Menu::waitUntilButtonIsUnpressed(Button& btn) {
	if (isLogEnabled()) {
		log("Entering Menu::waitUntilButtonIsUnpressed()");
	}
	while (btn.isPressed()) {
		delay(200);
		log(".", false);
	}
	if (isLogEnabled()) {
		log(".");
		log("Leaving Menu::waitUntilButtonIsUnpressed()");
	}
}

void Menu::show() {
	if (isLogEnabled()) {
		log("Entering Menu::show()");
	}
	init();

	bool exitFlag = false;
	while (!exitFlag) {
		if (lcdModified) {
			showCurrentMenu();
			lcdModified = false;
		}
		if (internals.btnUp.uniquePress()) {
			waitUntilButtonIsUnpressed(internals.btnUp);
			buttonUpDownClicked(-1);
		} else if (internals.btnDown.uniquePress()) {
			waitUntilButtonIsUnpressed(internals.btnDown);
			buttonUpDownClicked(1);
		} else if (internals.btnMenu.uniquePress() && internals.itemsCallbacks != NULL && internals.itemsCallbacks[internals.currentIndex] != NULL) {
			if (isLogEnabled()) {
				log("Selecting menu index ", false);
				logInt(internals.currentIndex);
			}
			waitUntilButtonIsUnpressed(internals.btnMenu);
			exitFlag = internals.itemsCallbacks[internals.currentIndex]->execute(&internals);
			lcdModified = true;
			t.save();
		} else if (t.elapsedSeconds() > internals.timeoutSeconds) {
			exitFlag = true;
			if (isLogEnabled()) {
				log("Menu timeout");
			}
		}
		if (isLogEnabled()) {
			if (exitFlag) {
				log("exitFlag = true, currentIndex = ", false);
			} else {
				log("exitFlag = false, currentIndex = ", false);
			}
			logInt(internals.currentIndex);
		}
		delay(200);
	}

	if (isLogEnabled()) {
		log("Leaving Menu::show()");
	}
}
