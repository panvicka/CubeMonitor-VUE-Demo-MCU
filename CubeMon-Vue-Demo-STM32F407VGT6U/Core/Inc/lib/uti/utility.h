/**
 * @file utility.h
 * @author panvicka
 * @date 2.1.2022
 * @brief Defines and functions that are useful for all library files
 *
 */

#ifndef INC_LIB_UTILITY_H_
#define INC_LIB_UTILITY_H_

/// possible return values
typedef enum retStatus {
	EOK = 0, ///<  operation OK
	EPERM = 1, ///<  Operation not permitted
	EIO = 5, ///<  IO error
	ENXIO = 6,  ///<  No such device or address
	EACCES = 13, ///<  Permission denied
	EFAULT = 14, ///<  Bad address (pointer)
	EBUSY = 16, ///<  Device busy
	ENODEV = 19, ///<  No such device
	EINVAL = 22, ///< Invalid argument
	EBADRQC = 56, ///<  Invalid request code
	ENODATA = 61, ///<  No data
	ECOMM = 70, ///<  Communication error on send
	EOVERFLOW = 76, ///<  Value too large for defined data type
	EMSGSIZE = 90, ///<  Message too long
} retStatus;

/// digital input/output states
typedef enum dioStates {
	DIO_OFF = 0, ///<  input/output is in logical 0 state
	DIO_ON = 1, ///<  input/output is in logical 1 state
	DIO_NOT_INITIALIZED = ENODEV, ///<  no information about the input/output state because it is not initialized
	DIO_PROBLEM_OCCURED = ENODATA ///<  no information about the input/output state due to an error
} dioStates;

/// digital input/output module data
typedef enum aioData {
	ANA_DIO_RAW_CURRENT, ///<  raw ADC/DAC value at this moment (for SMT32 12bit 0-4094)
	ANA_DIO_RAW_AVERAGED, ///<  raw ADC/DAC averaged value (for SMT32 12bit 0-4094)
	ANA_DIO_VOLTAGE  ///< ADC/DAC value converted to voltage
} aioData;

typedef uint32_t (*liner_fce)(uint32_t);

void initialization_error_handle(void);
#endif /* INC_LIB_UTILITY_H_ */
