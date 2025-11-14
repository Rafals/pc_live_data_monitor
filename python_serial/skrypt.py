import serial
import requests
import time

# --- Konfiguracja ---
PORT_COM = "COM6"  # Upewnij się, że to Twój port COM
# Wracamy do 115200, bo wysyłamy teraz malutkie dane. Jest stabilniejsze.
PREDKOSC_BAUD = 115200 
URL_LHM = "http://127.0.0.1:8085/data.json"
INTERWAL = 0.2 # Teraz możemy celować w 500ms!

# Lista ID, których szukamy (w kolejności)
IDS_DO_ZNALEZIENIA = [
    43,  # vCPU
    49,  # wCPU
    54,  # mhzCPU
    68,  # tempCPU
    71,  # loadCPU
    79,  # percentMemory
    82,  # usedGbMemory
    88,  # wGPU
    90,  # coreMHzGPU
    91,  # memoryMHzGPU
    93,  # tempCoreGPU
    94,  # tempHotSpotGPU
    96,  # loadGPU
    99,  # loadMemoryGPU
    123  # usedMemoryGPU
]
# --------------------

print(f"Start skryptu. Probuje polaczyc z {PORT_COM} (predkosc {PREDKOSC_BAUD})...")
esp_serial = None 

try:
    esp_serial = serial.Serial(PORT_COM, PREDKOSC_BAUD, timeout=0.1) 
    print(f"Polaczono. Rozpoczynam petle (cykl {INTERWAL}s).")
    
    while True:
        start_time = time.time()
        
        # 1. POBIERZ I SPARSUJ JSON (NA PC)
        try:
            response = requests.get(URL_LHM)
            response.raise_for_status()
            data = response.json() # Od razu parsujemy do obiektu Python
            
            # Słownik do przechowywania znalezionych wartości
            wartosci = {id_szukane: "N/A" for id_szukane in IDS_DO_ZNALEZIENIA}

            # 2. ZNAJDŹ WARTOŚCI (Logika skopiowana z C++)
            # Używamy .get('Children', []) aby uniknąć błędów, jeśli klucz nie istnieje
            for c1 in data.get('Children', []):
                for c2 in c1.get('Children', []):
                    for c3 in c2.get('Children', []):
                        for c4 in c3.get('Children', []):
                            current_id = c4.get('id')
                            if current_id in wartosci:
                                wartosci[current_id] = c4.get('Value', 'N/A')
            
            # 3. SFORMATUJ PROSTY STRING DLA ESP32
            # Stwórz string w kolejności z listy IDS_DO_ZNALEZIENIA
            string_do_wyslania = ""
            for id_szukane in IDS_DO_ZNALEZIENIA:
                string_do_wyslania += wartosci[id_szukane] + ";"
            
            # Dodaj znak nowej linii jako znacznik końca
            string_do_wyslania += "\n"

            # 4. WYŚLIJ MAŁY STRING DO ESP32
            esp_serial.write(string_do_wyslania.encode('utf-8'))
            print(f"\n--- Wyslano {len(string_do_wyslania)} bajtow ---")

        except requests.exceptions.RequestException as e:
            print(f"[BŁĄD PC] Nie mozna pobrac danych z LHM: {e}")
        except serial.SerialException as e:
            print(f"[BŁĄD PC] Blad zapisu na port COM: {e}")
        except Exception as e:
            print(f"[BŁĄD PC] Niespodziewany blad: {e}")

        # 5. ODCZYTYWANIE ODPOWIEDZI Z ESP32 (jak wcześniej)
        while esp_serial.in_waiting > 0:
            try:
                linia_od_esp = esp_serial.readline().decode('utf-8').strip()
                if linia_od_esp:
                    print(f"[ESP32 MÓWI]: {linia_od_esp}")
            except UnicodeDecodeError:
                pass 

        # 6. INTELIGENTNE CZEKANIE (jak wcześniej)
        czas_przetwarzania = time.time() - start_time
        czas_pauzy = INTERWAL - czas_przetwarzania
        
        if czas_pauzy < 0:
            print(f"!!! OSTRZEZENIE: Przetwarzanie trwało za długo ({czas_przetwarzania:.3f}s)")
            czas_pauzy = 0
            
        print(f"--- Czas przetwarzania: {czas_przetwarzania:.3f}s. Czekam {czas_pauzy:.3f}s. ---")
        time.sleep(czas_pauzy)

except serial.SerialException as e:
    print(f"KRYTYCZNY BŁĄD: Nie mozna otworzyc portu {PORT_COM}.")
    print(f"Upewnij sie, ze MONITOR ARDUINO IDE JEST ZAMKNIETY.")
except KeyboardInterrupt:
    print("\nZatrzymano przez uzytkownika.")
finally:
    if esp_serial and esp_serial.is_open:
        esp_serial.close()
        print("Zamknieto port COM.")