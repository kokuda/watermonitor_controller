
#include <stdio.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

#define LCD_PIN_RS	16
#define LCD_PIN_E	15
#define LCD_PIN_DB4	2
#define LCD_PIN_DB5	14
#define LCD_PIN_DB6	12
#define LCD_PIN_DB7	13
static LiquidCrystal lcd(LCD_PIN_RS,LCD_PIN_E,LCD_PIN_DB4,LCD_PIN_DB5,LCD_PIN_DB6,LCD_PIN_DB7);

#define NUM_LINES 5
#define LINE_LENGTH 16

static char displayLines[NUM_LINES][LINE_LENGTH + 1];
static int currentLine = 0;

static int getNextLine(int line) {

    int next = line;
    do {
        next = (next + 1) % NUM_LINES;
        if (strlen(displayLines[next]) > 0) {
            break;
        }        
    } while (next != line);

    return next;
}

void initDisplay() {
    // set up the LCD's number of columns and rows
    lcd.begin(16, 2);
    lcd.display();
}

void updateDisplay() {

    lcd.clear();
    lcd.noDisplay();
    delay(500);
    lcd.display();

    lcd.setCursor(0, 0);
    lcd.print(displayLines[currentLine]);
    currentLine = getNextLine(currentLine);

    lcd.setCursor(0, 1);
    lcd.print(displayLines[currentLine]);
}

void setDisplayLine(int line, const char * format, ...) {
    if (line >= NUM_LINES) {
        Serial.print(F("[setLine]: Invalid line")); Serial.println(line);
        return;
    }

    char* text = displayLines[line];
    memset(text, 0, LINE_LENGTH+1);

    va_list arg;
    va_start(arg, format);
    vsnprintf(text, LINE_LENGTH+1, format, arg);
    va_end(arg);
}
