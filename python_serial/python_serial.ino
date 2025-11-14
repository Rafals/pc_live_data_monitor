// --- Koniec z ArduinoJson.h! ---
// #include <ArduinoJson.h> // NIE POTRZEBUJEMY TEGO

// Funkcja pomocnicza do wyciągania N-tego elementu ze stringa
// Działa jak "split" i pobranie elementu
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "N/A";
}

void setup() {
  // Wracamy do 115200, bo dane są małe
  Serial.begin(115200); 
  // Nie potrzebujemy już setTimeout
  Serial.println("\n--- Start - Czekam na dane z PC (Tryb Prosty) ---");
}

void loop() {
  
  // Sprawdź, czy dane przyszły z komputera przez USB
  if (Serial.available() > 0) {
    
    // 1. Odczytaj dane aż do znaku nowej linii '\n'
    String payload = Serial.readStringUntil('\n');
    payload.trim(); // Usuń ewentualne białe znaki

    if (payload.length() == 0) {
        return; // Pusty odczyt, zignoruj
    }

    // 2. Parsowanie prostego stringa
    // Używamy naszej funkcji pomocniczej.
    // KOLEJNOŚĆ MUSI BYĆ TAKA SAMA JAK W LIŚCIE 'IDS_DO_ZNALEZIENIA' w Pythonie
    
    String vCPU           = getValue(payload, ';', 0);
    String wCPU           = getValue(payload, ';', 1);
    String mhzCPU         = getValue(payload, ';', 2);
    String tempCPU        = getValue(payload, ';', 3);
    String loadCPU        = getValue(payload, ';', 4);
    String percentMemory  = getValue(payload, ';', 5);
    String usedGbMemory   = getValue(payload, ';', 6);
    String wGPU           = getValue(payload, ';', 7);
    String coreMHzGPU     = getValue(payload, ';', 8);
    String memoryMHzGPU   = getValue(payload, ';', 9);
    String tempCoreGPU    = getValue(payload, ';', 10);
    String tempHotSpotGPU = getValue(payload, ';', 11);
    String loadGPU        = getValue(payload, ';', 12);
    String loadMemoryGPU  = getValue(payload, ';', 13);
    String usedMemoryGPU  = getValue(payload, ';', 14);


    // 3. Wyświetl dane (tak jak wcześniej)
    // (Wysyłamy je z powrotem do Pythona, aby widzieć je w konsoli)
    Serial.println("--- Odebrano i przetworzono dane ---");
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
  }
  
  // Pętla loop() jest pusta. Czeka na dane.
}