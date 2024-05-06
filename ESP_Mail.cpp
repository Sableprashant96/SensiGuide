#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

#define WIFI_SSID "ESP"
#define WIFI_PASSWORD "88888888"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "sensiguide8@gmail.com"
#define AUTHOR_PASSWORD "dlflyfkubnuhbqqa"

SMTPSession smtp;


const int BUTTON_PIN = 0; // Pin D4 for the button

Session_Config config;

bool buttonPressed = false; // Variable to track button press

void setup(){
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  MailClient.networkReconnect(true);

  smtp.debug(1);

  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";

  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 0;

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop(){
  int buttonState = digitalRead(BUTTON_PIN); // Read the state of the button

  // Check if the button is pressed (active LOW)
  if (buttonState == LOW && !buttonPressed) {
    buttonPressed = true; // Set flag to indicate button is pressed
  }

  // Check if the button is released (active HIGH)
  if (buttonState == HIGH && buttonPressed) {
    sendEmail(); // Call function to send email
    buttonPressed = false; // Reset flag when button is released
    delay(1000); // Add a delay to debounce the button
  }

  delay(50); 
}

void sendEmail() {
  SMTP_Message message;

  message.sender.name = F("SensiGuide");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("Emergency: Blind Person in Trouble");
  message.addRecipient(F("Prashant"), "sableprashant96@gmail.com");
  // message.addRecipient(F("shivlal"), "guptashivlal-elec@atharvacoe.ac.in");
  // message.addRecipient(F("Harshali"), "harshalibagale-elec@atharvacoe.ac.in");
  // message.addRecipient(F("Aditya"), "guptaaditya-elec@atharvacoe.ac.in");
    

  // Compose message body
  String body = "Dear,\n\n";
  body += "This is an emergency alert. I am in trouble.\n";
  body += "My Current Coordinates:\n";
  
  // Coordinates
  float latitude =  0;
  float longitude = 0;
  
  body += "Latitude: " + String(latitude, 6) + "\n";
  body += "Longitude: " + String(longitude, 6) + "\n\n";

  // Construct Google Maps link
  String googleMapsLink = "https://www.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);
  body += "Click here to view my location on Google Maps: " + googleMapsLink + "\n\n";

  body += "Please take immediate action to provide assistance.\n\n";
  body += "Sincerely,\n";
  body += "SensiGuide";
  message.text.content = body.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  /* Connect to the server */
  if (!smtp.connect(&config)){
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn()){
    Serial.println("\nNot yet logged in.");
  }
  else{
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
}
