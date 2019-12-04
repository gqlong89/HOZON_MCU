#ifndef UDS_DEFINE_H
#define UDS_DEFINE_H

//physical address or functional address
#define PHYSICAL_ADDRESS        0u
#define FUNCTIONAL_ADDRESS      1u

// Suppress Positive Response Message Indication Bit
#define SPRMIB               (0x80)

#define SESSION_TYPE_DEFAULT            (1)
#define SESSION_TYPE_PROGRAM            (2)
#define SESSION_TYPE_EXTENDED           (3)
#define SESSION_TYPE_UDSTEST            (0xAA)

#define SecurityAccess_LEVEL0           (0)
#define SecurityAccess_LEVEL1           (1)
#define SecurityAccess_LEVEL2           (2)

#define  POS_RESPOND_SID_MASK                    (0x40)

#define  SID_DiagnosticSeesionControl            (0x10)
#define  SID_ECUReset                            (0x11)
#define  SID_SecurityAccess                      (0x27)
#define  SID_CommunicationControl                (0x28)
#define  SID_TestPresent                         (0x3E)
#define  SID_AccessTimingParameter               (0x83)
#define  SID_SecuredDataTransmission             (0x84)
#define  SID_ControlDTCSetting                   (0x85)
#define  SID_ResponseOnEvent                     (0x86)
#define  SID_LinkControl                         (0x87)
#define  SID_ReadDataByIndentifier               (0x22)
#define  SID_ReadMemoryByAddress                 (0x23)
#define  SID_ReadScalingDataByIdentifier         (0x24)
#define  SID_ReadDataByPeriodicldentifier        (0x2A)
#define  SID_DynamicallyDefineDataIdentifier     (0x2C)
#define  SID_WriteDataByIdentifier               (0x2E)
#define  SID_WriteMemoryByAddress                (0x3D)
#define  SID_ClearDiagnosticInformation          (0x14)
#define  SID_ReadDTCInformation                  (0x19)
#define  SID_InputOutputControlByIdentifier      (0x2F)
#define  SID_RoutineControl                      (0x31)
#define  SID_RequestDownload                     (0x34)
#define  SID_RequestUpload                       (0x35)
#define  SID_TransferData                        (0x36)
#define  SID_RequestTransferExit                 (0x37)

#define  SID_NegativeResponse                    (0x7F)

#define NRC_GeneralReject                                (0x10)
#define NRC_ServiceNotSupported                          (0x11)
#define NRC_SubFuncationNotSupported                     (0x12)
#define NRC_IncorrectMessageLengthOrInvailFormat         (0x13)
#define NRC_ResponseTooLong                              (0x14)
#define NRC_BusyRepeatRequest                            (0x21)
#define NRC_ConditionsNotCorrect                         (0x22)
#define NRC_RequstSequenceError                          (0x24)
#define NRC_NoResponseFromSubnetComponent                (0x25)
#define NRC_FailurePreventsExecutionOfRequestedAction    (0x26)
#define NRC_RequestOutOfRange                            (0x31)
#define NRC_SecurityAccessDenied                         (0x33)
#define NRC_InvalidKey                                   (0x35)
#define NRC_ExceedNumberOfAttempts                       (0x36)
#define NRC_RequiredTimeDelayNotExpired                  (0x37)
#define NRC_UploadDownloadNotAccepted                    (0x70)
#define NRC_TransferDataSuspended                        (0x71)
#define NRC_GeneralProgrammingFailure                    (0x72)
#define NRC_wrongBlockSequenceCounter                    (0x73)
#define NRC_RequestCorrectlyReceivedResponsePending      (0x78)
#define NRC_SubFunctionNotSupportedInActiveSession       (0x7E)

#endif

