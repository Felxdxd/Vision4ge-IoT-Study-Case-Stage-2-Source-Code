#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_system.h>
#include <time.h>
#include <cstring>

// =====================================================
// SMART ENERGY ANOMALY DETECTOR
// Potensiometer mensimulasikan daya 50-500 W.
// =====================================================

// =====================================================
// 1. WIFI DAN MQTT
// =====================================================
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

const char* MQTT_BROKER =
    "swiftsquash-a58a6da6.a01.euc1.aws.hivemq.cloud";

const uint16_t MQTT_PORT = 8883;
const char* MQTT_USERNAME = "vision4ge";

// GANTI dengan password MQTT dari Access Management.
const char* MQTT_PASSWORD = "danzelshu";

const char* MQTT_TOPIC = "sic8/vision4ge/energy-anomaly-2026";
const char* DEVICE_ID = "vision4ge-room01";

// =====================================================
// 2. ROOT CERTIFICATE UNTUK VERIFIKASI TLS
// =====================================================
const char* MQTT_ROOT_CA = R"PEM(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP
BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV
5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY
1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4
NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG
Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91
8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe
pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl
MrY=
-----END CERTIFICATE-----
)PEM";

// =====================================================
// 3. PIN DAN INTERVAL
// =====================================================
const int POT_PIN = 34;
const int LED_GREEN = 26;
const int LED_RED = 27;
const int BUZZER_PIN = 25;
const int BUTTON_PIN = 14;
const int OLED_SDA = 21;
const int OLED_SCL = 22;

const uint32_t SAMPLE_INTERVAL_MS = 2000;

// =====================================================
// 4. MODEL DETEKSI ENERGI
// =====================================================
struct EnergyModel {
  static constexpr int HISTORY_SIZE = 10;
  static constexpr float RISE_LIMIT_PCT = 30.0f;

  float history[HISTORY_SIZE] = {};
  int count = 0;
  int index = 0;

  float power = 0;
  float baseline = 0;
  float threshold = 0;
  float deviation = 0;

  double energyWh = 0;

  bool learning = true;
  bool anomaly = false;
  bool hasPrevious = false;

  float previousPower = 0;
  uint32_t previousMs = 0;

  float mean() const {
    float total = 0;

    for (int i = 0; i < count; ++i) {
      total += history[i];
    }

    return count ? total / count : 0;
  }

  void remember(float value) {
    history[index] = value;
    index = (index + 1) % HISTORY_SIZE;

    if (count < HISTORY_SIZE) {
      ++count;
    }
  }

  void sample(float watts, uint32_t nowMs) {
    // Energi dihitung menggunakan selisih waktu aktual.
    if (hasPrevious) {
      uint32_t elapsed = nowMs - previousMs;

      energyWh +=
          (previousPower + watts) * 0.5 * elapsed / 3600000.0;
    }

    previousMs = nowMs;
    previousPower = watts;
    hasPrevious = true;

    power = watts;
    learning = count < HISTORY_SIZE;
    anomaly = false;

    if (learning) {
      remember(watts);
      baseline = mean();
      deviation = 0;
    } else {
      // Baseline diambil sebelum sampel sekarang ditambahkan.
      baseline = mean();

      deviation = baseline > 0
          ? (watts - baseline) * 100.0f / baseline
          : 0;

      anomaly =
          watts > baseline * (1.0f + RISE_LIMIT_PCT / 100.0f);

      // Data anomali tidak dimasukkan ke baseline normal.
      if (!anomaly) {
        remember(watts);
      }
    }

    threshold =
        baseline * (1.0f + RISE_LIMIT_PCT / 100.0f);
  }

  const char* status() const {
    if (learning) {
      return "LEARNING";
    }

    return anomaly ? "ANOMALY" : "NORMAL";
  }
};

// =====================================================
// 5. OBJEK DAN VARIABEL
// =====================================================
EnergyModel model;

WiFiClientSecure espClient;
PubSubClient mqtt(espClient);

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

bool oledReady = false;
bool alarmSilenced = false;
bool buzzerActive = false;
bool mqttConfigReady = false;
bool ntpStarted = false;

bool buttonRaw = HIGH;
bool buttonStable = HIGH;

uint32_t buttonChangedMs = 0;
uint32_t lastSampleMs = 0;
uint32_t lastOledMs = 0;
uint32_t lastWifiMs = 0;
uint32_t lastMqttMs = 0;
uint32_t mqttRetryMs = 0;
uint32_t sequenceNumber = 0;
uint32_t lastClockMessageMs = 0;

char bootId[9];
char clientId[48];

// =====================================================
// 6. VALIDASI KONFIGURASI
// =====================================================
bool validMQTTConfig() {
  if (!MQTT_BROKER[0] ||
      !MQTT_USERNAME[0] ||
      !MQTT_PASSWORD[0]) {
    return false;
  }

  if (strcmp(MQTT_PASSWORD, "ISI_PASSWORD_MQTT") == 0) {
    return false;
  }

  if (strcmp(MQTT_BROKER, "broker.hivemq.com") == 0) {
    return false;
  }

  if (!strchr(MQTT_BROKER, '.')) {
    return false;
  }

  return !strpbrk(MQTT_BROKER, ":/ @\\\t\r\n");
}

const char* mqttErrorText(int code) {
  switch (code) {
    case -4:
      return "Timeout MQTT. Periksa port 8883 dan koneksi.";

    case -3:
      return "Koneksi terputus. Mencoba kembali otomatis.";

    case -2:
      return "DNS/TCP/TLS gagal. Periksa hostname, CA dan waktu NTP.";

    case -1:
      return "Client belum tersambung.";

    case 1:
      return "Versi MQTT ditolak.";

    case 2:
      return "Client ID ditolak.";

    case 3:
      return "Broker belum tersedia. Periksa status cluster.";

    case 4:
      return "Username/password MQTT salah. Periksa Access Management.";

    case 5:
      return "Akses ditolak. Periksa credential dan role.";

    default:
      return "Periksa host, port, credential dan status cluster.";
  }
}

// =====================================================
// 7. LED DAN BUZZER
// =====================================================
void updateOutputs(uint32_t now) {
  digitalWrite(LED_RED, model.anomaly ? HIGH : LOW);

  if (model.learning) {
    digitalWrite(LED_GREEN, (now / 400) % 2);
  } else {
    digitalWrite(LED_GREEN, model.anomaly ? LOW : HIGH);
  }

  bool shouldSound = model.anomaly && !alarmSilenced;

  if (shouldSound != buzzerActive) {
    if (shouldSound) {
      tone(BUZZER_PIN, 1000);
    } else {
      noTone(BUZZER_PIN);
    }

    buzzerActive = shouldSound;
  }
}

// =====================================================
// 8. TOMBOL UNTUK MEMBISUKAN ALARM
// =====================================================
void handleButton(uint32_t now) {
  bool raw = digitalRead(BUTTON_PIN);

  if (raw != buttonRaw) {
    buttonRaw = raw;
    buttonChangedMs = now;
  }

  if (now - buttonChangedMs >= 40 &&
      buttonStable != raw) {
    buttonStable = raw;

    if (buttonStable == LOW && model.anomaly) {
      alarmSilenced = true;

      Serial.println(
          "Buzzer dibisukan. Status ANOMALY tetap aktif."
      );
    }
  }
}

// =====================================================
// 9. OLED
// =====================================================
void drawOLED() {
  if (!oledReady) {
    return;
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);

  oled.println("SMART ENERGY");

  if (model.learning) {
    oled.printf("LEARNING %d/10\n", model.count);
  } else {
    oled.printf(
        "%s%s\n",
        model.status(),
        alarmSilenced ? " MUTE" : ""
    );
  }

  oled.printf("Power : %6.1f W\n", model.power);
  oled.printf("Normal: %6.1f W\n", model.baseline);
  oled.printf("Limit : %6.1f W\n", model.threshold);
  oled.printf("Rise  : %+6.1f %%\n", model.deviation);
  oled.printf("Energy: %.3f Wh\n", model.energyWh);

  oled.printf(
      "WiFi:%s MQTT:%s",
      WiFi.status() == WL_CONNECTED ? "OK" : "--",
      mqtt.connected() ? "ON" : "--"
  );

  oled.display();
}

// =====================================================
// 10. KONEKSI WIFI, NTP DAN MQTT
// =====================================================
void maintainConnections(uint32_t now) {
  if (WiFi.status() != WL_CONNECTED) {
    if (mqtt.connected()) {
      mqtt.disconnect();
    }

    if (now - lastWifiMs >= 10000) {
      lastWifiMs = now;

      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6);

      Serial.println("WiFi: mencoba menghubungkan kembali...");
    }

    return;
  }

  if (!mqttConfigReady) {
    return;
  }

  // Waktu diperlukan untuk verifikasi sertifikat TLS.
  if (!ntpStarted) {
    configTime(
        0,
        0,
        "pool.ntp.org",
        "time.google.com",
        "time.cloudflare.com"
    );

    ntpStarted = true;

    Serial.println("TLS: menyinkronkan waktu NTP...");
  }

  if (time(nullptr) < 1700000000) {
    if (now - lastClockMessageMs >= 5000) {
      lastClockMessageMs = now;
      Serial.println("TLS: masih menunggu waktu NTP.");
    }

    return;
  }

  if (mqtt.connected()) {
    mqtt.loop();
    return;
  }

  if (now - lastMqttMs < mqttRetryMs) {
    return;
  }

  lastMqttMs = now;

  Serial.printf(
      "MQTT: menghubungkan %s ke %s:%u\n",
      clientId,
      MQTT_BROKER,
      MQTT_PORT
  );

  if (mqtt.connect(
      clientId,
      MQTT_USERNAME,
      MQTT_PASSWORD
  )) {
    mqttRetryMs = 5000;

    Serial.printf(
        "MQTT CONNECTED | topic: %s\n",
        MQTT_TOPIC
    );
  } else {
    mqttRetryMs =
        mqttRetryMs == 0 ? 5000 : mqttRetryMs * 2;

    if (mqttRetryMs > 30000) {
      mqttRetryMs = 30000;
    }

    Serial.printf(
        "MQTT gagal, state=%d. Coba lagi dalam %lu ms.\n",
        mqtt.state(),
        (unsigned long)mqttRetryMs
    );

    Serial.println(mqttErrorText(mqtt.state()));
  }
}

// =====================================================
// 11. PAYLOAD JSON UNTUK COLAB
// =====================================================
void publishReading(uint32_t measuredAtMs) {
  char payload[700];
  ++sequenceNumber;

  int length = snprintf(
      payload,
      sizeof(payload),
      "{\"schema\":1,\"device_id\":\"%s\",\"boot_id\":\"%s\",\"seq\":%lu,"
      "\"uptime_ms\":%lu,\"power_w\":%.1f,\"baseline_w\":%.1f,"
      "\"threshold_w\":%.1f,\"deviation_pct\":%.1f,\"energy_wh\":%.6f,"
      "\"status\":\"%s\",\"anomaly\":%s,\"silenced\":%s,\"buzzer_on\":%s,"
      "\"learning_samples\":%d,\"learning_target\":10,\"sample_ms\":2000}",
      DEVICE_ID,
      bootId,
      (unsigned long)sequenceNumber,
      (unsigned long)measuredAtMs,
      model.power,
      model.baseline,
      model.threshold,
      model.deviation,
      model.energyWh,
      model.status(),
      model.anomaly ? "true" : "false",
      alarmSilenced ? "true" : "false",
      buzzerActive ? "true" : "false",
      model.count
  );

  if (length <= 0 ||
      (size_t)length >= sizeof(payload)) {
    Serial.println("ERROR: buffer JSON tidak cukup.");
    return;
  }

  Serial.println(payload);

  if (!mqtt.connected()) {
    Serial.println(
        "MQTT offline; deteksi lokal tetap berjalan."
    );
  } else {
    bool success = mqtt.publish(
        MQTT_TOPIC,
        payload,
        false
    );

    if (!success) {
      Serial.println("MQTT publish gagal.");
    }
  }
}

// =====================================================
// 12. SETUP
// =====================================================
void setup() {
  Serial.begin(115200);

  pinMode(POT_PIN, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  analogReadResolution(12);
  noTone(BUZZER_PIN);

  Wire.begin(OLED_SDA, OLED_SCL);

  oledReady = oled.begin(
      SSD1306_SWITCHCAPVCC,
      0x3C
  );

  if (!oledReady) {
    Serial.println(
        "OLED gagal; Serial dan deteksi tetap aktif."
    );
  }

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6);

  lastWifiMs = millis();

  snprintf(
      bootId,
      sizeof(bootId),
      "%08lx",
      (unsigned long)esp_random()
  );

  snprintf(
      clientId,
      sizeof(clientId),
      "energy-%s-%s",
      DEVICE_ID,
      bootId
  );

  mqttConfigReady = validMQTTConfig();

  if (!mqttConfigReady) {
    Serial.println(
        "MQTT BELUM DIKONFIGURASI: isi MQTT_PASSWORD di bagian atas."
    );
  }

  // TLS dengan verifikasi sertifikat.
  espClient.setCACert(MQTT_ROOT_CA);
  espClient.setHandshakeTimeout(8);

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setKeepAlive(30);
  mqtt.setSocketTimeout(5);

  if (!mqtt.setBufferSize(1024)) {
    Serial.println(
        "ERROR: alokasi buffer MQTT gagal."
    );
  }

  lastSampleMs = millis();

  Serial.println("SMART ENERGY ANOMALY DETECTOR");
  Serial.println(
      "Biarkan daya normal selama 10 sampel / sekitar 20 detik."
  );

  drawOLED();
}

// =====================================================
// 13. MAIN LOOP
// =====================================================
void loop() {
  uint32_t now = millis();

  handleButton(now);

  if (now - lastSampleMs >= SAMPLE_INTERVAL_MS) {
    lastSampleMs = now;

    float power =
        50.0f + analogRead(POT_PIN) * (450.0f / 4095.0f);

    model.sample(power, now);

    if (!model.anomaly) {
      alarmSilenced = false;
    }

    updateOutputs(now);
    publishReading(now);
  }

  updateOutputs(now);

  if (now - lastOledMs >= 500) {
    lastOledMs = now;
    drawOLED();
  }

  maintainConnections(now);

  delay(2);
}