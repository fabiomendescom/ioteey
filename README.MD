# Ioteey

I wanted to build a set of libraries that would encapsulate the entire work of configuring, joining an IOT device to a network and executing using
environments such as Samsung Smartthings. Ioteey was born. This shows how an ESP8266 device can have full capability of configuring itself.
If it has not joined a network, it will configure itself as an Access Point allowing you to join it to a network and configure it without the 
need to compile any code. Once you define a device type, it will know how:
- To configure itself to join a WIFI network
- Broadcast itself using mDNS so you can know its IP and port
- Broadcast itself using SSDP and UPnP
- To serve webpages. ("/" for main page, "/settings" for settings)

This makes Ioteey a very configurable option on top of the well established ESP8266 platform.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

- ESP8266 setup in Arduino IDE to get all ESP8266 libraries

```
Give examples
```

### Installing

FOR ESP8266
Install these files and change the File->Preferences->Sketchbook Location to point to the ESP8266 directory 

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone who's code was used
* Inspiration
* etc
