/**
 * @file swo.h
 * @author panvicka
 * @date 2.1.2022
 * @brief SWO messages handling
 *
 */

#ifndef INC_LIB_SWO_H_
#define INC_LIB_SWO_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/utility.h>

typedef enum swoType {
	SWO_NOT_ACTIVE, ///< SWO messages turned off
	SWO_PLAIN_MESSAGES, ///< SWO messages printed as they are
	SWO_RTC_SUPPORT, ///< SWO messages printed with date and time
	SWO_NONE, ///< dummy, so we always know the enum length
} swoType;


/**
 * @brief Initialize the SWO message type, without initialization the type defaults to /ref SWO_NOT_ACTIVE
 * @param[in] type type of the SWO messages
 * @return  /ref EOK if the initialization succeeded
 * 			/ref ENODEV in case the /p type is invalid
 */
retStatus swo_init(swoType type);

/**
 * @brief Print a SWO message
 * @param[in] text text to print
 * @note an @c \\n character to appended to each message to prevent SVN console bug
 */
void swo_print(char text[]);

#endif /* INC_LIB_SWO_H_ */
