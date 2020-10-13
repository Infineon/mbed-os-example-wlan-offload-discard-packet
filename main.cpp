/******************************************************************************
 * File Name: main.cpp
 *
 * Description:
 *   This application demonstrates WLAN discard packet filter which is part of
 *   packet filter offload functionality. By default, the application is
 *   configured to discard ICMP packets trying to reach the host but allows
 *   all other packet types. The host stays in deep sleep during the ICMP
 *   requests from peer devices in the network. The WLAN simply discard the
 *   ICMP packets.
 *
 * Related Document: README.md
 *                   AN227910 Low-Power System Design with CYW43012 and PSoC 6
 *
 ******************************************************************************
 * Copyright (2019-2020), Cypress Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 * This software, including source code, documentation and related materials
 * (“Software”), is owned by Cypress Semiconductor Corporation or one of its
 * subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
 * protection (United States and foreign), United States copyright laws and
 * international treaty provisions. Therefore, you may use this Software only
 * as provided in the license agreement accompanying the software package from
 * which you obtained this Software (“EULA”).
 *
 * If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
 * non-transferable license to copy, modify, and compile the Software source
 * code solely for use in connection with Cypress’s integrated circuit products.
 * Any reproduction, modification, translation, compilation, or representation
 * of this Software except as specified above is prohibited without the express
 * written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death (“High Risk Product”). By
 * including Cypress’s product in a High Risk Product, the manufacturer of such
 * system or application assumes all risk of such use and in doing so agrees to
 * indemnify Cypress against all liability.
 *****************************************************************************/

#include "mbed.h"
#include "WhdSTAInterface.h"
#include "network_activity_handler.h"

/******************************************************************************
 *                                MACROS
 *****************************************************************************/
/* This macro specifies the interval in milliseconds that the device monitors
 * the network for inactivity. If the network is inactive for duration lesser
 * than NETWORK_INACTIVE_WINDOW_MS in this interval, the MCU does not suspend
 * the network stack and informs the calling function that the MCU wait period
 * timed out while waiting for network to become inactive.
 */
#define NETWORK_INACTIVE_INTERVAL_MS   (500)

/* This macro specifies the continuous duration in milliseconds for which the
 * network has to be inactive. If the network is inactive for this duaration,
 * the MCU will suspend the network stack. Now, the MCU will not need to service
 * the network timers which allows it to stay longer in sleep/deepsleep.
 */
#define NETWORK_INACTIVE_WINDOW_MS     (250)

#define APP_INFO(x)                do { printf("Info: "); printf x; } while(0);
#define ERR_INFO(x)                do { printf("Error: "); printf x; } while(0);

#define PRINT_AND_ASSERT(result, msg, args...)   \
                                   do                                 \
                                   {                                  \
                                       if (CY_RSLT_SUCCESS != result) \
                                       {                              \
                                           ERR_INFO((msg, ## args));  \
                                           MBED_ASSERT(0);            \
                                       }                              \
                                   } while(0);

/******************************************************************************
 *                       GLOBAL VARIABLES
 *****************************************************************************/
/* Wi-Fi (STA) object handle. */
WhdSTAInterface *wifi;

/******************************************************************************
 *                     FUNCTION DEFINITIONS
 *****************************************************************************/
/******************************************************************************
 * Function Name: app_wl_connect
 ******************************************************************************
 * Summary:
 *   This function tries to connect the kit to the given AP (Access Point).
 *
 * Parameters:
 *   wifi: A pointer to WLAN interface whose emac activity is being monitored.
 *   ssid: Wi-Fi AP SSID.
 *   pwd: Wi-Fi AP Password.
 *   secutiry: Wi-Fi security type as defined in structure nsapi_security_t.
 *
 * Return:
 *   cy_rslt_t: Returns CY_RSLT_SUCCESS or CY_RSLT_TYPE_ERROR indicating
 *              whether the kit connected to the given AP successfully or not.
 *
 *****************************************************************************/
cy_rslt_t app_wl_connect(WhdSTAInterface *wifi, const char *ssid,
                         const char *pwd, nsapi_security_t security)
{
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    SocketAddress sock_addr;

    APP_INFO(("SSID: %s, Security: %d\n", ssid, security));

    if ((NULL == wifi) || (NULL == ssid) || (NULL == pwd))
    {
        ERR_INFO(("Error: %s( %p, %p, %p) bad args\n",
                  __func__, (void*)wifi, (void*)ssid, (void*)pwd));
        return CY_RSLT_TYPE_ERROR;
    }

    /* Connect network */
    APP_INFO(("Connecting to %s...\n", ssid));

    ret = wifi->connect(ssid, pwd, security);

    if (CY_RSLT_SUCCESS == ret)
    {
        APP_INFO(("MAC\t : %s\n", wifi->get_mac_address()));
        wifi->get_netmask(&sock_addr);
        APP_INFO(("Netmask\t : %s\n", sock_addr.get_ip_address()));
        wifi->get_gateway(&sock_addr);
        APP_INFO(("Gateway\t : %s\n", sock_addr.get_ip_address()));
        APP_INFO(("RSSI\t : %d\n\n", wifi->get_rssi()));
        wifi->get_ip_address(&sock_addr);
        APP_INFO(("IP Addr\t : %s\n\n", sock_addr.get_ip_address()));
    }
    else
    {
        APP_INFO(("\nFailed to connect to Wi-Fi AP.\n"));
        ret = CY_RSLT_TYPE_ERROR;
    }

    return ret;
}

/******************************************************************************
 * Function Name: main()
 ******************************************************************************
 * Summary:
 *   Entry function of this application. This initializes WLAN device as
 *   station interface and joins to AP. The Wi-Fi credentials such as SSID,
 *   Password, and Security type need to be mentioned in the mbed_app.json file.
 *   The application takes Low Power Assistant (LPA) configuration from the
 *   device configurator generated sources and applies them while initializing
 *   WLAN station interface. It then runs a forever loop trying to suspend the
 *   network stack.
 *
 *****************************************************************************/
int main(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen */
    APP_INFO(("\x1b[2J\x1b[;H"));
    APP_INFO(("=====================================================\n"));
    APP_INFO(("PSoC 6 MCU: Discard Packet (ICMP) Filter Offload Demo\n"));
    APP_INFO(("=====================================================\n\n"));
    APP_INFO(("ICMP packets trying to reach the host will be discarded by\n"
              "the WLAN. To allow ICMP packets reach the host, remove the\n"
              "ICMP discard filter and save the changes using ModusToolbox\n"
              "device configurator tool. Refer to README.md document for the\n"
              "more detailed steps.\n\n"));

    /* Initializes the LPA offload manager and applies the discard filter
     * configured in the ModusToolbox device configurator tool.
     */
    wifi = new WhdSTAInterface();

    /* Associate to the Wi-Fi AP. */
    result = app_wl_connect(wifi, MBED_CONF_APP_WIFI_SSID,
                              MBED_CONF_APP_WIFI_PASSWORD,
                             MBED_CONF_APP_WIFI_SECURITY);
    PRINT_AND_ASSERT(result, "Failed to connect to AP. "
                     "Check Wi-Fi credentials in mbed_app.json file.\n");

    /* Suspend network stack forever to put the host into deep-sleep state.
     * Any WLAN packets other than ICMP type are allowed to reach the host and
     * wake from deep sleep. The ICMP packets will simply get discarded by the
     * WLAN.
     */
    while (true)
    {
        result = wait_net_suspend(wifi,
                                  osWaitForever,
                                  NETWORK_INACTIVE_INTERVAL_MS,
                                  NETWORK_INACTIVE_WINDOW_MS);
    }

    return result;
}


/* [] END OF FILE */

