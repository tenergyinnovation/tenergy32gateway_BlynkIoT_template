/***********************************************************************
 * Project      :     tenergy32gateway_Blynk
 * Description  :     Test program for Tenergy32 Gateway board
 * Hardware     :     Tenergy32GateWay
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     27/04/2025
 ***********************************************************************/
#include <Arduino.h>
#include <tenergy32gateway.h>
#include <esp_task_wdt.h>
#include <esp_system.h> // สำหรับ esp_read_mac

// —– Blynk Template & Device settings (จาก Console) —–
#define BLYNK_TEMPLATE_ID "TMPL6Elg-Y3N3"    // Template ID ที่คุณเห็นใน Console
#define BLYNK_TEMPLATE_NAME "FactoryMonitor" // ชื่อ Template ตามที่ตั้งไว้
#define BLYNK_DEVICE_NAME "ESP32-Gateway"    // ตั้งชื่ออุปกรณ์ใน Console (Device Name)

// —– Blynk Authentication —–
#define BLYNK_AUTH_TOKEN "7OGFKzhWJU7VXE_H8L-PxEgexup-Zkqf" // ได้มาหลังจาก Add Device ใน Console
#include <BlynkSimpleEthernet.h>                            // Blynk ใช้ Ethernet

Tenergy32GateWay mcu;

// ตัวแปรสำหรับเก็บชื่อ unitName
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
    Serial.printf("* Date         :     27/04/2025\r\n");
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

    if (!mcu.begin())
    {
        Serial.println("Board initialization failed!");
        while (1)
            ;
    }

    // —– เชื่อม Blynk.Cloud —–
    Serial.println("Connecting to Blynk.Cloud...");
    mcu.displayOLED("Connecting Blynk...");
    Blynk.begin(BLYNK_AUTH_TOKEN);

    // ตรวจสอบสถานะการเชื่อมต่อ
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
            ; // หยุดโปรแกรม
    }

    // Delay to view initial info
    delay(1000);

    // สร้าง unitName จาก MAC Address
    unitName = getUnitNameFromMac();

    // แสดงชื่อ unitName บน Serial และ OLED
    Serial.printf("unitName: %s\r\n", unitName.c_str());
    mcu.displayOLED(unitName.c_str());

    // Initialize watchdog timer
    esp_task_wdt_init(10, true);
    esp_task_wdt_add(NULL);
}

void loop()
{
    // รัน Blynk และรอรับแพ็กเก็ต LoRa
    Blynk.run();
    esp_task_wdt_reset();
    delay(1000);
}
