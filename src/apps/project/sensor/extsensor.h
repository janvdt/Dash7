/* * OSS-7 - An opensource implementation of the DASH7 Alliance Protocol for ultra
 * lowpower wireless sensor communication
 *
 * Copyright 2015 University of Antwerp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "types.h"
#include "link_c.h"

/*!
 * /brief Returns the external temperature of the sensor
 */
__LINK_C void get_flow_meter();

__LINK_C void start_pump();

__LINK_C void stop_pump();

__LINK_C void init_temp_sensor();

__LINK_C uint32_t get_temp_sensor_value();

__LINK_C void set_flowvalue(int new_flowvalue);

__LINK_C int get_flowvalue();

__LINK_C void set_tempvalue();
__LINK_C int get_tempvalue();




