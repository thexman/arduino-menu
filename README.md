# Info
Text menu library for Arduino. Shows text based menu on LCD display.

# Requirements
Requires [Button library](http://playground.arduino.cc/code/Button)

# License
The GNU Lesser General Public License (LGPL)


# Example:
```Arduino

LiquidCrystal _lcd(13, 12, 6, 5, 4, 3);

const int numRows = 2;
const int numCols = 16;


TimeProvider tp;
Lcd lcd(_lcd, numCols, numRows, ' ');


Button btnUp = Button(10, PULLDOWN);
Button btnDown = Button(9, PULLDOWN);
Button btnMenu = Button(11, PULLDOWN);
String menuItems[] = { String("Exit"), String("Menu item 1"), String("Menu item 2"), String("Menu item 3") };

bool printMenu(MenuInternals* internals) {
  String s = String(internals->currentIndex) + String(" ") + internals->items[internals->currentIndex];
  internals->lcd.showString(s, 1);
  return false;
}

Menu m = Menu(tp, lcd, btnUp, btnDown, btnMenu, menuItems, 4, 15, NULL, 1, printMenu, printMenu, printMenu, printMenu);


void setup() {
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  _lcd.begin(numCols, numRows);
  _lcd.setCursor(0, 0);
}

void loop() {
  if (btnMenu.uniquePress()) {
    m.show();
  } else {
    lcd.showString(String("Press menu key"), 0);
  }
}

```
