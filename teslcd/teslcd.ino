#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Kudu ditambah pin pull up external
int currentcap = 0;
int maxcap = 0;
char str1[] = "current capacity";
char str2[] = "max capacity";

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  //Pindahin ke I2C2 sesuai skematik alat
  Wire.setSDA(PB11);
  Wire.setSCL(PB10);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial1.println("SSD1306 allocation failed");
    for (;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();

  // Clear the buffer
  display.clearDisplay();

}

void loop() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,10);
  display.println("Qorites");
  display.display();
  delay(2000);
  
  lcdhandler();
}

void lcdhandler(void) {
  //clear, text size, text color, cursor!!! Kurang 1 aja gabakal keluar
  display.clearDisplay();
  delay(1000);
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setCursor(20, 0);     // Start at top-left corner
  display.setTextColor(SSD1306_WHITE);
  display.println(F(str1));
  //display.display();
  display.setCursor(60, 10);
  display.setTextSize(2);
  display.print(currentcap);
  //display.display();
  display.setTextSize(1);
  display.setCursor(20, 27);
  display.print(F(str2));
  display.setCursor(60, 45);
  display.setTextSize(2);
  display.print(maxcap);
  display.display();
  delay(2000);
  currentcap = currentcap + 1;
  maxcap = maxcap + 1;
}
