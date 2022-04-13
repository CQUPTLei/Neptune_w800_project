# Wi-Fi Aware<a name="EN-US_TOPIC_0000001124444687"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section1464106163817)
-   [Constraints](#section13591154411)

## Introduction<a name="section11660541593"></a>

The Wi-Fi Aware module provides near-field communication capabilities for applications.

## Directory Structure<a name="section1464106163817"></a>

```
foundation/communication/wifi_aware/         # Wi-Fi Aware directory
├── frameworks
│  └── source                            # Wi-Fi Aware source file
├── hals                                  # Hardware abstraction layer (HAL) header files
└── interfaces
    └── kits                              # External APIs
```

## Constraints<a name="section13591154411"></a>

The Wi-Fi Aware module is compiled using the C language. Currently, only the Hi3861 development board is supported. If you want to use other chips, you need to implement OpenHarmony integration APIs and place your implementation code in the  **vendor**  directory.

