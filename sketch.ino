#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 4       // حساس الحرارة
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// التوصيلات زي صورتك بالظبط
const int moisturePin = 3; // المقاومة المتغيرة على Pin 3
const int relayPin = 5;     // الريلاي على Pin 5
const int buzzerPin = 18;   // الصفارة على Pin 18

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9); // الشاشة على 8 و 9
  
  pinMode(moisturePin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // إطفاء الريلاي والصفارة في البداية
  digitalWrite(relayPin, HIGH); 
  noTone(buzzerPin);
  
  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    for(;;);
  }
  
  // شاشة الترحيب
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("AQUA 37");
  display.display();
  delay(2000);
}

void loop() {
  // قراءة الحرارة
  float t = dht.readTemperature();
  
  // قراءة الرطوبة من Pin 3
  int moistureValue = analogRead(moisturePin);
  int moisturePercent = map(moistureValue, 0, 4095, 0, 100);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("AQUA 37 - Smart Farm");
  display.drawLine(0, 10, 128, 10, WHITE);

  // عرض الحرارة
  display.setCursor(0, 15);
  display.print("Temp: ");
  if (isnan(t)) {
    display.println("Error");
  } else {
    display.print(t);
    display.println(" C");
  }

  // عرض الرطوبة
  display.setCursor(0, 28);
  display.print("Moisture: ");
  display.print(moisturePercent);
  display.println(" %");

  // منطق الري
  if (moisturePercent < 30) { 
    display.setCursor(0, 45);
    display.println("PUMP: ON (Dry Soil)");
    
    digitalWrite(relayPin, LOW); // تشغيل الريلاي (اللمبة تنور)
    tone(buzzerPin, 2000, 500);  // صوت إنذار
    
  } else if (moisturePercent > 60) {
    display.setCursor(0, 45);
    display.println("PUMP: OFF (Wet)");
    
    digitalWrite(relayPin, HIGH); // إطفاء الريلاي (اللمبة تطفي)
    noTone(buzzerPin);
    
  } else {
    display.setCursor(0, 45);
    display.println("STATUS: STABLE");
    digitalWrite(relayPin, HIGH); 
    noTone(buzzerPin);
  }
  
  display.display();
  delay(1000); 
}
