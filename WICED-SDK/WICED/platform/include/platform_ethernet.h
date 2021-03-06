/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
/** @file
 */
#pragma once

#include "wiced_result.h"
#include "wwd_buffer.h"

#include "platform_peripheral.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define PLATFORM_ETHERNET_SPEED_ADV(mode) (1 << PLATFORM_ETHERNET_SPEED_##mode)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    PLATFORM_ETHERNET_PHY_MII,
    PLATFORM_ETHERNET_PHY_RMII,
} platform_ethernet_phy_interface_t;

typedef enum
{
    PLATFORM_ETHERNET_SPEED_AUTO,
    PLATFORM_ETHERNET_SPEED_10FULL,
    PLATFORM_ETHERNET_SPEED_10HALF,
    PLATFORM_ETHERNET_SPEED_100FULL,
    PLATFORM_ETHERNET_SPEED_100HALF,
    PLATFORM_ETHERNET_SPEED_1000FULL,
    PLATFORM_ETHERNET_SPEED_1000HALF,
} platform_ethernet_speed_mode_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    wiced_mac_t                       mac_addr;
    uint8_t                           phy_addr;
    platform_ethernet_phy_interface_t phy_interface;
    uint16_t                          wd_period_ms;
    platform_ethernet_speed_mode_t    speed_force;
    uint32_t                          speed_adv;
} platform_ethernet_config_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

platform_result_t platform_ethernet_init                  ( void );
void              platform_ethernet_deinit                ( void );
platform_result_t platform_ethernet_send_data             ( wiced_buffer_t buffer );
platform_result_t platform_ethernet_get_config            ( platform_ethernet_config_t** config );
wiced_bool_t      platform_ethernet_is_ready_to_transceive( void );

#ifdef __cplusplus
} /*extern "C" */
#endif
