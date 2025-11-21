#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX = 0, touchY = 0;

    bool touched = false;//tft.getTouch( &touchX, &touchY, 600 );

    if( !touched )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print( "Data x " );
        Serial.println( touchX );

        Serial.print( "Data y " );
        Serial.println( touchY );
    }
}

//funkcja do parsingu
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


void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 3 ); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );

    ui_init();

    Serial.println( "Setup done" );
}

void loop()
{
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
    
    String vCPU           = getValue(payload, ';', 0) + " V";
    String wCPU           = getValue(payload, ';', 1) + " W";
    String mhzCPU         = getValue(payload, ';', 2) + " MHz";
    String tempCPU        = getValue(payload, ';', 3) + "°C";
    String loadCPU        = getValue(payload, ';', 4) + "%%";
    String percentMemory  = getValue(payload, ';', 5) + "%%";
    String usedGbMemory   = getValue(payload, ';', 6) + "/28 GB";
    String wGPU           = getValue(payload, ';', 7) + "W";
    String coreMHzGPU     = getValue(payload, ';', 8) + " MHz";
    String memoryMHzGPU   = getValue(payload, ';', 9) + " MHz";
    String tempCoreGPU    = getValue(payload, ';', 10) + "°C";
    String tempHotSpotGPU = getValue(payload, ';', 11) + "°C";
    String loadGPU        = getValue(payload, ';', 12) + "%%";
    String loadMemoryGPU  = getValue(payload, ';', 13) + "%%";
    String usedMemoryGPU  = getValue(payload, ';', 14) + "/4096 MB";

    //CPU
    lv_label_set_text_fmt(ui_tempCPU, tempCPU.c_str());
    lv_label_set_text_fmt(ui_usageCPU, loadCPU.c_str());
    lv_label_set_text_fmt(ui_wCPU, wCPU.c_str());
    lv_label_set_text_fmt(ui_mhzCPU, mhzCPU.c_str());
    lv_label_set_text_fmt(ui_vCPU, vCPU.c_str());
    //GPU
    lv_label_set_text_fmt(ui_tempGPU, tempCoreGPU.c_str());
    lv_label_set_text_fmt(ui_tempGPUH, tempHotSpotGPU.c_str());
    lv_label_set_text_fmt(ui_usageGPU, loadGPU.c_str());
    lv_label_set_text_fmt(ui_wGPU, wGPU.c_str());
    lv_label_set_text_fmt(ui_mhzGPU, coreMHzGPU.c_str());
    lv_label_set_text_fmt(ui_mUsageGPU, loadMemoryGPU.c_str());
    lv_label_set_text_fmt(ui_mbUsageGPU, usedMemoryGPU.c_str());
    //RAM
    lv_label_set_text_fmt(ui_memoryUsage, usedGbMemory.c_str());
  }
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
  
}
