#include <WebServer.h>
WebServer server(80);

bool nightMode = false;
bool isRocking = false;
bool isPlayingMusic = false;
bool isBabyQuiet = true;

void handleStream() {
  Serial.println("HANDLE STREAM");
  WiFiClient client = server.client();
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (client.connected()) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) continue;

    response = "--frame\r\n";
    response += "Content-Type: image/jpeg\r\n\r\n";
    server.sendContent(response);
    client.write(fb->buf, fb->len);
    server.sendContent("\r\n");
    esp_camera_fb_return(fb);
    delay(30); // ~30 fps
  }
}

void handleNightMode() {
  Serial.println("HANDLE NIGHT MODE");
  pinMode(4, OUTPUT);
  nightMode = !nightMode; // alterna estado
  analogWrite(4, nightMode ? 10 : 0);

  String json = "{\"nightMode\":";
  json += nightMode ? "\"ON\"" : "\"OFF\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleCapture() {
  Serial.println("HANDLE CAPTURE");
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void handleRocking() {
  Serial.println("TOGGLE ROCKING");
  isRocking = !isRocking; // alterna estado
  // hacer que el motor haga lo suyo

  String json = "{\"rockingMode\":";
  json += isRocking ? "\"ON\"" : "\"OFF\"";
  json += "}";

  server.send(200, "application/json", json);
}


void handleTemperature() {
  Serial.println("HANDLE TEMPERATURE");

  // obtener temperatura

  String json = "{\"temperature\":";
  json += "\"22˚C\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleMovement() {
  Serial.println("HANDLE MOVEMENT");
  isBabyQuiet = !isBabyQuiet; // alterna estado
  // get movement

  String json = "{\"isBabyQuiet\":";
  json += isBabyQuiet ? "\"yes\"" : "\"no\"";
  json += "}";

  server.send(200, "application/json", json);
}

void startCameraServer() {
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/night", HTTP_GET, handleNightMode);
  server.on("/capture", HTTP_GET, handleCapture);
  server.on("/toggleRocking", HTTP_GET, handleRocking);
  server.on("/getTemperature", HTTP_GET, handleTemperature);
  server.on("/getMovement", HTTP_GET, handleMovement);
  server.begin();
}
