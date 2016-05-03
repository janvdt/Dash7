APP_BUILD(NAME ${APP_NAME} SOURCES project.c uart/uart.c acc/acc.c sensor/extsensor.c lora/lora.c gps/gps.c lora/ComSlip.c lora/CRC16.c lora/IM880A_RadioInterface.c gps/minmea.c LIBS framework)
