#include <Button.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include "menu.h"

unsigned long TimeProvider::getTime() {
  return millis();
}

TimeSavePoint::TimeSavePoint(TimeProvider& tp) : timeProvider(tp) {
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

Lcd::Lcd(LiquidCrystal &l, int c, int r) : lcd(l), cols(c), rows(r) {
}

Lcd::Lcd(LiquidCrystal &l, int c, int r, char empty) : lcd(l), cols(c), rows(r), emptyChar(empty) {
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

MenuInternals::MenuInternals(Lcd &l, Button &up, Button &down, Button &menu, String* menuItems, int menuItemsCount, int timeoutSeconds, MenuCallback** menuItemsCallbacks, void* customData) : lcd(l), btnUp(up), btnDown(down), btnMenu(menu) {
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


void Menu::init() {
  t.save();
  internals.currentIndex = 0;
  lcdModified = true;
}

void Menu::showCurrentMenu() {
  internals.lcd.showString(internals.items[internals.currentIndex], 0);
  // internals.lcd.showString(String(menuIndex), 0);
}

void Menu::buttonUpDownClicked(int delta) {
  lcdModified = true;
  t.save();
  internals.currentIndex += delta;
  while (internals.currentIndex < 0) {
    internals.currentIndex = internals.itemsCount + internals.currentIndex;
  }
  while (internals.currentIndex >= internals.itemsCount) {
    internals.currentIndex = internals.currentIndex - internals.itemsCount;
  }
}


Menu::Menu(TimeProvider &tp, Lcd &lcd, Button &btnUp, Button &btnDown, Button &btnMenu, String* menuItems, int menuItemsCount, int timeoutSeconds, void* customData, MenuCallback* menuItemsCallbacks[]) : t(tp), internals(lcd, btnUp, btnDown, btnMenu, menuItems, menuItemsCount, timeoutSeconds, menuItemsCallbacks, customData) {
  init();
  numberOfCallbacks = 0;
}

Menu::Menu(TimeProvider &tp, Lcd &lcd, Button &btnUp, Button &btnDown, Button &btnMenu, String* menuItems, int menuItemsCount, int timeoutSeconds, void* customData, int numberOfCallbacks...) : t(tp), internals(lcd, btnUp, btnDown, btnMenu, menuItems, menuItemsCount, timeoutSeconds, NULL, customData) {
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
}

Menu::~Menu() {
  if (numberOfCallbacks > 0) {
    for (int i = 0; i < numberOfCallbacks; i++) {
      delete internals.itemsCallbacks[i];
    }
    delete internals.itemsCallbacks;
  }
}

void Menu::show() {
  init();

  bool exitFlag = false;
  while (!exitFlag) {
    if (lcdModified) {
      showCurrentMenu();
      lcdModified = false;
    }
    if (internals.btnUp.uniquePress()) {
      buttonUpDownClicked(-1);
    } else if (internals.btnDown.uniquePress()) {
      buttonUpDownClicked(1);
    } else if (internals.btnMenu.uniquePress() && internals.itemsCallbacks != NULL && internals.itemsCallbacks[internals.currentIndex] != NULL) {
      exitFlag = internals.itemsCallbacks[internals.currentIndex]->execute(&internals);
      lcdModified = true;
      t.save();
    } else if (t.elapsedSeconds() > internals.timeoutSeconds) {
      exitFlag = true;
    }
  }
}
