#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>
#include <Servo.h>

#define IR_SENSOR   D5
#define LED_PIN     D6
#define BUZZER_PIN  D7
#define SERVO_PIN   D4

// -------- WIFI DETAILS --------
#define WIFI_SSID "Sharmila"
#define WIFI_PASSWORD " "

// -------- EMAIL DETAILS --------
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "sender@gmail.com"
#define AUTHOR_PASSWORD " "   // 16-char app password
#define RECIPIENT_EMAIL "receiver@gmail.com"

SMTPSession smtp;
ESP_Mail_Session session;
SMTP_Message message;

Servo myServo;
bool triggered = false;

void setup() {
  pinMode(IR_SENSOR, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  Serial.begin(9600);

  // -------- WIFI CONNECT --------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");

  // -------- EMAIL SESSION --------
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  smtp.debug(0);
}

void loop() {
  int val = digitalRead(IR_SENSOR);

  if (val == LOW && !triggered) {

    Serial.println("OBJECT_DETECTED");

    // LED + Buzzer
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 5000);

    // Servo open
    myServo.write(90);

    // -------- SEND EMAIL --------
    message.sender.name = "Smart Door System";
    message.sender.email = AUTHOR_EMAIL;
    message.subject = "⚠️ Smart Door Alert";
    message.addRecipient("User", RECIPIENT_EMAIL);

    message.text.content =
      "Alert!\n\n"
      "An object has been detected near the door.\n\n"
      "Actions performed automatically:\n"
      "- LED turned ON\n"
      "- Buzzer activated\n"
      "- Servo motor rotated\n\n"
      "This is an automated message.";

    if (!smtp.connect(&session)) {
      Serial.println("SMTP connection failed");
    } else {
      if (!MailClient.sendMail(&smtp, &message)) {
        Serial.println("Email send failed");
      } else {
        Serial.println("Email sent successfully");
      }
    }

    delay(3000);

    // Turn OFF
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
    myServo.write(0);

    triggered = true;
  }

  if (val == HIGH) {
    triggered = false;
  }

  delay(200);
}
