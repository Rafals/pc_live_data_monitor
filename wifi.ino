#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- ZMIEŃ TE WARTOŚCI ---
const char* ssid = "x";    // Nazwa Twojego Wi-Fi
const char* password = "x"; // Hasło do Wi-Fi
String serverUrl = "x"; // IP komputera i port LHM
int jsonBufferSize = 46080;

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Start Kodu Diagnostycznego ---");

  WiFi.begin(ssid, password);
  Serial.print("Lacze z WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nPolaczono!");
  Serial.print("Adres IP ESP32: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    
    HTTPClient http;
    http.begin(serverUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        
        WiFiClient& stream = http.getStream();

        DynamicJsonDocument doc(jsonBufferSize);

        // ======================= NOWA LINIA 1 =======================
        // Twój JSON ma 11+ poziomów. Ustawmy limit na 15.
        DeserializationOption::NestingLimit nestingLimit(15);
        // ==========================================================

        // Parsuj JSON, podając nową opcję limitu zagnieżdżenia
        // ======================= NOWA LINIA 2 (ZMIENIONA) ===========
        DeserializationError error = deserializeJson(doc, stream, nestingLimit);
        // ==========================================================


        if (error) {
          Serial.print(F("Blad parsowania JSON: "));
          Serial.println(error.c_str());
          
          if (error == DeserializationError::NoMemory) {
             Serial.println(F("BLAD: Za malo pamieci dla JSONa!"));
          } else if (error == DeserializationError::TooDeep) {
             Serial.println(F("BLAD: Zbyt glebokie zagniezdzanie. Sprobuj zwiekszyc limit."));
          }
          
          http.end();
          delay(5000);
          return;
        }

        Serial.println("Parsowanie JSON powiodlo sie!");

        const char* vCPU = "NIE ZNALEZIONO";
        const char* wCPU = "NIE ZNALEZIONO";
        const char* mhzCPU = "NIE ZNALEZIONO";
        const char* tempCPU = "NIE ZNALEZIONO";
        const char* loadCPU = "NIE ZNALEZIONO";
        const char* percentMemory = "NIE ZNALEZIONO";
        const char* usedGbMemory = "NIE ZNALEZIONO";
        const char* wGPU = "NIE ZNALEZIONO";
        const char* coreMHzGPU = "NIE ZNALEZIONO";
        const char* memoryMHzGPU = "NIE ZNALEZIONO";
        const char* tempCoreGPU = "NIE ZNALEZIONO";
        const char* tempHotSpotGPU = "NIE ZNALEZIONO";
        const char* loadGPU = "NIE ZNALEZIONO";
        const char* loadMemoryGPU = "NIE ZNALEZIONO";
        const char* usedMemoryGPU = "NIE ZNALEZIONO";


        for (JsonObject c1 : doc["Children"].as<JsonArray>()) {
          for (JsonObject c2 : c1["Children"].as<JsonArray>()) {
            for (JsonObject c3 : c2["Children"].as<JsonArray>()) { 
              for (JsonObject c4 : c3["Children"].as<JsonArray>()) { 
              // CPU
              if (c4["id"].as<int>() == 43) { 
                    vCPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 49) { 
                    wCPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 54) { 
                    mhzCPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 68) { 
                    tempCPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 71) { 
                    loadCPU = c4["Value"];
                  }
              //Memory
              if (c4["id"].as<int>() == 79) { 
                    percentMemory = c4["Value"];
                  }
              if (c4["id"].as<int>() == 82) { 
                    usedGbMemory = c4["Value"];
                  }
              //GPU
              if (c4["id"].as<int>() == 88) { 
                    wGPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 90) { 
                    coreMHzGPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 91) { 
                    memoryMHzGPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 93) { 
                    tempCoreGPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 94) { 
                    tempHotSpotGPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 96) { 
                    loadGPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 99) { 
                    loadMemoryGPU = c4["Value"];
                  }
              if (c4["id"].as<int>() == 123) { 
                    usedMemoryGPU = c4["Value"];
                  }
              }
            }
          }
          break;
        }
        
        Serial.print("V CPU: "); Serial.println(vCPU);
        Serial.print("W CPU: "); Serial.println(wCPU);
        Serial.print("MHz CPU: "); Serial.println(mhzCPU);
        Serial.print("Temp CPU: "); Serial.println(tempCPU);
        Serial.print("Load CPU: "); Serial.println(loadCPU);
        Serial.print("% Memory: "); Serial.println(percentMemory);
        Serial.print("Used Memory: "); Serial.print(usedGbMemory); Serial.println("/28 GB");
        Serial.print("Wat GPU: "); Serial.println(wGPU);
        Serial.print("MHz GPU: "); Serial.println(coreMHzGPU);
        Serial.print("Temp GPU: "); Serial.println(tempCoreGPU);
        Serial.print("Temp Hot Spot GPU: "); Serial.println(tempHotSpotGPU);
        Serial.print("Load GPU: "); Serial.println(loadGPU);
        Serial.print("Load Memory GPU: "); Serial.println(loadMemoryGPU);
        Serial.print("Used Memory GPU: "); Serial.print(usedMemoryGPU); Serial.println("/4096 MB");


      } else {
        Serial.printf("[HTTP] Blad GET, kod: %d\n", httpCode);
      }
    } else {
      Serial.printf("[HTTP] Blad polaczenia.\n");
    }
    http.end();
    
  } else {
    Serial.println("Utracono połączenie WiFi.");
    WiFi.begin(ssid, password);
  }

  delay(1000);
}
