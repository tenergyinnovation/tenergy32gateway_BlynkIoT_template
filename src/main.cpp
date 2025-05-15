/***********************************************************************
 * Project      :     tenergy32gateway_Blynk
 * Description  :     Test program for Tenergy32 Gateway board
 * Revision     :     1.1
 * Rev1.0       :     Original
 * Rev1.1       :     แก้ไขลำดับ parameter ของฟังก์ชั่น begin() [2025-05-15]
 * Hardware     :     Tenergy32GateWay
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     14/05/2025
 ***********************************************************************/
#include <Arduino.h>
#include <tenergy32gateway.h>
#include <esp_task_wdt.h>
#include <esp_system.h> // สำหรับ esp_read_mac

// —– Blynk Template & Device settings (จาก Console) —–
#define BLYNK_TEMPLATE_ID "BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "BLYNK_TEMPLATE_NAME"
#define BLYNK_DEVICE_NAME "BLYNK_DEVICE_NAME"

// —– Blynk Authentication —–
#define BLYNK_AUTH_TOKEN "BLYNK_AUTH_TOKEN"

// ===== เลือกโหมดการเชื่อมต่อ Blynk =====
// #define USE_ETHERNET    // เลือกเชื่อมต่อผ่าน Ethernet
#define USE_WIFI // เลือกเชื่อมต่อผ่าน WiFi

#ifdef USE_ETHERNET
#include <BlynkSimpleEthernet.h>
#elif defined(USE_WIFI)
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
// กำหนด WiFi SSID และ Password
#define WIFI_SSID "WiFi_SSID"
#define WIFI_PASS "WiFi_PASS"
#endif

Tenergy32GateWay mcu;
String unitName = "";

/***********************************************************************
 * FUNCTION:    getUnitNameFromMac
 * DESCRIPTION: สร้างชื่อ unitName จาก MAC Address (6 ตัวหลัง)
 * RETURNED:    String ชื่อบอร์ด tenergy32gateway-xxxxxx
 ***********************************************************************/
String getUnitNameFromMac()
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char macStr[7];
    snprintf(macStr, sizeof(macStr), "%02X%02X%02X", mac[3], mac[4], mac[5]);
    return "tenergy32gateway-" + String(macStr);
}

void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     tenergy32gateway_Blynk\r\n");
    Serial.printf("* Description  :     Template coding for Tenergy32GateWay on PlatformIO\r\n");
    Serial.printf("* Hardware     :     Tenergy32GateWay\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     14/05/2025\r\n");
    Serial.printf("* Revision     :     %s\r\n", mcu._version.c_str());
    Serial.printf("* website      :     http://www.tenergyinnovation.co.th\r\n");
    Serial.printf("* Email        :     uten.boonliam@tenergyinnovation.co.th\r\n");
    Serial.printf("* TEL          :     +66 89-140-7205\r\n");
    Serial.printf("***********************************************************************/\r\n");
}

void setup()
{
    Serial.begin(115200);
    header_print();

#ifdef USE_ETHERNET
    if (!mcu.begin(true, 433E6)) // true = ใช้ Ethernet
#elif defined(USE_WIFI)
    if (!mcu.begin(false, 433E6)) // false = ไม่ใช้ Ethernet
#endif
    {
        Serial.println("Tenergy32 Gateway initialization failed!");
        mcu.displayOLED("Board init FAIL!");
        while (1)
            ;
    }
    else
    {
        Serial.println("Tenergy32 Gateway initialization OK!");
        mcu.displayOLED("Board init OK!");
    }

#ifdef USE_ETHERNET
    // —– เชื่อม Blynk.Cloud ผ่าน Ethernet —–
    Serial.println("Connecting to Blynk.Cloud via Ethernet...");
    mcu.displayOLED("Connecting Blynk (ETH)...");
    Blynk.begin(BLYNK_AUTH_TOKEN);
#elif defined(USE_WIFI)
    // —– เชื่อม Blynk.Cloud ผ่าน WiFi —–
    Serial.println("Connecting to WiFi...");
    mcu.displayOLED("Connecting WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    int wifi_retry = 0;
    while (WiFi.status() != WL_CONNECTED && wifi_retry < 20)
    {
        delay(1000);
        Serial.print(".");
        wifi_retry++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi Connected!");
        mcu.displayOLED("WiFi Connected!");

        // แสดงค่า IP Address ที่ Serial
        Serial.print("WiFi IP Address: ");
        Serial.println(WiFi.localIP());

        // แสดงค่า IP Address ที่ OLED
        char ipStr[20];
        snprintf(ipStr, sizeof(ipStr), "IP: %s", WiFi.localIP().toString().c_str());
        mcu.displayOLED(ipStr);
        mcu.beep(2, 100); // Beep 2 ครั้ง
        delay(1500); // แสดง IP สักครู่
    }
    else
    {
        Serial.println("\nWiFi Connect FAIL!");
        mcu.displayOLED("WiFi FAIL!");
        while (1)
            ;
    }

    Serial.println("Connecting to Blynk.Cloud...");
    mcu.displayOLED("Connecting Blynk...");
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
#endif

    // ตรวจสอบสถานะการเชื่อมต่อ Blynk
    int _retry = 0;
    while (!Blynk.connected() && _retry < 20)
    { // รอไม่เกิน 20 วินาที
        delay(1000);
        _retry++;
    }
    if (Blynk.connected())
    {
        Serial.println("Blynk Connected!");
        mcu.displayOLED("Blynk Connected!");
    }
    else
    {
        Serial.println("Blynk Connect FAIL!");
        mcu.displayOLED("Blynk FAIL!");
        while (1)
            ;
    }

    delay(1000);

    unitName = getUnitNameFromMac();
    Serial.printf("unitName: %s\r\n", unitName.c_str());
    mcu.displayOLED(unitName.c_str());

    esp_task_wdt_init(10, true);
    esp_task_wdt_add(NULL);
}

void loop()
{
    Blynk.run();
    esp_task_wdt_reset();

    // สุ่มค่าและส่งขึ้น Blynk
    int volt = random(0, 251);        // 0-250 V
    int current = random(0, 101);     // 0-100 A
    int power = random(0, 1001);      // 0-1000 W
    int vibration = random(0, 101);   // 0-100 rpm

    Blynk.virtualWrite(V1, volt);
    Blynk.virtualWrite(V2, current);
    Blynk.virtualWrite(V3, power);
    Blynk.virtualWrite(V6, vibration);

    delay(1000); // ส่งข้อมูลทุก 1 วินาที
}