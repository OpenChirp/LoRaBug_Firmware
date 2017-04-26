/*
 * loraChannels.h
 *
 *  Created on: Jan 23, 2017
 *      Author: Artur Balanuta
 */

#ifndef SERVICES_LORACHANNELS_H_
#define SERVICES_LORACHANNELS_H_

// US Spectrum
// US  902 to 928 MHz  902.3 MHz to 914.9 MHz spaced at 200KHz (Upstream-64 channels)
// 903 MHz to 914.2 MHz spaced at 1.6 MHz apart (Upstream- 8 channels) 500k
// 923.3 MHz to 927.5 MHz spaced at 600KHz apart (Downstream- 8 channels) 500k


#define CH_900_00 902300000   //hz  min_dr  0   max_dr  3
#define CH_900_01 902500000   //hz  min_dr  0   max_dr  3
#define CH_900_02 902700000   //hz  min_dr  0   max_dr  3
#define CH_900_03 902900000   //hz  min_dr  0   max_dr  3
#define CH_900_04 903100000   //hz  min_dr  0   max_dr  3
#define CH_900_05 903300000   //hz  min_dr  0   max_dr  3
#define CH_900_06 903500000   //hz  min_dr  0   max_dr  3
#define CH_900_07 903700000   //hz  min_dr  0   max_dr  3
#define CH_900_08 903900000   //hz  min_dr  0   max_dr  3
#define CH_900_09 904100000   //hz  min_dr  0   max_dr  3
#define CH_900_10 904300000   //hz  min_dr  0   max_dr  3
#define CH_900_11 904500000   //hz  min_dr  0   max_dr  3
#define CH_900_12 904700000   //hz  min_dr  0   max_dr  3
#define CH_900_13 904900000   //hz  min_dr  0   max_dr  3
#define CH_900_14 905100000   //hz  min_dr  0   max_dr  3
#define CH_900_15 905300000   //hz  min_dr  0   max_dr  3
#define CH_900_16 905500000   //hz  min_dr  0   max_dr  3
#define CH_900_17 905700000   //hz  min_dr  0   max_dr  3
#define CH_900_18 905900000   //hz  min_dr  0   max_dr  3
#define CH_900_19 906100000   //hz  min_dr  0   max_dr  3
#define CH_900_20 906300000   //hz  min_dr  0   max_dr  3
#define CH_900_21 906500000   //hz  min_dr  0   max_dr  3
#define CH_900_22 906700000   //hz  min_dr  0   max_dr  3
#define CH_900_23 906900000   //hz  min_dr  0   max_dr  3
#define CH_900_24 907100000   //hz  min_dr  0   max_dr  3
#define CH_900_25 907300000   //hz  min_dr  0   max_dr  3
#define CH_900_26 907500000   //hz  min_dr  0   max_dr  3
#define CH_900_27 907700000   //hz  min_dr  0   max_dr  3
#define CH_900_28 907900000   //hz  min_dr  0   max_dr  3
#define CH_900_29 908100000   //hz  min_dr  0   max_dr  3
#define CH_900_30 908300000   //hz  min_dr  0   max_dr  3
#define CH_900_31 908500000   //hz  min_dr  0   max_dr  3
#define CH_900_32 908700000   //hz  min_dr  0   max_dr  3
#define CH_900_33 908900000   //hz  min_dr  0   max_dr  3
#define CH_900_34 909100000   //hz  min_dr  0   max_dr  3
#define CH_900_35 909300000   //hz  min_dr  0   max_dr  3
#define CH_900_36 909500000   //hz  min_dr  0   max_dr  3
#define CH_900_37 909700000   //hz  min_dr  0   max_dr  3
#define CH_900_38 909900000   //hz  min_dr  0   max_dr  3
#define CH_900_39 910100000   //hz  min_dr  0   max_dr  3
#define CH_900_40 910300000   //hz  min_dr  0   max_dr  3
#define CH_900_41 910500000   //hz  min_dr  0   max_dr  3
#define CH_900_42 910700000   //hz  min_dr  0   max_dr  3
#define CH_900_43 910900000   //hz  min_dr  0   max_dr  3
#define CH_900_44 911100000   //hz  min_dr  0   max_dr  3
#define CH_900_45 911300000   //hz  min_dr  0   max_dr  3
#define CH_900_46 911500000   //hz  min_dr  0   max_dr  3
#define CH_900_47 911700000   //hz  min_dr  0   max_dr  3
#define CH_900_48 911900000   //hz  min_dr  0   max_dr  3
#define CH_900_49 912100000   //hz  min_dr  0   max_dr  3
#define CH_900_50 912300000   //hz  min_dr  0   max_dr  3
#define CH_900_51 912500000   //hz  min_dr  0   max_dr  3
#define CH_900_52 912700000   //hz  min_dr  0   max_dr  3
#define CH_900_53 912900000   //hz  min_dr  0   max_dr  3
#define CH_900_54 913100000   //hz  min_dr  0   max_dr  3
#define CH_900_55 913300000   //hz  min_dr  0   max_dr  3
#define CH_900_56 913500000   //hz  min_dr  0   max_dr  3
#define CH_900_57 913700000   //hz  min_dr  0   max_dr  3
#define CH_900_58 913900000   //hz  min_dr  0   max_dr  3
#define CH_900_59 914100000   //hz  min_dr  0   max_dr  3
#define CH_900_60 914300000   //hz  min_dr  0   max_dr  3
#define CH_900_61 914500000   //hz  min_dr  0   max_dr  3
#define CH_900_62 914700000   //hz  min_dr  0   max_dr  3
#define CH_900_63 914900000   //hz  min_dr  0   max_dr  3

#define CH_900_64 903000000   //hz  min_dr  4   max_dr  4
#define CH_900_65 904600000   //hz  min_dr  4   max_dr  4
#define CH_900_66 906200000   //hz  min_dr  4   max_dr  4
#define CH_900_67 907800000   //hz  min_dr  4   max_dr  4
#define CH_900_68 909400000   //hz  min_dr  4   max_dr  4
#define CH_900_69 911000000   //hz  min_dr  4   max_dr  4
#define CH_900_70 912600000   //hz  min_dr  4   max_dr  4
#define CH_900_71 914200000   //hz  min_dr  4   max_dr  4



// EUROPE Spectrum
// 863 to 870 MHz
#define CH_04_868 863200000 // 863.20MHz
#define CH_05_868 863500000 // 863.50MHz
#define CH_06_868 863800000 // 863.80MHz
#define CH_07_868 864100000 // 864.10MHz (used by End device to transmit Join Request)
#define CH_08_868 864300000 // 864.30MHz (used by End device to transmit Join Request)
#define CH_09_868 864500000 // 864.50MHz (used by End device to transmit Join Request)
#define CH_10_868 865200000 // 865.20MHz
#define CH_11_868 865500000 // 865.50MHz
#define CH_12_868 865800000 // 865.80MHz
#define CH_13_868 866100000 // 866.10MHz
#define CH_14_868 866700000 // 866.40MHz
#define CH_15_868 866700000 // 866.70MHz
#define CH_16_868 867000000 // 867.00MHz
#define CH_17_868 868000000 // 868.00MHz
#define CH_18_868 868100000 // 868.10 Mhz (used by Gateway to listen Join Request)
#define CH_19_868 868300000 // 868.30 Mhz (used by Gateway to listen Join Request)
#define CH_20_868 868500000 // 868.50 Mhz (used by Gateway to listen Join Request)


//433 MHz Band
#define CH_00_433 433300000 // 433.3MHz
#define CH_01_433 433600000 // 433.6MHz
#define CH_02_433 433900000 // 433.9MHz
#define CH_03_433 434300000 // 434.3MHz

#endif /* SERVICES_LORACHANNELS_H_ */
