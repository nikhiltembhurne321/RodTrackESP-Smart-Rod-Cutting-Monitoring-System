#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi credentials
const char* ssid = "Your_WiFi";         // 🔁 Your WiFi
const char* password = "Your_Password";  // 🔁 Your Password

WiFiServer server(80);

#define IR_SENSOR_PIN D5

int currentCount = 0;
unsigned long startTime = 0;
unsigned long duration = 0;
unsigned long totalDuration = 0;
float averageTime = 0;
bool isCutting = false;

const int maxRecords = 50;
unsigned long rodTimes[maxRecords];

void setup() {
  Serial.begin(115200);
  pinMode(IR_SENSOR_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.begin();
  Serial.println("WiFi connected!");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected!");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  static bool previousState = HIGH;
  bool currentState = digitalRead(IR_SENSOR_PIN);

  if (previousState == HIGH && currentState == LOW) {
    startTime = millis();
    isCutting = true;
  }

  if (previousState == LOW && currentState == HIGH && isCutting) {
    duration = millis() - startTime;
    totalDuration += duration;
    if (currentCount < maxRecords) {
      rodTimes[currentCount] = duration;
    }
    currentCount++;
    averageTime = totalDuration / (float)currentCount;
    isCutting = false;
  }

  previousState = currentState;

  // OLED Display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Count: ");
  display.println(currentCount);
  display.print("Last: ");
  display.print(duration);
  display.println(" ms");
  display.print("Avg: ");
  display.print(averageTime, 1);
  display.println(" ms");
  display.display();

  // Web Server
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    String request = client.readStringUntil('\r');
    client.flush();

    // Encode WhatsApp and Email message body
    String summary = "Rod Cutting Summary\n";
    summary += "Total Rods: " + String(currentCount) + "\n";
    summary += "Average Time: " + String(averageTime, 1) + " ms\n";
    int limit = currentCount > 10 ? 10 : currentCount;
    for (int i = 0; i < limit; i++) {
      summary += "Rod " + String(i + 1) + ": " + String(rodTimes[i]) + " ms\n";
    }

    // URL encode for WhatsApp and Gmail
    String encoded = "";
    for (int i = 0; i < summary.length(); i++) {
      if (summary[i] == ' ') encoded += "%20";
      else if (summary[i] == '\n') encoded += "%0A";
      else encoded += summary[i];
    }

    // Gmail Link
    String emailLink = "https://mail.google.com/mail/?view=cm&fs=1&to=official@email.com&su=Rod%20Cutting%20Report&body=" + encoded;

    // HTML Web Page
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>Rod Monitor</title>";
    html += "<script>";
    html += "function downloadCSV() {";
    html += "let csv = 'Rod Number,Time (ms)\\n';";
    html += "const rows = document.querySelectorAll('table tr');";
    html += "for (let i = 1; i < rows.length; i++) {";
    html += "const cols = rows[i].querySelectorAll('td');";
    html += "csv += cols[0].innerText + ',' + cols[1].innerText + '\\n';";
    html += "}";
    html += "const blob = new Blob([csv], {type: 'text/csv'});";
    html += "const link = document.createElement('a');";
    html += "link.href = URL.createObjectURL(blob);";
    html += "link.download = 'rod_data.csv';";
    html += "document.body.appendChild(link);";
    html += "link.click();";
    html += "document.body.removeChild(link);";
    html += "}";
    html += "</script></head><body style='font-family: Arial;'>";

    html += "<h2>Rod Cutting Monitor</h2>";
    html += "<p><strong>Total Rods:</strong> " + String(currentCount) + "</p>";
    html += "<p><strong>Last Time:</strong> " + String(duration) + " ms</p>";
    html += "<p><strong>Avg Time:</strong> " + String(averageTime, 1) + " ms</p>";

    html += "<table border='1' cellpadding='5'><tr><th>Rod #</th><th>Time (ms)</th></tr>";
    for (int i = 0; i < currentCount && i < maxRecords; i++) {
      html += "<tr><td>" + String(i + 1) + "</td><td>" + String(rodTimes[i]) + "</td></tr>";
    }
    html += "</table><br>";

    // Buttons
    html += "<a href='https://wa.me/?text=" + encoded + "' target='_blank'>";
    html += "<button style='padding:10px;font-size:16px;'>Share on WhatsApp</button></a> ";

    html += "<a href='" + emailLink + "' target='_blank'>";
    html += "<button style='padding:10px;font-size:16px;'>Send Email</button></a> ";

    html += "<button onclick='downloadCSV()' style='padding:10px;font-size:16px;'>Download CSV</button>";

    html += "<p><small>IP: " + WiFi.localIP().toString() + "</small></p>";
    html += "</body></html>";

    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    client.print(html);
    delay(10);
    client.stop();
  }
}
