#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define WIFI_SSID "########"
#define WIFI_PASSWORD "########"
#define WIFI_MAX_CONN_RETRY 5
#define WIFI_SCAN_AUTH_MODE "WPA2 PSK"
#define WIFI_SAE_MODE "BOTH" // Use "Hunt and Peck" and "H2E"
#define WIFI_H2E_IDENTIFIER ""

static const char *TAG = "socket";
static int retry_num = 0;

void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers for WIFI_EVENT and IP_EVENT
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    // define wifi station configuration
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_SCAN_AUTH_MODE,
            .sae_pwe_h2e = WIFI_SAE_MODE,
            .sae_h2e_identifier = WIFI_H2E_IDENTIFIER,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

void try_connection(void)
{
    while (retry_num < WIFI_MAX_CONN_RETRY)
    {
        ESP_LOGI(TAG, "retrying AP connection");
        esp_wifi_connect();
        retry_num++;
    }
}

// static void wifi_event_handler(void* arg, esp_event_base_t event_base,
//                                 int32_t event_id, void* event_data)
// {
//     ESP_LOGI(TAG, "wifi event handler: %"PRIi32, event_id);
//     switch(event_id) {
//         case WIFI_EVENT_STA_START:
//             ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
//             break;
//         case WIFI_EVENT_AP_STACONNECTED:
//             ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
//             if (wifi_events) {
//                 xEventGroupSetBits(wifi_events, WIFI_AP_STA_CONNECTED);
//             }
//             break;
//         case WIFI_EVENT_STA_CONNECTED:
//             ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
//             if (wifi_events) {
//                 xEventGroupSetBits(wifi_events, WIFI_AP_STA_CONNECTED);
//             }
//             break;
//         case WIFI_EVENT_STA_DISCONNECTED:
//             ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
//             if (! (EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT & wifi_event_handler_flag) ) {
//                 TEST_ESP_OK(esp_wifi_connect());
//             }
//             if (wifi_events) {
//                 xEventGroupSetBits(wifi_events, WIFI_DISCONNECT_EVENT);
//             }
//             break;
//         default:
//             break;
//     }
//     return;
// }

void app_main(void)
{
    // Initialize non-volatile storage partition
    esp_err_t returnStatus = nvs_flash_init();
    if (returnStatus == ESP_ERR_NVS_NO_FREE_PAGES || returnStatus == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // Erase non-volatile storage partition
        ESP_ERROR_CHECK(nvs_flash_erase());
        returnStatus = nvs_flash_init();
    }
    ESP_ERROR_CHECK(returnStatus); // Check if erase was successful

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}