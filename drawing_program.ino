#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

// Define commonly used colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color palette
#define NUM_COLORS 8
const uint16_t colors[NUM_COLORS] = {
    BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA
};

// UI Layout Constants
#define TOOLBAR_HEIGHT 48
#define COLOR_SWATCH_SIZE 40
#define BRUSH_SIZE_BTN_W 30
#define BRUSH_SIZE_BTN_H 30

// Drawing Area Constants
#define DRAWING_AREA_X 0
#define DRAWING_AREA_Y TOOLBAR_HEIGHT
#define DRAWING_AREA_W 320  // tft.width() in setup
#define DRAWING_AREA_H 432  // tft.height() - TOOLBAR_HEIGHT in setup

// Touch screen pin configuration
#define YP A2  // must be an analog pin
#define XM A1  // must be an analog pin
#define YM 6   // can be a digital pin
#define XP 7   // can be a digital pin

// Touch screen constants
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Touch screen calibration
#define TS_LEFT 150
#define TS_RT   920
#define TS_TOP  100
#define TS_BOT  940

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // 300 ohm across X plate
TSPoint tp;

// Global variables for drawing
uint16_t currentColor = BLACK;
uint8_t currentBrushSize = 1;

void setup() {
    Serial.begin(9600);

    uint16_t ID = tft.readID();
    if (ID == 0xD3D3) ID = 0x9486; // Fix for some screens
    tft.begin(ID);
    tft.setRotation(1);            // Screen rotation (0-3)
    tft.fillScreen(BLACK);         // Clear screen
    drawUI();                      // Initialize UI

    // Print memory usage information
    Serial.println(F("Memory Usage Information:"));
    Serial.print(F("Available SRAM: "));
    Serial.print(2048);  // Arduino Uno has 2KB SRAM
    Serial.println(F(" bytes"));
    Serial.print(F("Program Storage: "));
    Serial.print(32256);  // Arduino Uno flash memory
    Serial.println(F(" bytes total"));

    Serial.println(F("Drawing Program Started"));
    Serial.print(F("Screen ID: 0x"));
    Serial.println(ID, HEX);
}

uint16_t rainbowColor(int index) {
    switch (index % 6) {
        case 0: return RED;
        case 1: return YELLOW;
        case 2: return GREEN;
        case 3: return CYAN;
        case 4: return BLUE;
        case 5: return MAGENTA;
    }
    return WHITE;
}

uint16_t interpolateColor(uint16_t color1, uint16_t color2, float ratio) {
    int r1 = (color1 & 0xF800) >> 11;
    int g1 = (color1 & 0x07E0) >> 5;
    int b1 = color1 & 0x001F;

    int r2 = (color2 & 0xF800) >> 11;
    int g2 = (color2 & 0x07E0) >> 5;
    int b2 = color2 & 0x001F;

    int r = r1 + (r2 - r1) * ratio;
    int g = g1 + (g2 - g1) * ratio;
    int b = b1 + (b2 - b1) * ratio;

    return ((r << 11) | (g << 5) | b);
}

uint16_t hsvToRgb565(float h, float s, float v) {
    float r, g, b;
    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch(i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }

    return ((int(r * 31) << 11) | (int(g * 63) << 5) | int(b * 31));
}

// Touch coordinate mapping functions
int mapTouchX(TSPoint p) {
    return map(p.x, TS_LEFT, TS_RT, 0, tft.width());
}

int mapTouchY(TSPoint p) {
    return map(p.y, TS_TOP, TS_BOT, 0, tft.height());
}

// Check if touch pressure is within valid range
bool isTouchValid(TSPoint p) {
    return (p.z > MINPRESSURE && p.z < MAXPRESSURE);
}

// Restore pin modes after touch read
void restorePinModes() {
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
}

// Toolbar interaction
void handleToolbarTouch(int16_t x, int16_t y) {
    if (y < TOOLBAR_HEIGHT) {
        if (x < (NUM_COLORS * (COLOR_SWATCH_SIZE + 2))) {
            int newColor = colors[x / (COLOR_SWATCH_SIZE + 2)];
            if (newColor != currentColor) {
                currentColor = newColor;
                drawUI();
            }
        } else if (x > (tft.width() - (3 * BRUSH_SIZE_BTN_W))) {
            uint8_t newSize = ((x - (tft.width() - (3 * BRUSH_SIZE_BTN_W))) / BRUSH_SIZE_BTN_W) + 1;
            if (newSize != currentBrushSize && newSize <= 3) {
                currentBrushSize = newSize;
                drawUI();
            }
        }
    }
}

// Drawing Functions
void handleDrawing() {
    static int16_t old_x = -1, old_y = -1;

    // Get touch point
    TSPoint p = ts.getPoint();
    restorePinModes();  // Restore pins after touch read

    if (isTouchValid(p)) {
        int16_t x = mapTouchX(p);
        int16_t y = mapTouchY(p);

        if (y >= DRAWING_AREA_Y) {
            // Drawing area - draw line if we have an old point
            if (old_x != -1) {
                tft.drawLine(old_x, old_y, x, y, currentColor);
            }
            old_x = x;
            old_y = y;
        } else {
            handleToolbarTouch(x, y);  // Handle toolbar interaction
            old_x = old_y = -1;  // Reset drawing position
        }
    } else {
        old_x = old_y = -1;  // Reset when touch released
    }
}

// UI Drawing Functions
void drawColorPalette() {
    int x = 4; // Start with a small margin
    for(int i = 0; i < NUM_COLORS; i++) {
        tft.fillRect(x, 4, COLOR_SWATCH_SIZE, TOOLBAR_HEIGHT-8, colors[i]);
        if (colors[i] == currentColor) {
            tft.drawRect(x-2, 2, COLOR_SWATCH_SIZE+4, TOOLBAR_HEIGHT-4, WHITE);
        }
        x += COLOR_SWATCH_SIZE + 2; // Proper spacing as required
    }
}

void drawBrushSizes() {
    int x = tft.width() - (3 * BRUSH_SIZE_BTN_W) - 4;
    for(int i = 0; i < 3; i++) {
        tft.fillRect(x, 4, BRUSH_SIZE_BTN_W, BRUSH_SIZE_BTN_H, WHITE);
        tft.fillCircle(x + (BRUSH_SIZE_BTN_W/2),
                      TOOLBAR_HEIGHT/2,
                      (i+1)*2, BLACK);
        if ((i+1) == currentBrushSize) {
            tft.drawRect(x-2, 2, BRUSH_SIZE_BTN_W+4, BRUSH_SIZE_BTN_H+4, RED);
        }
        x += BRUSH_SIZE_BTN_W + 2;
    }
}

void drawUI() {
    tft.fillRect(0, 0, tft.width(), TOOLBAR_HEIGHT, BLACK);
    drawColorPalette();
    drawBrushSizes();
}

void loop() {
    handleDrawing();  // Handle touch input and drawing
    delay(5); // Touch sampling delay
}
