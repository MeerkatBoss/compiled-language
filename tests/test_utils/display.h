/**
 * @file display.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-28
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __TESTS_TEST_UTILS_DISPLAY_H
#define __TESTS_TEST_UTILS_DISPLAY_H

/**
 * @brief Output progress bar on screen
 * 
 * @param[in] done	    - Number of completed events
 * @param[in] total	    - Total number of events to complete
 * @param[in] last_ms	- Number of seconds since last event (ignored if NaN)
 */
void progress_bar(size_t done, size_t total, double last_ms);

#endif /* display.h */
