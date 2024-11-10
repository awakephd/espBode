@mainpage espBode v2.0

# espBode v2.0

Connects a Siglent oscilloscope (e.g., SDS804X-HD) to a non-Siglent AWG (e.g., FY6900) to run Bode plots.

## History

The following thread in the EEVBlog Forum describes the development of various solutions for producing Bode plots using Siglent oscilloscopes with non-Siglent AWGs: https://www.eevblog.com/forum/testgear/siglent-sds1104x-e-and-sds1204x-e-bode-plot-with-non-siglent-awg/

### PC and Python: sds1004x_bode

The earliest approach to connect a Siglent oscilloscope with non-Siglent AWGs used a PC connected to the Siglent oscilloscope via LAN and to the non-Siglent AWG via USB. A Python program running on the PC listens for the oscilloscope's SCPI commands, translates them to the non-Siglent AWG's protocol, and sends the translated commands to the AWG via USB. Active development continues for this approach, with the latest effort (as of the date of this writing) found here: https://github.com/hb020/sds1004x_bode/

### ESP-01 and C: espBode

Not long after the initial sds1004x_bode project was published, a new approach began, focused on using an ESP-01 module (ESP8266) to effect the interface and translation. The ESP-01 connects to the Siglent oscilloscope via WiFi and to the non-Siglent AWG via TTL-level serial. The original espBode project was written primarily in C using the Arduino IDE. The C program listens for packets from the Siglent oscilloscope, translates them to the non-Siglent AWG protocol, and sends the translated commands to the AWG via the Serial port. The latest efforts for this approach are found here: https://github.com/hankst69/espBode and here: https://github.com/Switchleg1/espBode

### ESP-01 and C++: espBode v2.0

In terms of inspiration and insight into the communication between a Siglent oscilloscope and a Siglent AWG, espBode v2.0 is very much a descendant of both of the projects above. In terms of code, however, it is an entirely new program, written from the ground up to take full advantage of C++, to maximize the robustness of its network communication, and to extend its usefulness and flexibility.

## A Different Approach

The difference between espBode v2.0 and its predecessors is more than just new code. To be sure, no more than a handful of lines of code were carried over from the earlier projects, and none of those without extensive editing. But the real difference lies in a whole new approach.

The earlier projects generally proceed in a rather linear, "brute force" fashion. The various SCPI commands are identified and handled via a series of conditional statements, and each different model of AWG requires a new set of individual functions to set frequency, amplitude, and so on.

By contrast, espBode2.0 takes advantage of common patterns that can be addressed with tables and inheritance. Thus, for example, most SCPI commands are handled via a table that pairs the command with the associated action, and new AWGs simply need to override two virtual functions (get and set) to send or retrieve all relevant parameters. Even more convenient, for the many variations of the FeelTech AWGs, one need only edit a single table that describes how the values are passed.

An equally significant difference of approach is the use of asynchronous "servers." The inspiration for this approach comes in part from the latest versions of sds1004x_bode which, under the leadership of hb020, have moved to a multi-processing model. For the ESP-01, multi-processing is not a readily available option, so espBode2.0 uses a set of server classes, each of which processes its own responsibilities without blocking the others.

## Supported AWG Models

As of November 2024 the program supports the following models:

* **Feeltech FY####** FeelTech makes a series of AWGs including the FY3200 models, which have only a 2-line monochrome LCD display, and the FY6600, FY6800, and FY6900 models that feature a color graphic display. Each of these can be configured with a variety of maximum output frequencies (20MHz, 40MHz, 60MHz, etc.). They all use a similar command structure, but differ in the details of how parameter values are formatted. The same base code serves all of these models, needing only the proper table to describe the formatting of the values. The initial commit provides a sample for the latest firmware of the FY6900 series.

## Compilation and Installation

To compile and run espBode2.0, you will need the following:

* **Arduino IDE** Go to https://www.arduino.cc/ to download the latest version of the Arduino IDE.

* **ESP-01 Module** The ESP-01 module is readily and inexpensively available from a variety of on-line retailers. espBode2 was developed using an ESP-01S with 1MB of flash. It should run with more flash, but may or may not run with less.

* **ESP-01 Programmer** The ESP-01 programmer is readily and inexpensively available from a variety of on-line retailers. The ESP-01 plugs directly into the programmer, and the programmer plugs into a USB port to communicate with the Arduino IDE.

* **ESP-01 to AWG Interface** The ESP-01 will communicate with the Siglent oscilloscope via WiFi (assuming, of course, that the Siglent oscilloscope has WiFi capability). To communicate with the non-Siglent AWG, it must connect to a TTL-level serial port. Some inexpensive non-Siglent AWGs (such as the FeelTech FY6900) provide an external connector with 5V, RX, TX, and GND. Coming soon are details of a simple interface board which allows the ESP-01 to connect to this port. If no external port is available, one may use a USB-to-serial adapter.

### Dependencies

Several dependencies must be installed in the Arduino IDE.

* **ESP8266 core** To install the ESP8266 core, follow these steps:

	* Go to File/Preferences and enter the following line into the Additional Boards Manager URL list:

		`https://arduino.esp8266.com/stable/package_esp8266com_index.json`

	* Open the Boards Manager (Tools/Board/Boards Manager) and enter `esp8266` into the search box.

	* Click to install the `esp8266 by ESP8266 Community` board.

* **Libraries** Click on Tools/Manage Libraries, then search for and install the following libraries:

	* `ESPTelnet` by Lennart Hennigs

	* `Streaming` by Mikal Hart

### ESP-01 Configuration

* **Board** Click on Tools/Board/esp8266 and select Generic ESP8266 Module.

* **Board Settings** espBode v2.0 was developed using the default settings for the Generic ESP8266 Module. For the board used, the settings were as follows:

	* Upload Speed: 115200
	* Crystal Frequency: 26 MHz
	* Debug port: Disabled
	* Flash Size: 1MB
	* C++ Exceptions: Disabled
	* Flash Frequency: 40MHz
	* Flash Mode: DOUT (compatible)
	* IwIP Variant: v2 Lower Memory
	* Builtin Led: 2
	* Debug Level: None
	* MMU: 32KB cache + 32KB IRAM (balanced)
	* Non-32-Bit-Access: Use pgm_read macros for IRAM/PROGMEM
	* Reset Method: dtr (aka nodemcu)
	* NONOS SDK Version: nonos-sdk-2.2.1+100 (1908703)
	* SSL Support: All SSL ciphers (most compatible)
	* Stack Protection: Disabled
	* VTables: Flash
	* Erase Flash: Only Sketch
	* CPU Frequency: 80MHz

	Note that different variants of the ESP-01 may require slightly different settings.

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

## License

[MIT](https://choosealicense.com/licenses/mit/)

## Authors

* **awakephd** - Primary author of espBode v2.0

* **hb020** - Invaluable consultant especially on network communication, git and github protocol, documentation guidelines, and more.
  
As noted above, even though espBode v2.0 is essentially all-new code, a debt is owed to many others who have contributed to the sds1004x_bode project and to the earlier version of espBode; see the github repositories for details.

