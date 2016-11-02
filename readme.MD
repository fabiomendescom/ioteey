# Ioteey

I wanted to create a framework that would allow an ESP8266 to have the ability to configure itself on a network, provide web pages for settings and general information.
IOTEEY was born.

## Features
* If not joined to a network, Ioteey devices will function as Access Points. You can connect to it, and configure network settings
* When joined to a network, Ioteeys present themselves with an informational web page with the ability to go to a settings page
* Ioteey devices broadcast their IPs and ports through mDNS
* Ioteey devices broadcast using SSDP for UPnP discovery. (NOTE: The services discovery is not implemented yet)
* Ioteey devices can be integrated with Samsung Smartthings. They can be added and controlled just like any other Smartthings device.

## Getting Started

* Install ESP8266 libraries for Arduino and make sure it is working properly
* Copy code to a directory and make sure the Arduino IDE File->Preferences->Settings->Sketchbook location points to the root of the folder.
* Connect your ESP8266 to the USB
* Make sure to choose Tools->Board Node MCU 1.0 (ESP-12E Module) and Programmer: Arduino ISP

* Open the examples and check the code. Start with the "BasicExample.ino" for a basic idea of how the Ioteey interacts

### Samsung Smartthings integration

* Copy the groovy files inside "Smartthings" directory into your Smartthings portal. 


### Prerequisites

* ESP8266 libraries installed on Arduino IDE

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning


## Authors

* **Fabio Mendes** - *Initial work* - [PurpleBooth](https://github.com/fabiomendescom)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License

## Acknowledgments

* Add people who provided libraries
* 
