EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:wemos_mini
LIBS:Wemos FSBrowser-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L WeMos_mini U1
U 1 1 57A9FDEF
P 4650 2550
F 0 "U1" H 4650 3050 60  0000 C CNN
F 1 "WeMos_mini" H 4650 2050 60  0000 C CNN
F 2 "wemos_d1_mini:D1_mini_board" H 5200 1850 60  0001 C CNN
F 3 "" H 5200 1850 60  0000 C CNN
	1    4650 2550
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL_H SW1
U 1 1 57AA0138
P 7250 2250
F 0 "SW1" H 7330 2360 50  0000 C CNN
F 1 "AP_MODE" H 7250 2200 50  0000 C CNN
F 2 "Buttons_Switches_SMD:SW_SPST_B3S-1000" H 7250 2450 50  0001 C CNN
F 3 "" H 7250 2450 50  0000 C CNN
	1    7250 2250
	-1   0    0    1   
$EndComp
$Comp
L R R1
U 1 1 57AA0183
P 7650 2500
F 0 "R1" V 7730 2500 50  0000 C CNN
F 1 "10k" V 7650 2500 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 7580 2500 50  0001 C CNN
F 3 "" H 7650 2500 50  0000 C CNN
	1    7650 2500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 57AA01CB
P 7650 2650
F 0 "#PWR01" H 7650 2400 50  0001 C CNN
F 1 "GND" H 7650 2500 50  0000 C CNN
F 2 "" H 7650 2650 50  0000 C CNN
F 3 "" H 7650 2650 50  0000 C CNN
	1    7650 2650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 57AA0275
P 3900 2300
F 0 "#PWR02" H 3900 2050 50  0001 C CNN
F 1 "GND" H 3900 2150 50  0000 C CNN
F 2 "" H 3900 2300 50  0000 C CNN
F 3 "" H 3900 2300 50  0000 C CNN
	1    3900 2300
	1    0    0    -1  
$EndComp
Text Label 3850 2200 0    60   ~ 0
+5V
$Comp
L CONN_01X02 P1
U 1 1 57AA02BA
P 3250 3450
F 0 "P1" H 3250 3600 50  0000 C CNN
F 1 "POWER" V 3350 3450 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 3250 3450 50  0001 C CNN
F 3 "" H 3250 3450 50  0000 C CNN
	1    3250 3450
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR03
U 1 1 57AA0383
P 3550 3550
F 0 "#PWR03" H 3550 3300 50  0001 C CNN
F 1 "GND" H 3550 3400 50  0000 C CNN
F 2 "" H 3550 3550 50  0000 C CNN
F 3 "" H 3550 3550 50  0000 C CNN
	1    3550 3550
	1    0    0    -1  
$EndComp
Text Label 3500 3400 0    60   ~ 0
+5V
NoConn ~ 4150 2700
Wire Wire Line
	7650 2350 7650 2250
Connection ~ 7650 2250
Wire Wire Line
	5150 2200 5400 2200
Wire Wire Line
	7100 2250 6900 2250
Wire Wire Line
	4150 2300 3900 2300
Wire Wire Line
	4150 2200 3850 2200
Wire Wire Line
	3450 3500 3550 3500
Wire Wire Line
	3550 3500 3550 3550
Wire Wire Line
	3450 3400 3650 3400
Wire Wire Line
	7400 2250 7950 2250
NoConn ~ 4150 2400
NoConn ~ 4150 2500
NoConn ~ 4150 2800
NoConn ~ 4150 2900
NoConn ~ 5150 2300
NoConn ~ 5150 2400
NoConn ~ 5150 2600
NoConn ~ 5150 2700
NoConn ~ 5150 2800
NoConn ~ 5150 2900
NoConn ~ 5100 3000
Text GLabel 5400 2200 2    60   Input ~ 0
+3V3
Text GLabel 6900 2250 0    60   Input ~ 0
+3V3
$Comp
L CONN_01X03 P2
U 1 1 57AA29E4
P 6550 3600
F 0 "P2" H 6550 3800 50  0000 C CNN
F 1 "TEMP_SENS" V 6650 3600 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 6550 3600 50  0001 C CNN
F 3 "" H 6550 3600 50  0000 C CNN
	1    6550 3600
	1    0    0    -1  
$EndComp
Text GLabel 4150 2600 0    60   Input ~ 0
GPIO4
Text GLabel 7950 2250 2    60   Input ~ 0
GPIO4
Text GLabel 5150 2500 2    60   Input ~ 0
GPIO12
Text GLabel 5650 3600 0    60   Input ~ 0
GPIO12
Text GLabel 6350 3500 0    60   Input ~ 0
+3V3
$Comp
L GND #PWR04
U 1 1 57AA2B2C
P 6300 3750
F 0 "#PWR04" H 6300 3500 50  0001 C CNN
F 1 "GND" H 6300 3600 50  0000 C CNN
F 2 "" H 6300 3750 50  0000 C CNN
F 3 "" H 6300 3750 50  0000 C CNN
	1    6300 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 3750 6300 3700
Wire Wire Line
	6300 3700 6350 3700
$Comp
L R R2
U 1 1 57AA2BE1
P 5800 3400
F 0 "R2" V 5880 3400 50  0000 C CNN
F 1 "4k7" V 5800 3400 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 5730 3400 50  0001 C CNN
F 3 "" H 5800 3400 50  0000 C CNN
	1    5800 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 3600 6350 3600
Wire Wire Line
	5800 3550 5800 3600
Connection ~ 5800 3600
Text GLabel 5800 3250 1    60   Input ~ 0
+3V3
$EndSCHEMATC
