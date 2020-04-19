# Generalized Automatic Anomaly Detection System

Enclosed with you can find Granny Smith Apple's 2019-2020 senior capstone project: a library for detecting anomalies within any system even without knowing finer details regarding that system's structure.

## Getting Started
This project is designed to work as a library where the SysAdmin's code interfaces with the code developed by the team. To understand this ecosystem, one should view the Data Flow and Feedback Flow pdf files on the root. Then, within the src folder one can find the main interface in AnomalyDetectionSystem.h. This file is complete with Doxygen commentary to make the setup as easy as possible. An example for demonstration purposes can be found within testADS.cpp. To start your own project, first run ADSSettingHelper which is located on the root. This will prompt you through the creation of a settings file that you can use to construct an AnomalyDetectionSystem object.

## Further information
More in-depth discussion on the different options can be found at Options Guide.pdf on the root. Benchmarks are located within Benchmarking.pdf.

## Future Progress
Unfortunately, due to Coronavirus certain aspects of the project needed to be rescoped. Future development could integrate more unsupervised models (ARIMA, SVM, HMM, etc), clean up the installation process, optimize some shared calculations, improve some approximated values, or document the non-UI files.
