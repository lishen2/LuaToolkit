//  PCAN-UDS.h
//
//  ~~~~~~~~~~~~
//
//  PCAN-UDS API
//
//  ~~~~~~~~~~~~
//
//  ------------------------------------------------------------------
//  Author : Thomas Haber
//	Last changed by:	$Author: Fabrice $
//  Last changed date:	$Date: 2015-11-13 14:41:22 +0100 (ven., 13 nov. 2015) $
//
//  Language: ANSI-C
//  ------------------------------------------------------------------
//
//  Copyright (C) 2015  PEAK-System Technik GmbH, Darmstadt
//  more Info at http://www.peak-system.com 
//
#ifndef __PCANUDS_H__
#define __PCANUDS_H__


////////////////////////////////////////////////////////////
// Value definitions
////////////////////////////////////////////////////////////

// Currently defined and supported PCAN-UDS channels
//
#define PUDS_NONEBUS			0x00	// Undefined/default value for a PCAN bus

#define PUDS_ISABUS1			0x21	// PCAN-ISA interface, channel 1
#define PUDS_ISABUS2			0x22	// PCAN-ISA interface, channel 2
#define PUDS_ISABUS3			0x23	// PCAN-ISA interface, channel 3
#define PUDS_ISABUS4			0x24	// PCAN-ISA interface, channel 4
#define PUDS_ISABUS5			0x25	// PCAN-ISA interface, channel 5
#define PUDS_ISABUS6			0x26	// PCAN-ISA interface, channel 6
#define PUDS_ISABUS7			0x27	// PCAN-ISA interface, channel 7
#define PUDS_ISABUS8			0x28	// PCAN-ISA interface, channel 8

#define PUDS_DNGBUS1			0x31	// PCAN-Dongle/LPT interface, channel 1

#define PUDS_PCIBUS1			0x41	// PCAN-PCI interface, channel 1
#define PUDS_PCIBUS2			0x42	// PCAN-PCI interface, channel 2
#define PUDS_PCIBUS3			0x43	// PCAN-PCI interface, channel 3
#define PUDS_PCIBUS4			0x44	// PCAN-PCI interface, channel 4
#define PUDS_PCIBUS5			0x45	// PCAN-PCI interface, channel 5
#define PUDS_PCIBUS6			0x46	// PCAN-PCI interface, channel 6
#define PUDS_PCIBUS7	        0x47	// PCAN-PCI interface, channel 7
#define PUDS_PCIBUS8	        0x48	// PCAN-PCI interface, channel 8

#define PUDS_USBBUS1			0x51	// PCAN-USB interface, channel 1
#define PUDS_USBBUS2			0x52	// PCAN-USB interface, channel 2
#define PUDS_USBBUS3			0x53	// PCAN-USB interface, channel 3
#define PUDS_USBBUS4			0x54	// PCAN-USB interface, channel 4
#define PUDS_USBBUS5			0x55	// PCAN-USB interface, channel 5
#define PUDS_USBBUS6			0x56	// PCAN-USB interface, channel 6
#define PUDS_USBBUS7			0x57	// PCAN-USB interface, channel 7
#define PUDS_USBBUS8			0x58	// PCAN-USB interface, channel 8

#define PUDS_PCCBUS1			0x61	// PCAN-PC Card interface, channel 1
#define PUDS_PCCBUS2			0x62	// PCAN-PC Card interface, channel 2

// PCAN-UDS Baudrates for the CAN Hardware 
//
#define PUDS_BAUD_1M                               0x0014  // Channel Baudrate 1 MBit/s
#define PUDS_BAUD_800K                             0x0016  // Channel Baudrate 800 kBit/s
#define PUDS_BAUD_500K                             0x001C  // Channel Baudrate 500 kBit/s
#define PUDS_BAUD_250K                             0x011C  // Channel Baudrate 250 kBit/s
#define PUDS_BAUD_125K                             0x031C  // Channel Baudrate 125 kBit/s
#define PUDS_BAUD_100K                             0x432F  // Channel Baudrate 100 kBit/s
#define PUDS_BAUD_95K                              0xC34E  // Channel Baudrate 95,238 kBit/s
#define PUDS_BAUD_83K                              0x852B  // Channel Baudrate 83,333 kBit/s
#define PUDS_BAUD_50K                              0x472F  // Channel Baudrate 50 kBit/s
#define PUDS_BAUD_47K                              0x1414  // Channel Baudrate 47,619 kBit/s
#define PUDS_BAUD_33K                              0x8B2F  // Channel Baudrate 33,333 kBit/s
#define PUDS_BAUD_20K                              0x532F  // Channel Baudrate 20 kBit/s
#define PUDS_BAUD_10K                              0x672F  // Channel Baudrate 10 kBit/s
#define PUDS_BAUD_5K                               0x7F7F  // Channel Baudrate 5 kBit/s

// Not Plug-And-Play PCAN-UDS Hardware types 
//
#define PUDS_TYPE_ISA            0x01  // PCAN-ISA 82C200
#define PUDS_TYPE_ISA_SJA        0x09  // PCAN-ISA SJA1000
#define PUDS_TYPE_ISA_PHYTEC     0x04  // PHYTEC ISA 
#define PUDS_TYPE_DNG            0x02  // PCAN-Dongle 82C200
#define PUDS_TYPE_DNG_EPP        0x03  // PCAN-Dongle EPP 82C200
#define PUDS_TYPE_DNG_SJA        0x05  // PCAN-Dongle SJA1000
#define PUDS_TYPE_DNG_SJA_EPP    0x06  // PCAN-Dongle EPP SJA1000

// Represent the PCAN-UDS error and status codes 
//
#define PUDS_ERROR_OK						0x00000	// No error 
#define PUDS_ERROR_NOT_INITIALIZED			0x00001 // Not Initialized
#define PUDS_ERROR_ALREADY_INITIALIZED		0x00002 // Already Initialized
#define PUDS_ERROR_NO_MEMORY				0x00003 // Could not obtain memory
#define PUDS_ERROR_OVERFLOW					0x00004 // Input buffer overflow
#define PUDS_ERROR_TIMEOUT					0x00006 // Timeout while accessing the PCANTP mutex
#define PUDS_ERROR_NO_MESSAGE				0x00007 // No Message available
#define PUDS_ERROR_WRONG_PARAM				0x00008 // Wrong message parameters
#define PUDS_ERROR_BUSLIGHT					0x00009 // PCANTP Channel is in BUS-LIGHT error state
#define PUDS_ERROR_BUSHEAVY					0x0000A // PCANTP Channel is in BUS-HEAVY error state
#define PUDS_ERROR_BUSOFF					0x0000B // PCANTP Channel is in BUS-OFF error state
#define PUDS_ERROR_CAN_ERROR				0x80000000	// Global CAN error, use following macros to merge/fetch PCAN error
// Macros to merge/retrieve PCAN error in PCAN-UDS error
//
#define PUDS_CAN_ERROR_SET(pcan_error)		(PUDS_ERROR_CAN_ERROR | pcan_error)
#define PUDS_CAN_ERROR_GET(puds_status)		(puds_status - PUDS_ERROR_CAN_ERROR)

// Represents message request confirmation values defined in ISO-15765-2 (see page 10-11)
//
#define PUDS_RESULT_N_OK                                  0x00	// No network error
#define PUDS_RESULT_N_TIMEOUT_A                           0x01	// timeout occured between 2 frames transmission (sender and receiver side) 
#define PUDS_RESULT_N_TIMEOUT_Bs                          0x02	// sender side timeout while waiting for flow control frame 
#define PUDS_RESULT_N_TIMEOUT_Cr                          0x03	// receiver side timeout while waiting for consecutive frame 
#define PUDS_RESULT_N_WRONG_SN                            0x04	// unexpected sequence number 
#define PUDS_RESULT_N_INVALID_FS                          0x05	// invalid or unknown FlowStatus 
#define PUDS_RESULT_N_UNEXP_PDU                           0x06	// unexpected protocol data unit 
#define PUDS_RESULT_N_WFT_OVRN                            0x07	// reception of flow control WAIT frame that exceeds the maximum counter defined by PUDS_PARAM_WFT_MAX 
#define PUDS_RESULT_N_BUFFER_OVFLW                        0x08	// buffer on the receiver side cannot store the data length (server side only) 
#define PUDS_RESULT_N_ERROR                               0x09	// buffer on the receiver side cannot store the data length (server side only) 

// PUDS parameters
//
#define PUDS_PARAM_SERVER_ADDRESS					0xC1	// 2 BYTE data describing the physical address of the equipment
#define PUDS_PARAM_SERVER_FILTER					0xC2	// 2 BYTE data (2 BYTE functional address and MSB for status)
															//	 describing a functional address to ignore or listen to
#define PUDS_PARAM_TIMEOUT_REQUEST					0xC3	// 4 BYTE data describing the maximum time allowed by the client to transmit a request 
															//	 See ISO-15765-3 ?.3.2 : /\P2Can_Req
#define PUDS_PARAM_TIMEOUT_RESPONSE					0xC4	// 4 BYTE data describing the maximum time allowed by the client to receive a response
															//	 See ISO-15765-3 ?.3.2 : /\P2Can_Rsp
#define PUDS_PARAM_SESSION_INFO						0xC5	// Require a pointer to a TPUDSSessionInfo structure
#define PUDS_PARAM_API_VERSION						0xC6	// API version parameter
#define PUDS_PARAM_RECEIVE_EVENT					0xC7	// Define UDS receive-event handler, require a pointer to an event HANDLE. 

#define PUDS_PARAM_BLOCK_SIZE	                    0xE1	// 1 BYTE data describing the block size parameter (BS)
#define PUDS_PARAM_SEPERATION_TIME					0xE2	// 1 BYTE data describing the seperation time parameter (STmin)
#define PUDS_PARAM_DEBUG       					    0xE3	// 1 BYTE data describing the debug mode 
#define PUDS_PARAM_CHANNEL_CONDITION				0xE4	// 1 BYTE data describing the condition of a channel. 
#define PUDS_PARAM_WFT_MAX							0xE5	// Integer data describing the Wait Frame Transmissions parameter. 
#define PUDS_PARAM_CAN_DATA_PADDING					0xE8    // 1 BYTE data stating if CAN frame DLC uses padding or not
#define PUDS_PARAM_PADDING_VALUE					0xED    // 1 BYTE data stating the value used for CAN data padding

// PUDS parameter values
//
#define PUDS_DEBUG_NONE       						0x00    // No debug messages
#define PUDS_DEBUG_CAN       						0x01    // Puts CAN debug messages to stdout
#define PUDS_CHANNEL_UNAVAILABLE					0x00    // The Channel is illegal or not available
#define PUDS_CHANNEL_AVAILABLE						0x01    // The Channel is available
#define PUDS_CHANNEL_OCCUPIED						0x02    // The Channel is valid, and is being used
#define PUDS_SERVER_ADDR_TEST_EQUIPMENT				0xF1    // Physical address for external test equipment
#define PUDS_SERVER_ADDR_REQUEST_OBD_SYSTEM			0x33	// Functional request address for Legislated OBD system
#define PUDS_SERVER_ADDR_FLAG_ENHANCED_ISO_15765_3	0x1000	// Flag stating that the address is defined as a ISO-15765-3 address
#define PUDS_SERVER_ADDR_MASK_ENHANCED_ISO_15765_3	0x07FF	// Mask used for the ISO-15765-3 enhanced addresses
#define PUDS_SERVER_FILTER_IGNORE					0x0000	// Filter status : ignore (used to remove previously set filter)
#define PUDS_SERVER_FILTER_LISTEN					0x8000	// Filter status : listen to (must be OR'ed with the 2 BYTE functional address)
#define PUDS_TIMEOUT_REQUEST						10000	// Default maximum timeout for UDS transmit confirmation
#define PUDS_TIMEOUT_RESPONSE						10000	// Default maximum timeout for UDS response reception
#define PUDS_P2CAN_DEFAULT_SERVER_MAX				50		// Default server performance requirement (See ISO-15765-3 ?.3.2)
#define PUDS_P2CAN_ENHANCED_SERVER_MAX				5000	// Enhanced server performance requirement (See ISO-15765-3 ?.3.2)
#define PUDS_CAN_DATA_PADDING_NONE   		        0x00    // uses CAN frame data optimization
#define PUDS_CAN_DATA_PADDING_ON					0x01    // uses CAN frame data padding (default, i.e. CAN DLC = 8)
#define PUDS_CAN_DATA_PADDING_VALUE					0x55	// default value used if CAN data padding is enabled


// PUDS Service ids defined in ISO 14229-1
//
#define PUDS_SI_DiagnosticSessionControl           0x10     // see ISO 14229-1
#define PUDS_SI_ECUReset                           0x11     // see ISO 14229-1           
#define PUDS_SI_SecurityAccess                     0x27     // see ISO 14229-1 
#define PUDS_SI_CommunicationControl               0x28     // see ISO 14229-1 
#define PUDS_SI_TesterPresent                      0x3E     // see ISO 14229-1
#define PUDS_SI_AccessTimingParameter              0x83     // see ISO 14229-1
#define PUDS_SI_SecuredDataTransmission            0x84     // see ISO 14229-1
#define PUDS_SI_ControlDTCSetting                  0x85     // see ISO 14229-1
#define PUDS_SI_ResponseOnEvent                    0x86     // see ISO 14229-1
#define PUDS_SI_LinkControl                        0x87     // see ISO 14229-1
#define PUDS_SI_ReadDataByIdentifier               0x22     // see ISO 14229-1
#define PUDS_SI_ReadMemoryByAddress                0x23     // see ISO 14229-1
#define PUDS_SI_ReadScalingDataByIdentifier        0x24     // see ISO 14229-1
#define PUDS_SI_ReadDataByPeriodicIdentifier       0x2A     // see ISO 14229-1
#define PUDS_SI_DynamicallyDefineDataIdentifier    0x2C     // see ISO 14229-1
#define PUDS_SI_WriteDataByIdentifier              0x2E     // see ISO 14229-1
#define PUDS_SI_WriteMemoryByAddress               0x3D     // see ISO 14229-1
#define PUDS_SI_ClearDiagnosticInformation         0x14     // see ISO 14229-1
#define PUDS_SI_ReadDTCInformation                 0x19     // see ISO 14229-1
#define PUDS_SI_InputOutputControlByIdentifier     0x2F     // see ISO 14229-1
#define PUDS_SI_RoutineControl                     0x31     // see ISO 14229-1
#define PUDS_SI_RequestDownload                    0x34     // see ISO 14229-1
#define PUDS_SI_RequestUpload                      0x35     // see ISO 14229-1
#define PUDS_SI_TransferData                       0x36     // see ISO 14229-1
#define PUDS_SI_RequestTransferExit                0x37     // see ISO 14229-1
#define PUDS_NR_SI                                 0x7F     // negative response code
#define PUDS_NRC_EXTENDED_TIMING                   0x78     // server wants more time
#define PUDS_SI_POSITIVE_RESPONSE				   0x40		// positive response offset


// PUDS ISO_15765_4 11 bit CAN ID definitions
//
#define PUDS_ISO_15765_4_CAN_ID_FUNCTIONAL_REQUEST		0x7DF        // CAN ID for functionally addressed request messages sent by external test equipment
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_REQUEST_1		0x7E0        // physical request CAN ID from external test equipment to ECU #1
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_RESPONSE_1		0x7E8        // physical response CAN ID from ECU #1 to external test equipment
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_REQUEST_2		0x7E1        // physical request CAN ID from external test equipment to ECU #2
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_RESPONSE_2		0x7E9        // physical response CAN ID from ECU #2 to external test equipment
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_REQUEST_3		0x7E2        // physical request CAN ID from external test equipment to ECU #3
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_RESPONSE_3		0x7EA        // physical response CAN ID from ECU #3 to external test equipment
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_REQUEST_4		0x7E3        // physical request CAN ID from external test equipment to ECU #4
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_RESPONSE_4		0x7EB        // physical response CAN ID from ECU #4 to external test equipment
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_REQUEST_5		0x7E4        // physical request CAN ID from external test equipment to ECU #5
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_RESPONSE_5		0x7EC        // physical response CAN ID from ECU #5 to external test equipment
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_REQUEST_6		0x7E5        // physical request CAN ID from external test equipment to ECU #6
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_RESPONSE_6		0x7ED        // physical response CAN ID from ECU #6 to external test equipment
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_REQUEST_7		0x7E6        // physical request CAN ID from external test equipment to ECU #7
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_RESPONSE_7		0x7EE        // physical response CAN ID from ECU #7 to external test equipment
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_REQUEST_8		0x7E7        // physical request CAN ID from external test equipment to ECU #8
#define PUDS_ISO_15765_4_CAN_ID_PHYSICAL_RESPONSE_8		0x7EF        // physical response CAN ID from ECU #8 to external test equipment

// PUDS ISO_15765_4 address definitions
//
#define PUDS_ISO_15765_4_ADDR_TEST_EQUIPMENT            0xF1     // external test equipment
#define PUDS_ISO_15765_4_ADDR_OBD_FUNCTIONAL            0x33     // OBD funtional system
#define PUDS_ISO_15765_4_ADDR_ECU_1                     0x01     // ECU 1
#define PUDS_ISO_15765_4_ADDR_ECU_2                     0x02     // ECU 2
#define PUDS_ISO_15765_4_ADDR_ECU_3                     0x03     // ECU 3
#define PUDS_ISO_15765_4_ADDR_ECU_4                     0x04     // ECU 4
#define PUDS_ISO_15765_4_ADDR_ECU_5                     0x05     // ECU 5
#define PUDS_ISO_15765_4_ADDR_ECU_6                     0x06     // ECU 6
#define PUDS_ISO_15765_4_ADDR_ECU_7                     0x07     // ECU 7
#define PUDS_ISO_15765_4_ADDR_ECU_8                     0x08     // ECU 8

// PUDS ISO_15765 Protocols
//
#define PUDS_PROTOCOL_ISO_15765_2_11B			0x01	// using PCAN-ISO-TP with 11 BIT CAN ID, NORMAL addressing and diagnostic message
#define PUDS_PROTOCOL_ISO_15765_2_11B_REMOTE	0x02	// using PCAN-ISO-TP with 11 BIT CAN ID, MIXED addressing and remote diagnostic message
#define PUDS_PROTOCOL_ISO_15765_2_29B			0x03	// using PCAN-ISO-TP with 29 BIT CAN ID, FIXED NORMAL addressing and diagnostic message
#define PUDS_PROTOCOL_ISO_15765_2_29B_REMOTE	0x04	// using PCAN-ISO-TP with 29 BIT CAN ID, MIXED addressing and remote diagnostic message
#define PUDS_PROTOCOL_ISO_15765_3_29B			0x05	// using PCAN-ISO-TP with Enhanced diagnostics 29 bit CAN Identifiers
#define PUDS_PROTOCOL_ISO_15765_2_29B_NORMAL	0x06	// using PCAN-ISO-TP with 29 BIT CAN ID, NORMAL addressing and diagnostic message (requires extra mapping definitions via PCAN-ISO-TP API)
#define PUDS_PROTOCOL_ISO_15765_2_11B_EXTENDED	0x07	// using PCAN-ISO-TP with 11 BIT CAN ID, EXTENDED addressing and diagnostic message (requires extra mapping definitions via PCAN-ISO-TP API)
#define PUDS_PROTOCOL_ISO_15765_2_29B_EXTENDED	0x08	// using PCAN-ISO-TP with 29 BIT CAN ID, EXTENDED addressing and diagnostic message (requires extra mapping definitions via PCAN-ISO-TP API)

// PUDS addressing types
//
#define PUDS_ADDRESSING_PHYSICAL      0x01     // Physical addressing
#define PUDS_ADDRESSING_FUNCTIONAL    0x02     // Functional addressing


// PUDS message types
//
#define PUDS_MESSAGE_TYPE_REQUEST			0x00     // UDS Request Message
#define PUDS_MESSAGE_TYPE_CONFIRM			0x01     // UDS Request/Response confirmation  Message
#define PUDS_MESSAGE_TYPE_INDICATION		0x02     // Incoming UDS Message

// TPUDSMsg constants
//
#define PUDS_MAX_DATA							4095
#define PUDS_SUPPR_POS_RSP_MSG_INDICATION_BIT	0x80	// Suppress positive response message 
#define PUDS_KEEP_POS_RSP_MSG_INDICATION_BIT	0x00	// Keep positive response message (default)



////////////////////////////////////////////////////////////
// Type definitions
////////////////////////////////////////////////////////////

#define TPUDSCANHandle			WORD		// Represents a CAN hardware channel of the underlying CAN system
#define TPUDSStatus				DWORD		// Represents a PUDS status/error code
#define TPUDSBaudrate			WORD		// Represents the baudrate register for the PUDS channel
#define TPUDSHWType				BYTE		// Represents the type of PCAN hardware to be initialized
#define TPUDSParameter			BYTE		// Represents a PUDS parameter to be read or set
#define TPUDSAddressingType		BYTE        // Represents a PUDS addressing type
#define TPUDSProtocol			BYTE        // Represents a PUDS protocol type
#define TPUDSMessageType		BYTE        // Represents a PUDS message type

// PCAN-UDS Network Addressing Information
typedef struct tagTPUDSNetAddrInfo 
{
	BYTE					SA;             // Source Address
	BYTE					TA;             // Target Address
	TPUDSAddressingType		TA_TYPE;        // Target Address Type (see PUDS_ADDRESSING_xxx)
	BYTE					RA;             // Remote address
	TPUDSProtocol			PROTOCOL;		// UDS Protocol (see PUDS_PROTOCOL_xxx)
} TPUDSNetAddrInfo;

// Diagnostic Session Information of a server
typedef struct tagTPUDSSessionInfo					
{
	TPUDSNetAddrInfo NETADDRINFO;			// Network address information
	BYTE SESSION_TYPE;						// Activated Diagnostic Session (see PUDS_SVC_PARAM_DSC_xxx values)
	WORD TIMEOUT_P2CAN_SERVER_MAX;			// Default P2Can_Server_Max timing for the activated session
	WORD TIMEOUT_ENHANCED_P2CAN_SERVER_MAX;	// Enhanced P2Can_Server_Max timing for the activated session
} TPUDSSessionInfo;

// A PCAN-UDS Message
typedef struct tagTPUDSMsg
{
	TPUDSNetAddrInfo NETADDRINFO;		// Network Addressing Information
	BYTE RESULT;						// Result status of the network communication
	BYTE NO_POSITIVE_RESPONSE_MSG;		// States wether Positive Response Message should be suppressed (see PUDS_SUPPR_POS_RSP_MSG_INDICATION_BIT & PUDS_KEEP_POS_RSP_MSG_INDICATION_BIT)
	WORD LEN;							// Data Length Code of the message	
	TPUDSMessageType MSGTYPE;			// Type of UDS Message (request, response, pending)
	union {
		BYTE RAW[PUDS_MAX_DATA];		// Raw Data
		
		struct tagREQUEST
		{
            BYTE		SI;							// Service code
            BYTE        PARAM[PUDS_MAX_DATA-1];     // Parameter 
		} REQUEST;						// Structure for message request

        struct tagPOSITIVE             
        {
            BYTE        SI;							// Service response code
            BYTE        PARAM[PUDS_MAX_DATA-1];     // Parameter
        } POSITIVE;						// Structure for positive service response

        struct tagNEGATIVE
        {
            BYTE        NR_SI;						// 0x7f in case of negative response
            BYTE        SI;							// Service response code
            BYTE        NRC;						// Response value
        } NEGATIVE;						// Structure for negative service response
	} DATA;								// Data of the message	

} TPUDSMsg;


////////////////////////////////////////////////////////////
// PCAN-UDS API function declarations
////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#define _DEF_ARG =0
#else
#define _DEF_ARG
#endif
		
////////////////////////////////////////////////////////////
// PCAN-UDS API : Core function declarations
////////////////////////////////////////////////////////////


/// <summary>
/// Initializes a PUDS-Client based on a PUDS-Channel
/// </summary>
/// <remarks>Only one UDS-Client can be initialized per CAN-Channel</remarks>
/// <param name="CanChannel">The PCAN-Basic channel to be used as UDS client</param>
/// <param name="Baudrate">The CAN Hardware speed</param>
/// <param name="HwType">NON PLUG&PLAY: The type of hardware and operation mode</param>
/// <param name="IOPort">NON PLUG&PLAY: The I/O address for the parallel port</param>
/// <param name="Interupt">NON PLUG&PLAY: Interrupt number of the parallel port</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_Initialize(
        TPUDSCANHandle CanChannel, 
        TPUDSBaudrate Baudrate, 
        TPUDSHWType HwType _DEF_ARG,
        DWORD IOPort _DEF_ARG, 
        WORD Interrupt _DEF_ARG);


/// <summary>
/// Uninitializes a PUDS-Client initialized before
/// </summary>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_Uninitialize(
        TPUDSCANHandle CanChannel);


/// <summary>
/// Resets the receive and transmit queues of a PUDS-Client 
/// </summary>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_Reset(
        TPUDSCANHandle CanChannel);


/// <summary>
/// Gets information about the internal BUS status of a PUDS CAN-Channel.
/// </summary>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_GetStatus(
        TPUDSCANHandle CanChannel);


/// <summary>
/// Reads a PUDS message from the receive queue of a PUDS-Client
/// </summary>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <param name="MessageBuffer">A TPUDSMsg structure buffer to store the PUDS message</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_Read(
        TPUDSCANHandle CanChannel, 
        TPUDSMsg* MessageBuffer);


/// <summary>
/// Transmits a PUDS message
/// </summary>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <param name="MessageBuffer">A TPUDSMsg buffer with the message to be sent</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_Write(
        TPUDSCANHandle CanChannel, 
        TPUDSMsg* MessageBuffer);


/// <summary>
/// Retrieves a PUDS-Client value
/// </summary>
///	<remark>
///	 * Parameter PUDS_PARAM_SERVER_ADDRESS uses 2 BYTE data to describe 
///	the physical address of the equipment, but the first BYTE is needed only 
///	for ISO-15765-3 Enhanced diagnostics 29 bit CAN ID where addresses 
///	are 11 bits long.
///	 * Parameter PUDS_PARAM_SERVER_FILTER uses 2 BYTE data to describe 
///	a functional address, but the first BYTE is needed only 
///	for ISO-15765-3 Enhanced diagnostics 29 bit CAN ID where addresses 
///	are 11 bits long ; the Most Significant Bit is used to define filter
///	status (see PUDS_SERVER_FILTER_LISTEN).
///	</remark>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <param name="Parameter">The TPUDSParameter parameter to get</param>
/// <param name="Buffer">Buffer for the parameter value</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_GetValue(
        TPUDSCANHandle CanChannel, 
        TPUDSParameter Parameter,  
        void* Buffer, 
        DWORD BufferLength);


/// <summary>
/// Configures or sets a PUDS-Client value
/// </summary>
///	<remark>
///	 * Parameter PUDS_PARAM_SERVER_ADDRESS uses 2 BYTE data to describe 
///	the physical address of the equipment, but the first BYTE is needed only 
///	for ISO-15765-3 Enhanced diagnostics 29 bit CAN ID where addresses 
///	are 11 bits long.
///	 * Parameter PUDS_PARAM_SERVER_FILTER uses 2 BYTE data to describe 
///	a functional address, but the first BYTE is needed only 
///	for ISO-15765-3 Enhanced diagnostics 29 bit CAN ID where addresses 
///	are 11 bits long ; the Most Significant Bit is used to define filter
///	status (see PUDS_SERVER_FILTER_LISTEN).
///	</remark>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <param name="Parameter">The TPUDSParameter parameter to set</param>
/// <param name="Buffer">Buffer with the value to be set</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SetValue(
        TPUDSCANHandle CanChannel,
        TPUDSParameter Parameter,
        void* Buffer,
		DWORD BufferLength);


////////////////////////////////////////////////////////////
// PCAN-UDS API : Utility function declarations
////////////////////////////////////////////////////////////

/// <summary>
/// Waits for a message (a response or a transmit confirmation) based on a UDS Message Request.
/// </summary>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <param name="MessageBuffer">A TPUDSMsg structure buffer to store the PUDS response</param>
/// <param name="MessageRequest">A sent TPUDSMsg message</param>
/// <param name="IsWaitForTransmit">The message to wait for is a Transmit Confirmation or not</param>
/// <param name="TimeInterval">Time interval to check for new message</param>
/// <param name="Timeout">Maximum time to wait for the message</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_WaitForSingleMessage(
		TPUDSCANHandle CanChannel,
		TPUDSMsg * MessageBuffer,
		TPUDSMsg * MessageRequest,
		BOOL IsWaitForTransmit,
		DWORD TimeInterval,
		DWORD Timeout);

/// <summary>
/// Waits for multiple messages (multiple responses from a functional request for instance) based on a UDS Message Request.
/// </summary>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <param name="Buffer">Buffer must be an array of 'MaxCount' entries (must have at least 
/// a size of iMaxCount * sizeof(TPUDSMsg) bytes</param>
/// <param name="MaxCount">Size of the Buffer array (max. messages that can be received)</param>
/// <param name="pCount">Buffer for the real number of messages read</param>
/// <param name="MessageRequest">A sent TPUDSMsg message</param>
/// <param name="TimeInterval">Time interval to check for new message</param>
/// <param name="Timeout">Maximum time to wait for the message</param>
/// <param name="TimeoutEnhanced">Maximum time to wait for the message in UDS Enhanced mode</param>
/// <param name="WaitUntilTimeout">if <code>FALSE</code> the function is interrupted if pCount reaches MaxCount.</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success, 
///	PUDS_ERROR_OVERFLOW indicates success but Buffer was too small to hold all responses.</returns>
TPUDSStatus __stdcall UDS_WaitForMultipleMessage(
		TPUDSCANHandle CanChannel,
		TPUDSMsg * Buffer,
		DWORD MaxCount,
		DWORD *pCount,
		TPUDSMsg * MessageRequest,
		DWORD TimeInterval,
		DWORD Timeout, 
		DWORD TimeoutEnhanced,
		BOOLEAN WaitUntilTimeout);

/// <summary>
/// Handles the communication workflow for a UDS service expecting a single response.
/// </summary>
///	<remark>
///	The function waits for a transmit confirmation then for a message response.
///	Even if the SuppressPositiveResponseMessage flag is set, the function will still wait 
/// for an eventual Negative Response.
///	</remark>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <param name="MessageBuffer">A TPUDSMsg structure buffer to store the PUDS response</param>
/// <param name="MessageRequest">A sent TPUDSMsg message</param>
/// <param name="MessageReqBuffer">A TPUDSMsg structure buffer to store the PUDS request confirmation 
///	(if <code>NULL</code>, the result confirmation will be set in MessageRequest parameter)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_WaitForService(
		TPUDSCANHandle CanChannel,
		TPUDSMsg * MessageBuffer,
		TPUDSMsg * MessageRequest,
		TPUDSMsg * MessageReqBuffer);

/// <summary>
/// Handles the communication workflow for a UDS service expecting multiple responses.
/// </summary>
///	<remark>
///	The function waits for a transmit confirmation then for N message responses.
///	Even if the SuppressPositiveResponseMessage flag is set, the function will still wait 
/// for eventual Negative Responses.
///	</remark>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <param name="Buffer">Buffer must be an array of 'MaxCount' entries (must have at least 
/// a size of iMaxCount * sizeof(TPUDSMsg) bytes</param>
/// <param name="MaxCount">Size of the Buffer array (max. messages that can be received)</param>
/// <param name="pCount">Buffer for the real number of messages read</param>
/// <param name="WaitUntilTimeout">if <code>FALSE</code> the function is interrupted if pCount reaches MaxCount.</param>
/// <param name="WaitUntilTimeout">Buffer for the real number of messages read</param>
/// <param name="MessageRequest">A sent TPUDSMsg message</param>
/// <param name="MessageReqBuffer">A TPUDSMsg structure buffer to store the PUDS request confirmation 
///	(if <code>NULL</code>, the result confirmation will be set in MessageRequest parameter)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success, 
///	PUDS_ERROR_OVERFLOW indicates success but Buffer was too small to hold all responses.</returns>
TPUDSStatus __stdcall UDS_WaitForServiceFunctional(
		TPUDSCANHandle CanChannel,
		TPUDSMsg * Buffer,
		DWORD MaxCount,
		DWORD *pCount,				
		BOOLEAN WaitUntilTimeout,
		TPUDSMsg * MessageRequest, 		
		TPUDSMsg * MessageReqBuffer);

/// <summary>
/// Process a UDS response message to manage ISO-14229/15765 features (like session handling).
/// </summary>
/// <param name="CanChannel">A PUDS CAN-Channel representing a PUDS-Client</param>
/// <param name="MessageBuffer">A TPUDSMsg structure buffer representing the PUDS Response Message</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_ProcessResponse(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer);

////////////////////////////////////////////////////////////
// PCAN-UDS API : UDS Service function declarations
////////////////////////////////////////////////////////////

// ISO-15765-3:2004 ?.2.1 p.42 & ISO-14229-1:2006 ?.2 p.36
#define PUDS_SVC_PARAM_DSC_DS			0x01	// Default Session
#define PUDS_SVC_PARAM_DSC_ECUPS		0x02	// ECU Programming Session
#define PUDS_SVC_PARAM_DSC_ECUEDS		0x03	// ECU Extended Diagnostic Session
#define PUDS_SVC_PARAM_DSC_SSDS			0x04	// Safety System Diagnostic Session
/// <summary>
/// The DiagnosticSessionControl service is used to enable different diagnostic sessions in the server.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="SessionType">Subfunction parameter: type of the session (see PUDS_SVC_PARAM_DSC_xxx)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcDiagnosticSessionControl(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE SessionType);


// ISO-15765-3:2004 ?.2.2 p.42 && ISO-14229-1:2006 ?.3 p.42
#define PUDS_SVC_PARAM_ER_HR			0x01	// Hard Reset
#define PUDS_SVC_PARAM_ER_KOFFONR		0x02	// Key Off on Reset
#define PUDS_SVC_PARAM_ER_SR			0x03	// Soft Reset
#define PUDS_SVC_PARAM_ER_ERPSD			0x04	// Enable Rapid Power Shutdown
#define PUDS_SVC_PARAM_ER_DRPSD			0x05	// Disable Rapid Power Shutdown
/// <summary>
/// The ECUReset service is used by the client to request a server reset.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="ResetType">Subfunction parameter: type of Reset (see PUDS_SVC_PARAM_ER_xxx)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcECUReset(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE ResetType);


// ISO-15765-3:2004 ?.2.3 p.43 && ISO-14229-1:2006 ?.4 p.45
#define PUDS_SVC_PARAM_SA_RSD_1			0x01	// Request Seed
#define PUDS_SVC_PARAM_SA_RSD_3			0x03	// Request Seed
#define PUDS_SVC_PARAM_SA_RSD_5			0x05	// Request Seed
#define PUDS_SVC_PARAM_SA_RSD_MIN		0x07	// Request Seed (odd numbers)
#define PUDS_SVC_PARAM_SA_RSD_MAX		0x5F	// Request Seed (odd numbers)
#define PUDS_SVC_PARAM_SA_SK_2			0x02	// Send Key
#define PUDS_SVC_PARAM_SA_SK_4			0x04	// Send Key
#define PUDS_SVC_PARAM_SA_SK_6			0x06	// Send Key
#define PUDS_SVC_PARAM_SA_SK_MIN		0x08	// Send Key (even numbers)
#define PUDS_SVC_PARAM_SA_SK_MAX		0x60	// Send Key (even numbers)
/// <summary>
/// SecurityAccess service provides a means to access data and/or diagnostic services which have
///	restricted access for security, emissions or safety reasons.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="SecurityAccessType">Subfunction parameter: type of SecurityAccess (see PUDS_SVC_PARAM_SA_xxx)</param>
/// <param name="Buffer">If Requesting Seed, buffer is the optional data to transmit to a server (like identification).
///	If Sending Key, data holds the value generated by the security algorithm corresponding to a specific “seed?value</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcSecurityAccess(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE SecurityAccessType,
	BYTE * Buffer, 
	WORD BufferLength);


// ISO-15765-3:2004 ?.2.4 p.43 && ISO-14229-1:2006 ?.5 p.52
#define PUDS_SVC_PARAM_CC_ERXTX					0x00	// Enable Rx and Tx
#define PUDS_SVC_PARAM_CC_ERXDTX				0x01	// Enable Rx and Disable Tx
#define PUDS_SVC_PARAM_CC_DRXETX				0x02	// Disable Rx and Enable Tx
#define PUDS_SVC_PARAM_CC_DRXTX					0x03	// Disable Rx and Tx
#define PUDS_SVC_PARAM_CC_FLAG_APPL				0x01	// Application (01b)
#define PUDS_SVC_PARAM_CC_FLAG_NWM				0x02	// NetworkManagement (10b)
#define PUDS_SVC_PARAM_CC_FLAG_DESCTIRNCN		0x00	// Disable/Enable specified communicationType (see Flags APPL/NMW)
														// in the receiving node and all connected networks
#define PUDS_SVC_PARAM_CC_FLAG_DENWRIRO			0xF0	// Disable/Enable network which request is received on
#define PUDS_SVC_PARAM_CC_FLAG_DESNIBNN_MIN		0x10	// Disable/Enable specific network identified by network number (minimum value)
#define PUDS_SVC_PARAM_CC_FLAG_DESNIBNN_MAX		0xE0	// Disable/Enable specific network identified by network number (maximum value)
#define PUDS_SVC_PARAM_CC_FLAG_DESNIBNN_MASK	0xF0	// Mask for DESNIBNN bits
/// <summary>
///	CommunicationControl service's purpose is to switch on/off the transmission 
///	and/or the reception of certain messages of (a) server(s).
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="ControlType">Subfunction parameter: type of CommunicationControl (see PUDS_SVC_PARAM_CC_xxx)</param>
/// <param name="CommunicationType">a bit-code value to reference the kind of communication to be controlled,
///	See PUDS_SVC_PARAM_CC_FLAG_xxx flags and ISO_14229-2006 §B.1 for bit-encoding</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcCommunicationControl(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE ControlType,	
	BYTE CommunicationType);


// ISO-15765-3:2004 ?.2.5 p.43 && ISO-14229-1:2006 ?.6 p.55
#define PUDS_SVC_PARAM_TP_ZSUBF			0x00	// Zero SubFunction
/// <summary>
///	TesterPresent service indicates to a server (or servers) that a client is still connected
///	to the vehicle and that certain diagnostic services and/or communications 
///	that have been previously activated are to remain active.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="TesterPresentType">No Subfunction parameter by default (PUDS_SVC_PARAM_TP_ZSUBF)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcTesterPresent(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE TesterPresentType);


// ISO-15765-3:2004 ?.2.6 p.44 && ISO-14229-1:2006 ?.8 p.63
/// <summary>
///	SecuredDataTransmission service's purpose is to transmit data that is protected 
///	against attacks from third parties, which could endanger data security.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="Buffer">buffer containing the data as processed by the Security Sub-Layer (See ISO-15764)</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcSecuredDataTransmission(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE * Buffer,
	WORD BufferLength);	


// ISO-15765-3:2004 ?.2.7 p.44 && ISO-14229-1:2006 ?.9 p.69
#define PUDS_SVC_PARAM_CDTCS_ON			0x01	// The server(s) shall resume the setting of diagnostic trouble codes
#define PUDS_SVC_PARAM_CDTCS_OFF		0x02	// The server(s) shall stop the setting of diagnostic trouble codes
/// <summary>
///	ControlDTCSetting service shall be used by a client to stop or resume the setting of 
///	diagnostic trouble codes (DTCs) in the server(s).
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="DTCSettingType">Subfunction parameter (see PUDS_SVC_PARAM_CDTCS_xxx)</param>
/// <param name="Buffer">This parameter record is user-optional and transmits data to a server when controlling the DTC setting. 
///	It can contain a list of DTCs to be turned on or off.</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcControlDTCSetting(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE DTCSettingType,
	BYTE * Buffer,
	WORD BufferLength);


// ISO-15765-3:2004 ?.2.8 p.44 && ISO-14229-1:2006 ?.10 p.73
#define PUDS_SVC_PARAM_ROE_STPROE		0x00	// Stop Response On Event
#define PUDS_SVC_PARAM_ROE_ONDTCS		0x01	// On DTC Status Change
#define PUDS_SVC_PARAM_ROE_OTI			0x02	// On Timer Interrupt
#define PUDS_SVC_PARAM_ROE_OCODID		0x03	// On Change Of Data Identifier
#define PUDS_SVC_PARAM_ROE_RAE			0x04	// Report Activated Events
#define PUDS_SVC_PARAM_ROE_STRTROE		0x05	// Start Response On Event
#define PUDS_SVC_PARAM_ROE_CLRROE		0x06	// Clear Response On Event
#define PUDS_SVC_PARAM_ROE_OCOV			0x07	// On Comparison Of Values
#define PUDS_SVC_PARAM_ROE_STPROE_LEN	0	// expected size of EventTypeRecord for ROE_STPROE
#define PUDS_SVC_PARAM_ROE_ONDTCS_LEN	1	// expected size of EventTypeRecord for ROE_ONDTCS
#define PUDS_SVC_PARAM_ROE_OTI_LEN		1	// expected size of EventTypeRecord for ROE_OTI
#define PUDS_SVC_PARAM_ROE_OCODID_LEN	2	// expected size of EventTypeRecord for ROE_OCODID
#define PUDS_SVC_PARAM_ROE_RAE_LEN		0	// expected size of EventTypeRecord for ROE_RAE
#define PUDS_SVC_PARAM_ROE_STRTROE_LEN	0	// expected size of EventTypeRecord for ROE_STRTROE
#define PUDS_SVC_PARAM_ROE_CLRROE_LEN	0	// expected size of EventTypeRecord for ROE_CLRROE
#define PUDS_SVC_PARAM_ROE_OCOV_LEN		10	// expected size of EventTypeRecord for ROE_OCOV
#define PUDS_SVC_PARAM_ROE_EWT_ITTR		0x02	// Infinite Time To Response (eventWindowTime parameter)
#define PUDS_SVC_PARAM_ROE_STRT_SI_RDBI		PUDS_SI_ReadDataByIdentifier			// Recommended service (first byte of ServiceToRespondTo Record)
#define PUDS_SVC_PARAM_ROE_STRT_SI_RDTCI	PUDS_SI_ReadDTCInformation				// Recommended service (first byte of ServiceToRespondTo Record)
#define PUDS_SVC_PARAM_ROE_STRT_SI_RC		PUDS_SI_RoutineControl					// Recommended service (first byte of ServiceToRespondTo Record)
#define PUDS_SVC_PARAM_ROE_STRT_SI_IOCBI	PUDS_SI_InputOutputControlByIdentifier	// Recommended service (first byte of ServiceToRespondTo Record)
/// <summary>
///	The ResponseOnEvent service requests a server to 
///	start or stop transmission of responses on a specified event.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="EventType">Subfunction parameter: event type (see PUDS_SVC_PARAM_ROE_xxx)</param>
/// <param name="StoreEvent">Storage State (TRUE = Store Event, FALSE = Do Not Store Event)</param>
/// <param name="EventWindowTime">Specify a window for the event logic to be active in the server (see PUDS_SVC_PARAM_ROE_EWT_ITTR)</param>
/// <param name="EventTypeRecord">Additional parameters for the specified eventType</param>
/// <param name="EventTypeRecordLength">Size in bytes of the EventType Record (see PUDS_SVC_PARAM_ROE_xxx_LEN)</param>
/// <param name="ServiceToRespondToRecord">Service parameters, with first byte as service Id (see PUDS_SVC_PARAM_ROE_STRT_SI_xxx)</param>
/// <param name="ServiceToRespondToRecordLength">Size in bytes of the ServiceToRespondTo Record</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcResponseOnEvent(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE EventType,
	BOOLEAN StoreEvent,
	BYTE EventWindowTime,
	BYTE * EventTypeRecord,
	WORD EventTypeRecordLength,
	BYTE * ServiceToRespondToRecord,
	WORD ServiceToRespondToRecordLength);


// ISO-15765-3:2004 ?.2.9 p.47 && ISO-14229-1:2006 ?.11 p.91
#define PUDS_SVC_PARAM_LC_VBTWFBR				0x01	// Verify Baudrate Transition With Fixed Baudrate
#define PUDS_SVC_PARAM_LC_VBTWSBR				0x02	// Verify Baudrate Transition With Specific Baudrate
#define PUDS_SVC_PARAM_LC_TB					0x03	// Transition Baudrate
#define PUDS_SVC_PARAM_LC_BAUDRATE_PC_9600		0x01	// standard PC baud rate of 9.6 KBaud
#define PUDS_SVC_PARAM_LC_BAUDRATE_PC_19200		0x02	// standard PC baud rate of 19.2 KBaud
#define PUDS_SVC_PARAM_LC_BAUDRATE_PC_38400		0x03	// standard PC baud rate of 38.4 KBaud
#define PUDS_SVC_PARAM_LC_BAUDRATE_PC_57600		0x04	// standard PC baud rate of 57.6 KBaud
#define PUDS_SVC_PARAM_LC_BAUDRATE_PC_115200	0x05	// standard PC baud rate of 115.2 KBaud
#define PUDS_SVC_PARAM_LC_BAUDRATE_CAN_125K		0x10	// standard CAN baud rate of 125 KBaud
#define PUDS_SVC_PARAM_LC_BAUDRATE_CAN_250K		0x11	// standard CAN baud rate of 250 KBaud
#define PUDS_SVC_PARAM_LC_BAUDRATE_CAN_500K		0x12	// standard CAN baud rate of 500 KBaud
#define PUDS_SVC_PARAM_LC_BAUDRATE_CAN_1M		0x13	// standard CAN baud rate of 1 MBaud
/// <summary>
///	The LinkControl service is used to control the communication link baud rate
///	between the client and the server(s) for the exchange of diagnostic data.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="LinkControlType">Subfunction parameter: Link Control Type (see PUDS_SVC_PARAM_LC_xxx)</param>
/// <param name="BaudrateIdentifier">defined baud rate identifier (see PUDS_SVC_PARAM_LC_BAUDRATE_xxx)</param>
/// <param name="LinkBaudrate">used only with PUDS_SVC_PARAM_LC_VBTWSBR parameter: 
///	a three-byte value baud rate (baudrate High, Middle and Low Bytes).
///	</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success </returns>
TPUDSStatus __stdcall UDS_SvcLinkControl(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE LinkControlType,
	BYTE BaudrateIdentifier,
	DWORD LinkBaudrate);


// Data Identifiers ISO-14229-1:2006 §C.1 p.259
#define PUDS_SVC_PARAM_DI_BSIDID		0xF180	// bootSoftwareIdentificationDataIdentifier
#define PUDS_SVC_PARAM_DI_ASIDID		0xF181	// applicationSoftwareIdentificationDataIdentifier
#define PUDS_SVC_PARAM_DI_ADIDID		0xF182	// applicationDataIdentificationDataIdentifier
#define PUDS_SVC_PARAM_DI_BSFPDID		0xF183	// bootSoftwareIdentificationDataIdentifier
#define PUDS_SVC_PARAM_DI_ASFPDID		0xF184	// applicationSoftwareFingerprintDataIdentifier
#define PUDS_SVC_PARAM_DI_ADFPDID		0xF185	// applicationDataFingerprintDataIdentifier
#define PUDS_SVC_PARAM_DI_ADSDID		0xF186	// activeDiagnosticSessionDataIdentifier
#define PUDS_SVC_PARAM_DI_VMSPNDID		0xF187	// vehicleManufacturerSparePartNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_VMECUSNDID	0xF188	// vehicleManufacturerECUSoftwareNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_VMECUSVNDID	0xF189	// vehicleManufacturerECUSoftwareVersionNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_SSIDDID		0xF18A	// systemSupplierIdentifierDataIdentifier
#define PUDS_SVC_PARAM_DI_ECUMDDID		0xF18B	// ECUManufacturingDateDataIdentifier
#define PUDS_SVC_PARAM_DI_ECUSNDID		0xF18C	// ECUSerialNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_SFUDID		0xF18D	// supportedFunctionalUnitsDataIdentifier
#define PUDS_SVC_PARAM_DI_VMKAPNDID		0xF18E	// vehicleManufacturerKitAssemblyPartNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_VINDID		0xF190	// VINDataIdentifier
#define PUDS_SVC_PARAM_DI_VMECUHNDID	0xF191	// vehicleManufacturerECUHardwareNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_SSECUHWNDID	0xF192	// systemSupplierECUHardwareNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_SSECUHWVNDID	0xF193	// systemSupplierECUHardwareVersionNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_SSECUSWNDID	0xF194	// systemSupplierECUSoftwareNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_SSECUSWVNDID	0xF195	// systemSupplierECUSoftwareVersionNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_EROTANDID		0xF196	// exhaustRegulationOrTypeApprovalNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_SNOETDID		0xF197	// systemNameOrEngineTypeDataIdentifier
#define PUDS_SVC_PARAM_DI_RSCOTSNDID	0xF198	// repairShopCodeOrTesterSerialNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_PDDID			0xF199	// programmingDateDataIdentifier
#define PUDS_SVC_PARAM_DI_CRSCOCESNDID	0xF19A	// calibrationRepairShopCodeOrCalibrationEquipmentSerialNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_CDDID			0xF19B	// calibrationDateDataIdentifier
#define PUDS_SVC_PARAM_DI_CESWNDID		0xF19C	// calibrationEquipmentSoftwareNumberDataIdentifier
#define PUDS_SVC_PARAM_DI_EIDDID		0xF19D	// ECUInstallationDateDataIdentifier
#define PUDS_SVC_PARAM_DI_ODXFDID		0xF19E	// ODXFileDataIdentifier
#define PUDS_SVC_PARAM_DI_EDID			0xF19F	// entityDataIdentifier

// ISO-15765-3:2004 ?.3.1 p.47 && ISO-14229-1:2006 ?0.2 p.97
/// <summary>
///	The ReadDataByIdentifier service allows the client to request data record values 
///	from the server identified by one or more dataIdentifiers.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="Buffer">buffer containing a list of two-byte Data Identifiers (see PUDS_SVC_PARAM_DI_xxx)</param>
/// <param name="BufferLength">Number of elements in the buffer (size in WORD of the buffer)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadDataByIdentifier(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	WORD * Buffer,
	WORD BufferLength);


// ISO-15765-3:2004 ?.3.2 p.47 && ISO-14229-1:2006 ?0.3 p.102
/// <summary>
///	The ReadMemoryByAddress service allows the client to request memory data from the server 
///	via a provided starting address and to specify the size of memory to be read.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="MemoryAddress">starting address of server memory from which data is to be retrieved</param>
/// <param name="MemoryAddressLength">Size in bytes of the MemoryAddress buffer (max.: 0xF)</param>
/// <param name="MemorySize">number of bytes to be read starting at the address specified by memoryAddress</param>
/// <param name="MemorySizeLength">Size in bytes of the MemorySize buffer (max.: 0xF)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadMemoryByAddress(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE * MemoryAddress,
	BYTE MemoryAddressLength,
	BYTE * MemorySize,		
	BYTE MemorySizeLength);


// ISO-15765-3:2004 ?.3.3 p.48 && ISO-14229-1:2006 ?0.4 p.106
/// <summary>
///	The ReadScalingDataByIdentifier service allows the client to request 
///	scaling data record information from the server identified by a dataIdentifier.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="DataIdentifier">a two-byte Data Identifier (see PUDS_SVC_PARAM_DI_xxx)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadScalingDataByIdentifier(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	WORD DataIdentifier);


// ISO-15765-3:2004 ?.3.4 p.48 && ISO-14229-1:2006 ?0.5 p.112
#define PUDS_SVC_PARAM_RDBPI_SASR		0x01	// Send At Slow Rate
#define PUDS_SVC_PARAM_RDBPI_SAMR		0x02	// Send At Medium Rate
#define PUDS_SVC_PARAM_RDBPI_SAFR		0x03	// Send At Fast Rate
#define PUDS_SVC_PARAM_RDBPI_SS			0x04	// Stop Sending
/// <summary>
///	The ReadDataByPeriodicIdentifier service allows the client to request the periodic transmission 
///	of data record values from the server identified by one or more periodicDataIdentifiers.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="TransmissionMode">transmission rate code (see PUDS_SVC_PARAM_RDBPI_xxx)</param>
/// <param name="Buffer">buffer containing a list of Periodic Data Identifiers</param>
/// <param name="BufferLength">Number of elements in the buffer (size in WORD of the buffer)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadDataByPeriodicIdentifier(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE TransmissionMode,
	BYTE * Buffer,
	WORD BufferLength);


// ISO-15765-3:2004 ?.3.5 p.54 && ISO-14229-1:2006 ?0.6 p.123
#define PUDS_SVC_PARAM_DDDI_DBID		0x01	// Define By Identifier
#define PUDS_SVC_PARAM_DDDI_DBMA		0x02	// Define By Memory Address
#define PUDS_SVC_PARAM_DDDI_CDDDI		0x03	// Clear Dynamically Defined Data Identifier
/// <summary>
///	The DynamicallyDefineDataIdentifier service allows the client to dynamically define 
///	in a server a data identifier that can be read via the ReadDataByIdentifier service at a later time.
///	The Define By Identifier subfunction specifies that definition of the dynamic data
///	identifier shall occur via a data identifier reference.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="DynamicallyDefinedDataIdentifier">a two-byte Data Identifier (see PUDS_SVC_PARAM_DI_xxx)</param>
/// <param name="SourceDataIdentifier">buffer containing the sources of information to be included into the dynamic data record</param>
/// <param name="MemorySize">buffer containing the total numbers of bytes from the source data record address</param>
/// <param name="PositionInSourceDataRecord">buffer containing the starting byte positions of the excerpt of the source data record</param>
/// <param name="BuffersLength">Number of elements in the buffers (SourceDataIdentifier, MemoryAddress and PositionInSourceDataRecord)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcDynamicallyDefineDataIdentifierDBID(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	WORD DynamicallyDefinedDataIdentifier,
	WORD * SourceDataIdentifier,
	BYTE * MemorySize,
	BYTE * PositionInSourceDataRecord,
	WORD BuffersLength);
/// <summary>
///	The DynamicallyDefineDataIdentifier service allows the client to dynamically define 
///	in a server a data identifier that can be read via the ReadDataByIdentifier service at a later time.
///	The Define By Memory Address subfunction specifies that definition of the dynamic data
///	identifier shall occur via an address reference.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="DynamicallyDefinedDataIdentifier">a two-byte Data Identifier (see PUDS_SVC_PARAM_DI_xxx)</param>
/// <param name="MemoryAddressLength">Size in bytes of the MemoryAddress items in the MemoryAddressBuffer buffer (max.: 0xF)</param>
/// <param name="MemorySizeLength">Size in bytes of the MemorySize items in the MemorySizeBuffer buffer (max.: 0xF)</param>
/// <param name="MemoryAddressBuffer">buffer containing the MemoryAddress buffers,
///	must be an array of 'BuffersLength' entries which contains 'MemoryAddressLength' bytes
///	(size is 'BuffersLength * MemoryAddressLength' bytes)</param>
/// <param name="MemorySizeBuffer">buffer containing the MemorySize buffers,
///	must be an array of 'BuffersLength' entries which contains 'MemorySizeLength' bytes
///	(size is 'BuffersLength * MemorySizeLength' bytes)</param>
/// <param name="BuffersLength">Size in bytes of the MemoryAddressBuffer and MemorySizeBuffer buffers</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcDynamicallyDefineDataIdentifierDBMA(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	WORD DynamicallyDefinedDataIdentifier,
	BYTE MemoryAddressLength,
	BYTE MemorySizeLength,
	BYTE * MemoryAddressBuffer,
	BYTE * MemorySizeBuffer,
	WORD BuffersLength);
/// <summary>
///	The Clear Dynamically Defined Data Identifier subfunction shall be used to clear 
///	the specified dynamic data identifier.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="DynamicallyDefinedDataIdentifier">a two-byte Data Identifier (see PUDS_SVC_PARAM_DI_xxx)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcDynamicallyDefineDataIdentifierCDDDI(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	WORD DynamicallyDefinedDataIdentifier);


// ISO-15765-3:2004 ?.3.6 p.54 && ISO-14229-1:2006 ?0.7 p.143
/// <summary>
///	The WriteDataByIdentifier service allows the client to write information into the server at an internal location
///	specified by the provided data identifier.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="DataIdentifier">a two-byte Data Identifier (see PUDS_SVC_PARAM_DI_xxx)</param>
/// <param name="Buffer">buffer containing the data to write</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcWriteDataByIdentifier(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	WORD DataIdentifier,
	BYTE * Buffer,
	WORD BufferLength);


// ISO-15765-3:2004 ?.3.7 p.54 && ISO-14229-1:2006 ?0.8 p.146
/// <summary>
///	The WriteMemoryByAddress service allows the client to write 
///	information into the server at one or more contiguous memory locations.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="DataIdentifier">a two-byte Data Identifier (see PUDS_SVC_PARAM_DI_xxx)</param>
/// <param name="MemoryAddress">starting address of server memory to which data is to be written</param>
/// <param name="MemoryAddressLength">Size in bytes of the MemoryAddress buffer (max.: 0xF)</param>
/// <param name="MemorySize">number of bytes to be written starting at the address specified by memoryAddress</param>
/// <param name="MemorySizeLength">Size in bytes of the MemorySize buffer (max.: 0xF)</param>
/// <param name="Buffer">buffer containing the data to write</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcWriteMemoryByAddress(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	WORD DataIdentifier,
	BYTE * MemoryAddress,
	BYTE MemoryAddressLength,
	BYTE * MemorySize,
	BYTE MemorySizeLength,
	BYTE * Buffer,
	WORD BufferLength);


// ISO-15765-3:2004 ?.4.2 p.56 && ISO-14229-1:2006 ?1.2 p.152
#define PUDS_SVC_PARAM_CDI_ERS		0x000000		// Emissions-related systems group of DTCs
#define PUDS_SVC_PARAM_CDI_AGDTC	0xFFFFFF		// All Groups of DTCs
/// <summary>
///	The ClearDiagnosticInformation service is used by the client to clear diagnostic information 
///	in one server's or multiple servers?memory.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="groupOfDTC">a three-byte value indicating the group of DTCs (e.g. powertrain, body, chassis) 
/// or the particular DTC to be cleared (see PUDS_SVC_PARAM_CDI_xxx)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcClearDiagnosticInformation(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	DWORD groupOfDTC);


// ISO-15765-3:2004 ?.4.1 p.54 && ISO-14229-1:2006 ?1.3 p.154
#define PUDS_SVC_PARAM_RDTCI_RNODTCBSM			0x01	// report Number Of DTC By Status Mask
#define PUDS_SVC_PARAM_RDTCI_RDTCBSM			0x02	// report DTC By Status Mask
#define PUDS_SVC_PARAM_RDTCI_RDTCSSI			0x03	// report DTC Snapshot Identification
#define PUDS_SVC_PARAM_RDTCI_RDTCSSBDTC			0x04	// report DTC Snapshot Record By DTC Number
#define PUDS_SVC_PARAM_RDTCI_RDTCSSBRN			0x05	// report DTC Snapshot Record By Record Number
#define PUDS_SVC_PARAM_RDTCI_RDTCEDRBDN			0x06	// report DTC Extended Data Record By DTC Number
#define PUDS_SVC_PARAM_RDTCI_RNODTCBSMR			0x07	// report Number Of DTC By Severity Mask Record
#define PUDS_SVC_PARAM_RDTCI_RDTCBSMR			0x08	// report DTC By Severity Mask Record
#define PUDS_SVC_PARAM_RDTCI_RSIODTC			0x09	// report Severity Information Of DTC
#define PUDS_SVC_PARAM_RDTCI_RSUPDTC			0x0A	// report Supported DTC
#define PUDS_SVC_PARAM_RDTCI_RFTFDTC			0x0B	// report First Test Failed DTC
#define PUDS_SVC_PARAM_RDTCI_RFCDTC				0x0C	// report First Confirmed DTC
#define PUDS_SVC_PARAM_RDTCI_RMRTFDTC			0x0D	// report Most Recent Test Failed DTC
#define PUDS_SVC_PARAM_RDTCI_RMRCDTC			0x0E	// report Most Recent Confirmed DTC
#define PUDS_SVC_PARAM_RDTCI_RMMDTCBSM			0x0F	// report Mirror Memory DTC By Status Mask
#define PUDS_SVC_PARAM_RDTCI_RMMDEDRBDN			0x10	// report Mirror Memory DTC Extended Data Record By DTC Number
#define PUDS_SVC_PARAM_RDTCI_RNOMMDTCBSM		0x11	// report Number Of Mirror MemoryDTC By Status Mask
#define PUDS_SVC_PARAM_RDTCI_RNOOBDDTCBSM		0x12	// report Number Of Emissions Related OBD DTC By Status Mask
#define PUDS_SVC_PARAM_RDTCI_ROBDDTCBSM			0x13	// report Emissions Related OBD DTC By Status Mask
// DTCSeverityMask (DTCSVM) : ISO-14229-1:2006 §D.3 p.285
#define PUDS_SVC_PARAM_RDTCI_DTCSVM_NSA			0x00	// DTC severity bit definitions : no SeverityAvailable
#define PUDS_SVC_PARAM_RDTCI_DTCSVM_MO			0x20	// DTC severity bit definitions : maintenance Only
#define PUDS_SVC_PARAM_RDTCI_DTCSVM_CHKANH		0x40	// DTC severity bit definitions : check At Next Halt
#define PUDS_SVC_PARAM_RDTCI_DTCSVM_CHKI		0x80	// DTC severity bit definitions : check Immediately

/// <summary>
///	This service allows a client to read the status of server-resident Diagnostic Trouble Code (DTC) information.
/// Only reportNumberOfDTCByStatusMask, reportDTCByStatusMask, reportMirrorMemoryDTCByStatusMask,
///	reportNumberOfMirrorMemoryDTCByStatusMask, reportNumberOfEmissionsRelatedOBDDTCByStatusMask, 
///	reportEmissionsRelatedOBDDTCByStatusMask Sub-functions are allowed.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="RDTCIType">Subfunction parameter: ReadDTCInformation type, use one of the following:
///	PUDS_SVC_PARAM_RDTCI_RNODTCBSM, PUDS_SVC_PARAM_RDTCI_RDTCBSM,
///	PUDS_SVC_PARAM_RDTCI_RMMDTCBSM, PUDS_SVC_PARAM_RDTCI_RNOMMDTCBSM,
///	PUDS_SVC_PARAM_RDTCI_RNOOBDDTCBSM, PUDS_SVC_PARAM_RDTCI_ROBDDTCBSM</param>
/// <param name="DTCStatusMask">Contains eight DTC status bit.</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadDTCInformation(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE RDTCIType,
	BYTE DTCStatusMask);

/// <summary>
///	This service allows a client to read the status of server-resident Diagnostic Trouble Code (DTC) information.
///	The sub-function reportDTCSnapshotRecordByDTCNumber (PUDS_SVC_PARAM_RDTCI_RDTCSSBDTC) is implicit.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="DTCMask">a unique identification number (three byte value) for a specific diagnostic trouble code</param>
/// <param name="DTCSnapshotRecordNumber">the number of the specific DTCSnapshot data records</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadDTCInformationRDTCSSBDTC(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	DWORD DTCMask, 
	BYTE DTCSnapshotRecordNumber);

/// <summary>
///	This service allows a client to read the status of server-resident Diagnostic Trouble Code (DTC) information.
///	The sub-function reportDTCSnapshotByRecordNumber (PUDS_SVC_PARAM_RDTCI_RDTCSSBRN) is implicit.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="DTCSnapshotRecordNumber">the number of the specific DTCSnapshot data records</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadDTCInformationRDTCSSBRN(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE DTCSnapshotRecordNumber);

/// <summary>
///	This service allows a client to read the status of server-resident Diagnostic Trouble Code (DTC) information.
/// Only reportDTCExtendedDataRecordByDTCNumber and reportMirrorMemoryDTCExtendedDataRecordByDTCNumber Sub-functions are allowed.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="RDTCIType">Subfunction parameter: ReadDTCInformation type, use one of the following:
///	PUDS_SVC_PARAM_RDTCI_RDTCEDRBDN, PUDS_SVC_PARAM_RDTCI_RMMDEDRBDN</param>
/// <param name="DTCMask">a unique identification number (three byte value) for a specific diagnostic trouble code</param>
/// <param name="DTCExtendedDataRecordNumber">the number of the specific DTCExtendedData record requested.</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadDTCInformationReportExtended(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE RDTCIType,
	DWORD DTCMask, 
	BYTE DTCExtendedDataRecordNumber);

/// <summary>
///	This service allows a client to read the status of server-resident Diagnostic Trouble Code (DTC) information.
/// Only reportNumberOfDTCBySeverityMaskRecord and reportDTCSeverityInformation Sub-functions are allowed.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="RDTCIType">Subfunction parameter: ReadDTCInformation type, use one of the following:
///	PUDS_SVC_PARAM_RDTCI_RNODTCBSMR, PUDS_SVC_PARAM_RDTCI_RDTCBSMR</param>
/// <param name="DTCSeverityMask">a mask of eight (8) DTC severity bits (see PUDS_SVC_PARAM_RDTCI_DTCSVM_xxx)</param>
/// <param name="DTCStatusMask">a mask of eight (8) DTC status bits</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadDTCInformationReportSeverity(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE RDTCIType,
	BYTE DTCSeverityMask, 
	BYTE DTCStatusMask);

/// <summary>
///	This service allows a client to read the status of server-resident Diagnostic Trouble Code (DTC) information.
///	The sub-function reportSeverityInformationOfDTC (PUDS_SVC_PARAM_RDTCI_RSIODTC) is implicit.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="DTCMask">a unique identification number for a specific diagnostic trouble code</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadDTCInformationRSIODTC(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	DWORD DTCMask);

/// <summary>
///	This service allows a client to read the status of server-resident Diagnostic Trouble Code (DTC) information.
/// Only reportSupportedDTC, reportFirstTestFailedDTC, reportFirstConfirmedDTC, reportMostRecentTestFailedDTC,
///	reportMostRecentConfirmedDTC, reportDTCFaultDetectionCounter, reportDTCWithPermanentStatus, 
/// and reportDTCSnapshotIdentification Sub-functions are allowed.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="RDTCIType">Subfunction parameter: ReadDTCInformation type, use one of the following:
///	PUDS_SVC_PARAM_RDTCI_RFTFDTC, PUDS_SVC_PARAM_RDTCI_RFCDTC, 
///	PUDS_SVC_PARAM_RDTCI_RMRTFDTC, PUDS_SVC_PARAM_RDTCI_RMRCDTC, 
///	PUDS_SVC_PARAM_RDTCI_RSUPDTC, PUDS_SVC_PARAM_RDTCI_RDTCWPS,
///	PUDS_SVC_PARAM_RDTCI_RDTCSSI</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcReadDTCInformationNoParam(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE RDTCIType);


// ISO-15765-3:2004 ?.5.1 p.56 && ISO-14229-1:2006 ?2.2 p.209, (inputOutputControlParameter §E.1 p.289)
#define PUDS_SVC_PARAM_IOCBI_RCTECU			0x00	// inputOutputControlParameter : returnControlToECU (0 controlState bytes in request)
#define PUDS_SVC_PARAM_IOCBI_RTD			0x01	// inputOutputControlParameter : resetToDefault (0 controlState bytes in request)
#define PUDS_SVC_PARAM_IOCBI_FCS			0x02	// inputOutputControlParameter : freezeCurrentState (0 controlState bytes in request)
#define PUDS_SVC_PARAM_IOCBI_STA			0x03	// inputOutputControlParameter : shortTermAdjustment
/// <summary>
///	The InputOutputControlByIdentifier service is used by the client to substitute a value for an input signal,
///	internal server function and/or control an output (actuator) of an electronic system.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="DataIdentifier">a two-byte Data Identifier (see PUDS_SVC_PARAM_DI_xxx)</param>
/// <param name="ControlOptionRecord">First byte can be used as either an InputOutputControlParameter 
///	that describes how the server shall control its inputs or outputs (see PUDS_SVC_PARAM_IOCBI_xxx),
///	or as an additional controlState byte</param>
/// <param name="ControlOptionRecordLength">Size in bytes of the ControlOptionRecord buffer</param>
/// <param name="ControlEnableMaskRecord">The ControlEnableMask shall only be supported when 
///	the inputOutputControlParameter is used and the dataIdentifier to be controlled consists 
///	of more than one parameter (i.e. the dataIdentifier is bit-mapped or packeted by definition). 
///	There shall be one bit in the ControlEnableMask corresponding to each individual parameter 
///	defined within the dataIdentifier.</param>
/// <param name="ControlEnableMaskRecordLength">Size in bytes of the controlEnableMaskRecord buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcInputOutputControlByIdentifier(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer, 
	WORD DataIdentifier, 
	BYTE * ControlOptionRecord, 
	WORD ControlOptionRecordLength, 
	BYTE * ControlEnableMaskRecord, 
	WORD ControlEnableMaskRecordLength);


// ISO-15765-3:2004 ?.6.1 p.56 && ISO-14229-1:2006 ?3.2 p.225
#define PUDS_SVC_PARAM_RC_STR			0x01	// Start Routine
#define PUDS_SVC_PARAM_RC_STPR			0x02	// Stop Routine
#define PUDS_SVC_PARAM_RC_RRR			0x03	// Request Routine Results
// routineIdentifier: ISO-14229-1:2006 §F.1 p.290
#define PUDS_SVC_PARAM_RC_RID_DLRI_		0xE200	// routineIdentifier : DeployLoopRoutineID
#define PUDS_SVC_PARAM_RC_RID_EM_		0xFF00	// routineIdentifier : eraseMemory
#define PUDS_SVC_PARAM_RC_RID_CPD_		0xFF01	// routineIdentifier : checkProgrammingDependencies
#define PUDS_SVC_PARAM_RC_RID_EMMDTC_	0xFF02	// routineIdentifier : eraseMirrorMemoryDTCs
/// <summary>
///	The RoutineControl service is used by the client to start/stop a routine,
///	and request routine results.
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message</param>
/// <param name="RoutineControlType">Subfunction parameter: RoutineControl type (see PUDS_SVC_PARAM_RC_xxx)</param>
/// <param name="RoutineIdentifier">Server Local Routine Identifier (see PUDS_SVC_PARAM_RC_RID_xxx)</param>
/// <param name="Buffer">buffer containing the Routine Control Options (only with start and stop routine sub-functions)</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcRoutineControl(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE RoutineControlType, 
	WORD RoutineIdentifier, 
	BYTE * Buffer,
	WORD BufferLength);


// ISO-15765-3:2004 ?.7.1 p.57 && ISO-14229-1:2006 ?4.2 p.231
/// <summary>
///	The requestDownload service is used by the client to initiate a data transfer 
///	from the client to the server (download).
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="CompressionMethod">A nibble-value that specifies the "compressionMethod",	
///	The value 0x0 specifies that no compressionMethod is used.</param>
/// <param name="EncryptingMethod">A nibble-value that specifies the "encryptingMethod",
///	The value 0x0 specifies that no encryptingMethod is used.</param>
/// <param name="MemoryAddress">starting address of server memory to which data is to be written</param>
/// <param name="MemoryAddressLength">Size in bytes of the MemoryAddress buffer (max.: 0xF)</param>
/// <param name="MemorySize">used by the server to compare the uncompressed memory size with 
///	the total amount of data transferred during the TransferData service</param>
/// <param name="MemorySizeLength">Size in bytes of the MemorySize buffer (max.: 0xF)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcRequestDownload(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE CompressionMethod,
	BYTE EncryptingMethod,
	BYTE * MemoryAddress,
	BYTE MemoryAddressLength,
	BYTE * MemorySize,		
	BYTE MemorySizeLength);


// ISO-15765-3:2004 ?.7.1 p.57 && ISO-14229-1:2006 ?4.3 p.234
/// <summary>
///	The requestUpload service is used by the client to initiate a data transfer 
///	from the server to the client (upload).
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="CompressionMethod">A nibble-value that specifies the "compressionMethod",	
///	The value 0x0 specifies that no compressionMethod is used.</param>
/// <param name="EncryptingMethod">A nibble-value that specifies the "encryptingMethod",
///	The value 0x0 specifies that no encryptingMethod is used.</param>
/// <param name="MemoryAddress">starting address of server memory from which data is to be retrieved</param>
/// <param name="MemoryAddressLength">Size in bytes of the MemoryAddress buffer (max.: 0xF)</param>
/// <param name="MemorySize">used by the server to compare the uncompressed memory size with 
///	the total amount of data transferred during the TransferData service</param>
/// <param name="MemorySizeLength">Size in bytes of the MemorySize buffer (max.: 0xF)</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcRequestUpload(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE CompressionMethod,
	BYTE EncryptingMethod,
	BYTE * MemoryAddress,
	BYTE MemoryAddressLength,
	BYTE * MemorySize,		
	BYTE MemorySizeLength);


// ISO-15765-3:2004 ?.7.1 p.57 && ISO-14229-1:2006 ?4.4 p.237
/// <summary>
///	The TransferData service is used by the client to transfer data either from the client 
///	to the server (download) or from the server to the client (upload).
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="BlockSequenceCounter">The blockSequenceCounter parameter value starts at 01 hex
///	with the first TransferData request that follows the RequestDownload (34 hex) 
///	or RequestUpload (35 hex) service. Its value is incremented by 1 for each subsequent
///	TransferData request. At the value of FF hex, the blockSequenceCounter rolls over 
///	and starts at 00 hex with the next TransferData request message.</param>
/// <param name="Buffer">buffer containing the required transfer parameters</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcTransferData(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE BlockSequenceCounter,
	BYTE * Buffer,		
	WORD BufferLength);


// ISO-15765-3:2004 ?.7.1 p.57 && ISO-14229-1:2006 ?4.5 p.242
/// <summary>
///	The RequestTransferExit service is used by the client to terminate a data 
///	transfer between client and server (upload or download).
/// </summary>
/// <param name="CanChannel">A PUDS Channel Handle representing a PUDS-Client</param>
/// <param name="MessageBuffer">The PUDS message (NO_POSITIVE_RESPONSE_MSG is ignored)</param>
/// <param name="Buffer">buffer containing the required transfer parameters</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPUDSStatus code. PUDS_ERROR_OK is returned on success</returns>
TPUDSStatus __stdcall UDS_SvcRequestTransferExit(
	TPUDSCANHandle CanChannel,
	TPUDSMsg * MessageBuffer,
	BYTE * Buffer,		
	WORD BufferLength);

#ifdef __cplusplus
}
#endif


#endif __PCANUDS_H__