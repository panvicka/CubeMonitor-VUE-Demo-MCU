/*
 * utility.h
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#ifndef INC_LIB_UTILITY_H_
#define INC_LIB_UTILITY_H_

typedef enum {
	EOK = 0, EPERM = 1, // Operation not permitted
	EIO = 5, // IO error
	ENXIO = 6,  // No such device or address
	EACCES = 13, // Permission denied
	EFAULT = 14, // Bad address
	EBUSY = 16, // Device busy
	ENODEV = 19, // No such device
	EINVAL = 22, // Invalid argument
	EBADRQC = 56, // Invalid request code
	ECOMM = 70, // Communication error on send
	EOVERFLOW = 76, // Value too large for defined data type
	EMSGSIZE = 90, // Message too long

} ret_state;
#endif /* INC_LIB_UTILITY_H_ */
