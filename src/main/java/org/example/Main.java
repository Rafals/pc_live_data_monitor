package org.example;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fazecast.jSerialComm.SerialPort;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.charset.StandardCharsets;
import java.time.Duration;
import java.util.HashMap;
import java.util.Map;

public class Main {
    // --- KONFIGURACJA ---
    private static final String COM_PORT = "COM6"; // ZMIEŃ NA SWÓJ PORT
    private static final int BAUD_RATE = 115200;
    private static final String LHM_URL = "http://127.0.0.1:8085/data.json";
    private static final int INTERVAL_MS = 200; // 200ms

    // Lista ID w kolejności (musi pasować do kodu C++ na ESP32)
    private static final int[] IDS_TO_FIND = {
            43, 49, 54, 68, 71,      // CPU
            79, 82,                  // RAM
            88, 90, 91, 93, 94, 96, 99, 123 // GPU
    };

    private static final HttpClient client = HttpClient.newHttpClient();
    private static final ObjectMapper mapper = new ObjectMapper();

    public static void main(String[] args) {
        System.out.println("Start aplikacji Java Monitor...");

        // 1. Konfiguracja Portu Serial
        SerialPort comPort = SerialPort.getCommPort(COM_PORT);
        comPort.setBaudRate(BAUD_RATE);
        comPort.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0);

        if (!comPort.openPort()) {
            System.err.println("KRYTYCZNY BŁĄD: Nie można otworzyć portu " + COM_PORT);
            System.err.println("Sprawdź czy Monitor w Arduino IDE jest zamknięty!");
            return;
        }
        System.out.println("Połączono z portem " + COM_PORT);

        // Wątek do odczytywania odpowiedzi z ESP32 (nasłuchuje w tle)
        startSerialReader(comPort);

        try {
            while (true) {
                long startTime = System.currentTimeMillis();

                try {
                    // 2. Pobranie JSON z LHM
                    String jsonResponse = fetchJson();

                    // 3. Parsowanie i szukanie wartości
                    JsonNode rootNode = mapper.readTree(jsonResponse);
                    Map<Integer, String> values = new HashMap<>();
                    findValuesRecursive(rootNode, values);

                    // 4. Budowanie stringa dla ESP32
                    StringBuilder sb = new StringBuilder();
                    for (int id : IDS_TO_FIND) {
                        // Domyślnie "N/A" jeśli nie znaleziono
                        String val = values.getOrDefault(id, "N/A");
                        sb.append(val).append(";");
                    }
                    sb.append("\n"); // Znak końca linii

                    // 5. Wysyłanie
                    byte[] dataToSend = sb.toString().getBytes(StandardCharsets.UTF_8);
                    comPort.writeBytes(dataToSend, dataToSend.length);

                    System.out.println("--- Wysłano dane (" + dataToSend.length + " bytes) ---");

                } catch (Exception e) {
                    System.err.println("[BŁĄD PC] " + e.getMessage());
                }

                // 6. Inteligentne czekanie
                long processingTime = System.currentTimeMillis() - startTime;
                long sleepTime = INTERVAL_MS - processingTime;

                if (sleepTime > 0) {
                    Thread.sleep(sleepTime);
                } else {
                    System.out.println("!!! OSTRZEŻENIE: Przetwarzanie trwało za długo: " + processingTime + "ms");
                }
            }
        } catch (InterruptedException e) {
            System.out.println("Zatrzymano aplikację.");
        } finally {
            comPort.closePort();
        }
    }

    // Pobieranie JSON przez HTTP
    private static String fetchJson() throws Exception {
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(LHM_URL))
                .timeout(Duration.ofSeconds(2))
                .GET()
                .build();

        HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
        if (response.statusCode() != 200) {
            throw new RuntimeException("HTTP Error: " + response.statusCode());
        }
        return response.body();
    }

    // Rekurencyjne przeszukiwanie drzewa JSON
    private static void findValuesRecursive(JsonNode node, Map<Integer, String> results) {
        // Sprawdź czy ten węzeł ma ID, którego szukamy
        if (node.has("id") && node.has("Value")) {
            int id = node.get("id").asInt();
            // Sprawdzamy czy to ID jest na naszej liście celów (prosta optymalizacja)
            if (isTargetId(id)) {
                results.put(id, node.get("Value").asText());
            }
        }

        // Jeśli ma dzieci, wejdź głębiej
        if (node.has("Children")) {
            for (JsonNode child : node.get("Children")) {
                findValuesRecursive(child, results);
            }
        }
    }

    // Pomocnicza: czy szukamy tego ID?
    private static boolean isTargetId(int id) {
        for (int target : IDS_TO_FIND) {
            if (target == id) return true;
        }
        return false;
    }

    // Oddzielny wątek do czytania co ESP32 do nas gada
    private static void startSerialReader(SerialPort port) {
        Thread readerThread = new Thread(() -> {
            try {
                while (port.isOpen()) {
                    if (port.bytesAvailable() > 0) {
                        byte[] readBuffer = new byte[1024];
                        int numRead = port.readBytes(readBuffer, readBuffer.length);
                        if (numRead > 0) {
                            String msg = new String(readBuffer, 0, numRead, StandardCharsets.UTF_8);
                            System.out.print("[ESP32]: " + msg); // print zamiast println bo msg może mieć własne \n
                        }
                    }
                    Thread.sleep(100);
                }
            } catch (Exception e) { e.printStackTrace(); }
        });
        readerThread.setDaemon(true);
        readerThread.start();
    }
}
