# NTP-basierter WiFi-Uhr-Code: Ausführliche Dokumentation

In dieser ausführlichen Dokumentation wird der bereitgestellte Code Schritt für Schritt erläutert. Der Code zeigt die aktuelle Zeit, die von einem NTP-Server (Network Time Protocol) abgerufen wurde, auf einem LED Dot Matrix Display an. Das Programm verwendet die ESP32 WiFi-Bibliothek, um eine Verbindung zum Internet herzustellen, und die NTPClient- und MD_Parola-Bibliotheken, um die Zeitinformationen zu verarbeiten und auf dem Display anzuzeigen.
Abhängigkeiten

## Dieser Abschnitt beschreibt die verwendeten Bibliotheken und deren Funktionen

    `WiFi.h`: Diese Bibliothek ist Teil des ESP32-Standard-SDK und ermöglicht die Verbindung mit einem WiFi-Netzwerk.
    NTPClient.h: Die NTPClient-Bibliothek ermöglicht es, die Zeit von einem NTP-Server abzurufen und zu verarbeiten.
    WiFiUdp.h: Diese Bibliothek ist ebenfalls Teil des ESP32-SDK und wird benötigt, um UDP-Kommunikation (z. B. mit einem NTP-Server) durchzuführen.
    MD_Parola.h: Die MD_Parola-Bibliothek ist eine leistungsfähige Textanzeige-Bibliothek für LED Dot Matrix Displays, die auf MAX72xx-Chips basieren.
    MD_MAX72xx.h: Diese Bibliothek stellt Funktionen zur Ansteuerung von LED Dot Matrix Displays bereit, die auf MAX72xx-Chips basieren.
    SPI.h: Die SPI-Bibliothek ermöglicht die Kommunikation über den SPI-Bus und wird von der MD_MAX72xx-Bibliothek verwendet.

Pin-Definitionen und Konfiguration

In diesem Abschnitt werden die Pins für das Display definiert und erklärt:

cpp

#define CLK_PIN   18
#define DATA_PIN  23
#define CS_PIN    5

    CLK_PIN: Dieser Pin ist für das Clock-Signal des SPI-Bus verantwortlich und wird an den CLK-Pin des LED Dot Matrix Displays angeschlossen.
    DATA_PIN: Dieser Pin wird für das Daten-Signal des SPI-Bus verwendet und wird an den DIN-Pin des Displays angeschlossen.
    CS_PIN: Der Chip-Select-Pin wird verwendet, um das Display zu aktivieren und ist mit dem CS-Pin des Displays verbunden.

Die folgenden Konstanten definieren die Hardware-Konfiguration für das Display:

cpp

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

    HARDWARE_TYPE: Diese Konstante gibt die Art der verwendeten Hardware an. In diesem Fall wird die FC16-Hardware verwendet, die eine häufig verwendete Konfiguration für LED Dot Matrix Displays ist.
    MAX_DEVICES: Diese Konstante gibt die Anzahl der miteinander verbundenen LED Dot Matrix Module an.

Danach wird eine Instanz des MD_Parola-Objekts erstellt, das für die Ansteuerung des Displays verantwortlich ist:

cpp

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

WiFi-Konfiguration

Die Zugangsdaten für das WiFi-Netzwerk werden hier festgelegt:

cpp

const char* ssid     = "yourwlanssid";
const char* password = "yourwlanpassword";

    ssid: Der Name des WiFi-Netzwerks, mit dem eine Verbindung hergestellt werden soll.
    password: Das Passwort für das angegebene WiFi-Netzwerk.

Zeitvariablen

Die folgenden Variablen werden verwendet, um die Zeitinformationen zu speichern und die Zeitaktualisierungen zu steuern:

cpp

String Time, hour, minute;
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;

    Time: Eine Zeichenkette, die die aktuelle Zeit im Format "hh:mm" speichert.
    hour und minute: Zeichenketten, die die aktuelle Stunde und Minute speichern.
    currentMillis und previousMillis: Variablen zum Speichern der aktuellen und vorherigen Zeit in Millisekunden seit dem Start des Mikrocontrollers.
    interval: Das Intervall (in Millisekunden), in dem die Zeitinformationen aktualisiert werden sollen.

Setup-Funktion

In der setup()-Funktion wird die serielle Kommunikation initialisiert, eine Verbindung zum WiFi-Netzwerk hergestellt, das Display initialisiert und der NTP-Client konfiguriert.

cpp

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Initialize the display
  myDisplay.begin();
  myDisplay.setIntensity(0);
  myDisplay.displayClear();

  // Initialize the NTPClient
  timeClient.begin();
  timeClient.setTimeOffset(3600);
}

    Serial.begin(115200): Initialisiert die serielle Kommunikation mit einer Baudrate von 115200 bps.
    Der folgende Codeblock stellt eine Verbindung zum WiFi-Netzwerk her und gibt während des Verbindungsprozesses Punkte auf der seriellen Schnittstelle aus.
    myDisplay.begin(): Initialisiert das LED Dot Matrix Display.
    myDisplay.setIntensity(0): Legt die Helligkeit des Displays fest (0 ist die niedrigste Helligkeit).
    myDisplay.displayClear(): Löscht das Display und stellt sicher, dass es initial leer ist.
    timeClient.begin(): Initialisiert den NTP-Client.
    timeClient.setTimeOffset(3600): Legt die Zeitverschiebung in Sekunden fest. In diesem Fall wird eine Stunde (3600 Sekunden) hinzugefügt.

Loop-Funktion

Die loop()-Funktion ruft kontinuierlich die obtainTime()-Funktion auf, um die Zeit vom NTP-Server abzufragen und auf dem Display anzuzeigen.

cpp

void loop() {
  obtainTime();
}

obtainTime-Funktion

Die obtainTime()-Funktion aktualisiert die Zeit vom NTP-Server und zeigt die aktuelle Stunde und Minute im Format "hh:mm" auf dem Dot Matrix Display an. Die Zeitaktualisierung erfolgt basierend auf dem in der interval-Variable festgelegten Intervall (in Millisekunden).

cpp

void obtainTime() {
  // Update the time from the NTP server
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  currentMillis = millis();

  if (currentMillis - previousMillis > interval)  {
    previousMillis = millis();

    // Get the time as a formatted string
    Time = timeClient.getFormattedTime();
    Serial.println(Time);

    // Extract the hour and minute from the time string
    hour = Time.substring(0, 2);
    minute = Time.substring(3, 5);

    // Build the time string to display
    Time = hour + ":" + minute;
    Serial.println(Time);

    // Display the time string on the dot matrix display
    myDisplay.setTextAlignment(PA_CENTER);
    myDisplay.print(Time);
  }
}

    while(!timeClient.update()): Dieser Codeblock erzwingt eine Aktualisierung der Zeit vom NTP-Server, falls die reguläre Aktualisierung fehlschlägt.
    currentMillis = millis(): Speichert die aktuelle Zeit in Millisekunden seit dem Start des Mikrocontrollers.
    Der folgende if-Block prüft, ob das festgelegte Intervall (in diesem Fall 1000 ms oder 1 s) seit der letzten Aktualisierung vergangen ist. Wenn ja, wird die Zeit aktualisiert und auf dem Display angezeigt.
    Time = timeClient.getFormattedTime(): Ruft die formatierte Zeit vom NTP-Client ab.
    hour = Time.substring(0, 2) und minute = Time.substring(3, 5): Extrahieren die Stunde und Minute aus der formatierten Zeit.
    Time = hour + ":" + minute: Baut die Zeitzeichenkette für die Anzeige auf.
    Der folgende Codeblock zeigt die Zeitzeichenkette auf dem Dot Matrix Display an:
        myDisplay.setTextAlignment(PA_CENTER): Legt die Textausrichtung auf dem Display fest (in diesem Fall zentriert).
        myDisplay.print(Time): Zeigt die formatierte Zeitzeichenkette auf dem Display an.



Pilotkosinus with help from gpt
