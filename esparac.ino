
//////////////////////////////////////////////
//      RemoteXY include bölümü             //
//////////////////////////////////////////////

#define REMOTEXY_MODE__WIFI_POINT
#include <WiFi.h>
#include <RemoteXY.h>

#define REMOTEXY_WIFI_SSID     "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT   6377

#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] = {
  255,2,0,9,0,109,0,19,0,0,0,0,31,2,106,200,200,84,1,1,
  6,0,129,7,21,41,12,47,3,28,5,64,6,97,108,101,118,32,100,117,
  114,117,109,117,0,129,63,21,24,12,21,16,17,5,64,88,104,97,114,101,
  107,101,116,0,5,207,43,143,143,8,24,47,47,0,2,26,31,70,26,36,
  43,43,78,2,7,7,16,136,37,0,67,82,9,21,24,164,5,32,8,78,
  2,26,2,67,81,36,17,19,164,15,32,8,78,2,26,2
};

struct {
  int8_t  joystick_01_x;   // -100 … 100
  int8_t  joystick_01_y;   // -100 … 100
  uint8_t led_01;          // var ama kullanmıyoruz
  float   value_01;        // sol teker yönü (-1,0,1)
  float   value_02;        // sağ teker yönü (-1,0,1)
  uint8_t connect_flag;
} RemoteXY;
#pragma pack(pop)

//////////////////////////////////////////////
//            Pin tanımları                 //
//////////////////////////////////////////////
#define IN1_PIN 18
#define IN2_PIN 19
#define IN3_PIN 21
#define IN4_PIN 22

// ──────────────────────────────────────────
void setMotorDir(int leftDir, int rightDir);

void setup() {
  RemoteXY_Init();

  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  setMotorDir(0, 0);               // duruş
}

void loop() {
  RemoteXY_Handler();              // wifi trafiğini yönet

  // Joystick verisini oku ▸ yön bilgisine çevir
  int x = RemoteXY.joystick_01_x;  // sol -100, sağ +100
  int y = -RemoteXY.joystick_01_y; // yukarı +100, aşağı -100

  // Ölü bölge: küçük titreşimleri yok say
  const int DEAD  = 20;
  int leftMix  = y + x;            // diferansiyel karışım
  int rightMix = y - x;

  int leftDir  = (leftMix  >  DEAD) ?  1 : (leftMix  < -DEAD ? -1 : 0);
  int rightDir = (rightMix >  DEAD) ?  1 : (rightMix < -DEAD ? -1 : 0);

  setMotorDir(leftDir, rightDir);

  // Ekranda göstermek için
  RemoteXY.value_01 = leftDir;
  RemoteXY.value_02 = rightDir;

  // Gerekirse çok küçük gecikme
  // RemoteXY_delay(5);
}

// ──────────────────────────────────────────
// Dir: 1 = ileri, -1 = geri, 0 = dur
void setMotorDir(int leftDir, int rightDir) {
  // Sol
  if (leftDir == 1)      { digitalWrite(IN1_PIN, HIGH); digitalWrite(IN2_PIN, LOW); }
  else if (leftDir == -1){ digitalWrite(IN1_PIN, LOW);  digitalWrite(IN2_PIN, HIGH); }
  else                   { digitalWrite(IN1_PIN, LOW);  digitalWrite(IN2_PIN, LOW); }

  // Sağ
  if (rightDir == 1)      { digitalWrite(IN3_PIN, HIGH); digitalWrite(IN4_PIN, LOW); }
  else if (rightDir == -1){ digitalWrite(IN3_PIN, LOW);  digitalWrite(IN4_PIN, HIGH); }
  else                    { digitalWrite(IN3_PIN, LOW);  digitalWrite(IN4_PIN, LOW); }
}
