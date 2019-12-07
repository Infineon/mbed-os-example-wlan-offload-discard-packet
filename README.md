# WLAN Discard Packet Filter Offload demonstration in Mbed OS

 This application demonstrates the discard packet filter offload. The application is configured with a discard packet filter for the ICMP packet type using ModusToolbox 2.0 Device Configurator tool so that the WLAN will discard ICMP packets that are trying to reach the host.

## Requirements

- [ModusToolbox™ IDE](https://www.cypress.com/products/modustoolbox-software-environment) v2.0
- [Mbed CLI](https://github.com/ARMmbed/mbed-cli)
- Programming Language: C++
- Associated Parts: All [PSoC® 6 MCU](http://www.cypress.com/PSoC6) parts

## Supported Kits

- CY8CPROTO-062-4343W [PSoC 6 Wi-Fi BT Prototyping Kit](https://www.cypress.com/CY8CPROTO-062-4343W) (CY8CPROTO_062_4343W)
- CY8CKIT-062-WiFi-BT [PSoC 6 WiFi-BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062-WiFi-BT) (CY8CKIT_062_WIFI_BT)
- CY8CKIT-062S2-43012 [PSoC 62S2 Wi-Fi BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062S2-43012) (CY8CKIT_062S2_43012)

## Hardware Setup

This example uses the kit’s default configuration. See the kit guide to ensure that the kit is configured correctly.

**Note**: The PSoC 6 BLE Pioneer kit and the PSoC 6 WiFi-BT Pioneer kit ship with KitProg2 installed. Before using this code example, make sure that the kit is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/cypresssemiconductorco/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like “unable to find CMSIS-DAP device” or “KitProg firmware is out of date”.

## Software Setup

Install a terminal emulator such as [Tera Term](https://ttssh2.osdn.jp/index.html.en) or [PuTTY](https://www.putty.org/) if you don't have any.

This example requires the [Device Configurator Tool](https://www.cypress.com/ModusToolboxDeviceConfig), which is installed as part of [ModusToolbox™ IDE](https://www.cypress.com/products/modustoolbox-software-environment) v2.0. Refer to the [ModusToolbox Installation Guide](http://www.cypress.com/ModusToolboxInstallGuide) for installation instructions.

## Using the Code Example in Mbed CLI Tools

Mbed CLI commands are used to import and compile the code example. See [Working with Mbed CLI web page](https://os.mbed.com/docs/mbed-os/v5.14/tools/working-with-mbed-cli.html).

1. Import the code example into your Mbed OS directory using the following command.
   ```
   mbed import https://github.com/cypresssemiconductorco/mbed-os-example-wlan-offload-discard-packet
   ```

2. Change the working directory to the code example folder.
    ```
    cd mbed-os-example-wlan-offload-discard-packet
    ```
3. Open *mbed_app.json* and edit the Wi-Fi SSID and password to match your AP settings.

4. Connect the board to your PC using the provided USB cable through the USB connector.

5. To allow programming from Mbed CLI, the kit must be in DAPLink mode. See [Firmware-loader](https://github.com/cypresssemiconductorco/Firmware-loader) for more information on how to update the firmware and switch to DAPLink mode.

6. Compile the code and program the target board.
     ```
     mbed compile -m CY8CKIT_062S2_43012 -t GCC_ARM --flash --sterm
     ```

   **Note:** The `--sterm` option opens the serial terminal with 9600-8N1 setting on the command prompt itself after programming completes. Do not use this option if you wish to connect using another serial terminal application.

## Operation
1. After programming, the application starts automatically and the message like the following appears on the serial terminal.

    ```
    ***ICMP discard packet filter offload demo***
    ICMP packets trying to reach the host will be discarded by the WLAN.
    To allow ICMP packets reach the host, remove the ICMP discard filter and save the changes using MTB device-configurator tool.
    Refer to README.md document for more detailed steps.

    SSID: <SSID_NAME>, Security: 4

    Connecting to <SSID_NAME>...
    WLAN MAC Address : <WLAN MAC ADDR>
    WLAN Firmware    : wl0: <WLAN INFO>
    WLAN CLM         : API: <CLM INFO>
    WHD VERSION      : <WHD VERSION INFO>
    MAC     : <WLAN MAC ADDR>
    Netmask : 255.255.255.0
    Gateway : 192.168.43.116
    RSSI    : -50

    IP Addr : 192.168.43.37

    Network Stack Suspended, MCU will enter DeepSleep power mode
    ```

2. The host network stack resumes when it detects any TX/RX activity. When the host resumes, the message such as the following will appear on the serial terminal. This shows the statistics of the SDIO bus transaction.

    ```
    Resuming Network Stack, Network stack was suspended for 25203ms

    =====================================================
    WHD Stats..
    tx_total:42, rx_total:46, tx_no_mem:0, rx_no_mem:0
    tx_fail:0, no_credit:0, flow_control:0
    Bus Stats..
    cmd52:1940, cmd53_read:235, cmd53_write:486
    cmd52_fail:0, cmd53_read_fail:0, cmd53_write_fail:0
    oob_intrs:42, sdio_intrs:85, error_intrs:0, read_aborts:0
    =====================================================
    ```
3. To get started quickly with the demo, jump to the [Verify Discard Packet Filter](#verify-the-discard-packet-filter-offload) section.

## Design and Implementation
The Packet Filter helps to filter out unwanted network packets from reaching the host MCU (PSoC 6 MCU) and waking it. This helps the host to stay in deep sleep for longer periods of time. WLAN network packets can be filtered based on the port (in the application layer in the OSI model), IP address type (transport layer), or Ether Type (network layer).

The Internet Assigned Numbers Authority (IANA) is responsible for maintaining the official assignments of port numbers, protocol numbers, or Ether type for WLAN packets. See the [IANA Protocols Registry](https://www.iana.org/protocols) for information on protocol numbers.

[Figure 1](#figure-1-packet-filters-supported) shows the packet filters Supported, based on a standard IP Stack.

##### Figure 1. Packet Filters Supported
![](./images/pkt_filter_osi_layers.png)

For more details on the implementation, see the [Low Power Assistant Reference Manual](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html#group_lpa_p2_packet_filter).

This application does the following when the kit is powered up:

1. Initializes Wi-Fi as a STA (Station) interface.

2. Initializes the OLM (Offload Manager) with the configuration present in the *GeneratedSources* folder inside *TARGET_<kit>*, where the source code for the feature is generated by the Device Configurator tool in the ModusToolbox IDE. The Offload Manager will be initialized based on the GeneratedSources configuration.

3. Connects to the AP (Access Point) with the Wi-Fi credentials in the *mbed_app.json* file.

### Configure Packet Filters

Use the Cypress Device Configurator tool to configure packet filters and the host MCU wake pin. By default, Mbed OS is shipped with a *design.modus* file that can be used to configure the kit's peripherals from scratch per application requirement. The Device Configurator tool is a part of Cypress' ModusToolBox (MTB) IDE. Install ModusToolBox from the [ModusToolbox web page](https://www.cypress.com/products/modustoolbox-software-environment). The *design.modus* file can only be opened via the Device Configurator tool.

For this discard packet filter demo, a pre-configured *design.modus* file is provided for each target *TARGET_\<kit>*. We recommend that you go through the following steps to understand how a packet filter is configured. It provides useful information on how to access the *design.modus* file and how to configure a discard packet filter using the ModusToolbox Device Configurator tool.

**Note**:
The steps in this section are already handled with this application. They are provided only for informational purposes.

1. Open Device Configurator from the ModusToolbox installation directory: *<mtb_install_dir>\ModusToolbox\tools_2.0\device-configurator*

2. Select **File** > **Open** and then open the *design.modus* file in the example directory for the respective kit, in *<application_folder>/TARGET_\<kit>/CUSTOM_BSP_DESIGN_MODUS/design.modus*.

If the *design.modus* file does not open, and an error message *No device support library path provided* is displayed, point to the *psoc6pdl* folder inside the *mbed-os* directory. The path should look like as follows: *mbed-os-example-wlan-offload-discard-packet\mbed-os\targets\TARGET_Cypress\TARGET_PSOC6\psoc6pdl*.

##### Figure 2. Error Opening design.modus File
![](./images/error_no_device_support.PNG)


##### Figure 3. Set Path to psoc6pdl
![](./images/point_to_devicesupport.PNG)

3. On the **PSoC6 MCU Pins** tab of the Device Configurator tool, do the following:

    - **CY8CKIT_062_WIFI_BT**
        - Enable the Host wake pin *P2[7]* and name it *CYBSP_WIFI_HOST_WAKE*.
        - In the **Parameters** pane, change the following.
            - *Drive Mode*: Analog High-Z. Input buffer off.
            - *Initial Drive State*: High(1).
            - *Interrupt Trigger Type*: Rising Edge.
        ![](./images/configurator_pins_tab_wifi_bt.png)

    - **CY8CKIT_062S2_43012**
        - Enable the Host wake pin *P4[1]* and name it *CYBSP_WIFI_HOST_WAKE*.
        - In the **Parameters** pane, change the following.
            - *Drive Mode*: Analog High-Z. Input buffer off.
            - *Initial Drive State*: High(1).
            - *Interrupt Trigger Type*: Rising Edge.
        ![](./images/configurator_pins_tab_43012.png)

    - **CY8CPROTO_062_4343W**
        - Enable the Host wake pin *P0[4]* and name it *CYBSP_WIFI_HOST_WAKE*.
        - In the **Parameters** pane, change the following.
            - *Drive Mode*: Analog High-Z. Input buffer off.
            - *Initial Drive State*: High(1).
            - *Interrupt Trigger Type*: Rising Edge.
        ![](./images/configurator_pins_tab.png)

4. Go to **WiFi** tab and configure the fields as given below. The below configuration is applicable to all the [supported kits](#supported-kits) in this document. [Figure 4](#figure-4-wifi-configuration) and [Figure 5](#figure-5-discard-filter-configuration) shows reference to Discard packet filter configuration for the CY8CKIT_062S2_43012.
     
     1. Enable Host Wake Configuration and set **Host Device Interrupt Pin** to **CYBSP_WIFI_HOST_WAKE**.

     2. Under **Packet Filters** menu, select only the *Enable Filter Configuration 0* and configure as shown below.

    ##### Figure 4. WiFi Configuration
    ![](./images/add_discard_icmp_filter.PNG)

    ##### Figure 5. Discard Filter Configuration
    ![](./images/discard_filter_configuration.PNG)

This will set a discard packet filter for ICMP packet type which is a *ping* packet.

7. Select **File** > **Save**. The generated source files *cycfg_connectivity_wifi.c* and *cycfg_connectivity_wifi.h* will be available under the *GeneratedSource* folder present in the same location where the *design.modus* file was opened.

## Verify the Discard Packet Filter Offload

Do the following to verify the discard packet filter offload:

1. After programming, the host (PSoC 6 MCU) will enter the deep sleep state. It wakes up for all network packets reaching the host other than the ICMP packet type.

2. Connect a client machine to the same network as the kit connected to.

3. Verify that the host does not wake up when a `ping` command is sent from the client machine, but wakes up for other network packet such as SSH and FTP.

4. Remove the ICMP packet filter using the Device Configurator tool and save the changes to regenerate the sources. Do the following:
    - Open *TARGET_\<kit>/CUSTOM_BSP_DESIGN_MODUS/design.modus* using the  Device Configurator tool.
    - On the **CYW943012WKWBG** > **WiFi** tab (for the CY8CKIT_062S2_43012 board) and under **Packet Filters** section, remove the discard packet filter by unchecking the *Enable Filter Configuration 0* option.
    
    ##### Figure 6. Remove the Discard Filter
    ![](./images/remove_discard_filter.PNG)

    - **File > Save** the changes to regenerate the sources.

5. Compile and program the application.

6. Verify that the host wakes up for all the packets reaching the host including ICMP packets from the peer device.

**Note**:
- The code example disables the default device configuration provided in `mbed-os\targets\TARGET_Cypress\TARGET_PSOC6\TARGET\COMPONENT_BSP_DESIGN_MODUS` with the one provided in `TARGET/CUSTOM_BSP_DESIGN_MODUS`. The custom configuration disables the Phase-locked Loop (PLL), disables HF clock to unused peripherals like audio/USB, and configures the Buck instead of Low Dropout (LDO) regulator to power the PSoC 6 device. This configuration reduces the current consumed by the PSoC 6 in Active state with a small increase in Deep Sleep current. Please enable the peripherals using `device-configurator` if you are using them. The`device-configurator` is bundled with the ModusToobox 2.0 installation and can be found at *\<ModusToolbox_installed_location>/tools_\*.\*/device-configurator*.

- This application does not try to associate to Access Point if disconnected from the network. In that case, reset the kit to connect to AP.

## Reference Documents

|                  Application Notes                           |                        Description                           |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
| [AN227640](https://cypresssemiconductorco.github.io/cypress-mbed-os-docs/) – Getting Started with PSoC 6 and CYW43xxx in Mbed | Describes PSoC 6 MCU devices and how to build your first application with Mbed.|
| [AN227910](http://www.cypress.com/AN227910) – Low-Power System Design with PSoC 6 and 43012 | Describes how to implement a low-power system design.        |
| **Code Examples**                                            |                                                              |
| [Using Mbed OS](https://github.com/cypresssemiconductorco?q=mbed-os-example%20NOT%20Deprecated) |
| [Using ModusToolbox IDE](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software) | [Using PSoC Creator](https://www.cypress.com/documentation/code-examples/psoc-6-mcu-code-examples) |
| **Device Documentation**                                     |                                                              |
| [PSoC 6 MCU Datasheets](http://www.cypress.com/search/all?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A575&f[2]=field_related_products%3A114026) | [PSoC 6 Technical Reference Manuals](https://www.cypress.com/search/all/PSoC%206%20Technical%20Reference%20Manual?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A583) |
| **Development Kits**                                         | Buy at Cypress.com                                     |
| [CY8CKIT-062S2-43012 PSoC 62S2 WiFi-BT Pioneer Kit](http://www.cypress.com/CY8CKIT-062-BLE) | [CY8CKIT-062-WiFi-BT PSoC 6   WiFi-BT Pioneer Kit](http://www.cypress.com/CY8CKIT-062-WiFi-BT) |
| [CY8CPROTO-062-4343W   PSoC 6 Wi-Fi BT Prototyping Kit](http://www.cypress.com/cy8cproto-062-4343w) |                                                              |
| **Libraries**                                                |                                                              |
| PSoC 6 Peripheral Driver Library and docs                    | [psoc6pdl](https://github.com/cypresssemiconductorco/psoc6pdl) on GitHub |
| Cypress Hardware Abstraction Layer Library and docs          | [psoc6hal](https://github.com/cypresssemiconductorco/psoc6hal) on GitHub |
| **Middleware**                                               | Middleware libraries are distributed on GitHub               |
| Low Power Assistant (LPA)                    | [lpa](https://github.com/cypresssemiconductorco/lpa) on GitHub |
| HTTP Server                                    | [http-server](https://github.com/cypresssemiconductorco/http-server) on GitHub |
| Connectivity-Utilities                                    | [connectivity-utilities](https://github.com/cypresssemiconductorco/connectivity-utilities) on GitHub |
| Links to all PSoC 6 MCU Middleware                               | [psoc6-middleware](https://github.com/cypresssemiconductorco/psoc6-middleware) on GitHub |
| **Tools**                                                    |                                                              |
| [ModusToolbox   IDE](http://www.cypress.com/modustoolbox)    | The Cypress IDE for PSoC 6 and IoT designers                 |
| [PSoC Creator](https://www.cypress.com/products/psoc-creator-integrated-design-environment-ide) | The Cypress IDE for PSoC and FM0+ development                |

Cypress provides a wealth of data at www.cypress.com to help you to select the right device, and quickly and effectively integrate the device into your design.

For PSoC 6 MCU devices, see [How to Design with PSoC 6 MCU - KBA223067](https://community.cypress.com/docs/DOC-14644) in the Cypress community.

## Low Power Assistant: Related Documents
 - [LPA Middleware RELEASE.md](https://github.com/cypresssemiconductorco/lpa/blob/master/README.md)
 - [LPA Middleware API Reference Guide](https://cypresssemiconductorco.github.io/lpa/lpa_api_reference_manual/html/index.html)
 - [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)
 - [LPA Middleware Code Example for MBED OS](https://github.com/cypresssemiconductorco)
 - [ModusToolbox Device Configurator Tool Guide](https://www.cypress.com/file/455241/download)

## Document History

Document Title: CE229167 - Mbed OS and PSoC 6 MCU: WLAN Discard Packet Filter Offload

| Revision |                 Description of Change              |
| -------- | -------------------------------------------------- |
|    1.0.0   |   Initial release.<br/>Tested with Mbed OS v5.14.2 and LPA middleware v1.0.0       |

------

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

![Banner](images/Banner.png)

-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2019. This document is the property of Cypress Semiconductor Corporation and its subsidiaries ("Cypress"). This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide. Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights. If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress's patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products. Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.  
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. No computing device can be absolutely secure. Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach"). Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach. In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes. It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product. "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage. Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices. "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness. Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, its directors, officers, employees, agents, affiliates, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress's published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.  
Cypress, the Cypress logo, Spansion, the Spansion logo, and combinations thereof, WICED, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress in the United States and other countries. For a more complete list of Cypress trademarks, visit cypress.com. Other names and brands may be claimed as property of their respective owners.



